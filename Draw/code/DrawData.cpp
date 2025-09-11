#include <drx/Draw/Draw.h>

namespace drx {

#define LTIMING(x)
// #define BENCHMARK_RLE

VecMap<Txt, uk >& DataDrawer::Map()
{
	static VecMap<Txt, uk > x;
	return x;
}

static StaticCriticalSection sDataDrawer;

void DataDrawer::AddFormat(tukk id, Factory factory)
{
	Mutex::Lock __(sDataDrawer);
	Map().Add(id, (uk )factory);
}

One<DataDrawer> DataDrawer::Create(const Txt& id)
{
	Mutex::Lock __(sDataDrawer);
	Factory q = (Factory) Map().Get(id, NULL);
	if(q)
		return (*q)();
	return NULL;
}

bool IsWhiteColumn(const Image& m, i32 x)
{
	LTIMING("IsEqColumn");
	Size sz = m.GetSize();
	const RGBA *s = ~m + x;
	while(sz.cy > 1) {
		s += sz.cx;
		if((s->a & s->r & s->g & s->b) != 255)
			return false;
		sz.cy--;
	}
	return true;
}


#ifdef BENCHMARK_RLE
static i32 sTotal;
static i32 sRle;

EXITBLOCK
{
	DUMP(sTotal);
	DUMP(sRle);
}
#endif

void DrawImageBandRLE(Draw& w, i32 x, i32 y, const Image& m, i32 minp)
{
	i32 xi = 0;
	i32 cx = m.GetWidth();
	i32 ccy = m.GetHeight();
	Buffer<bool> todo(cx, true);
#ifdef BENCHMARK_RLE
	sTotal += cx;
#endif
	while(xi < cx) {
		i32 xi0 = xi;
		while(w.Dots() && IsWhiteColumn(m, xi) && xi < cx)
			xi++;
		if(xi - xi0 >= 16) {
#ifdef BENCHMARK_RLE
			sRle += xi - xi0;
#endif
			w.DrawRect(x + xi0, y, xi - xi0, ccy, White);
			Fill(~todo + xi0, ~todo + xi, false);
		}
		xi++;
	}
	
	xi = 0;
	while(xi < cx)
		if(todo[xi]) {
			i32 xi0 = xi;
			while(xi < cx && todo[xi] && xi - xi0 < 2000)
				xi++;
			w.DrawImage(x + xi0, y, m, RectC(xi0, 0, xi - xi0, ccy));
		}
		else
			xi++;
}

void Draw::DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id)
{
	bool tonative = !IsNative();
	if(tonative) {
		BeginNative();
		Native(x, y);
		Native(cx, cy);
	}
	One<DataDrawer> dd = DataDrawer::Create(id);
	if(dd) {
		dd->Open(data, cx, cy);
		if((cx > 2048 || cy > 2048) && (GetInfo() & DATABANDS)) {
			i32 yy = 0;
			while(yy < cy) {
				i32 ccy = min(cy - yy, 32); // Must be multiply of 4 because of dithering
				ImageBuffer ib(cx, ccy);
				dd->Render(ib);
				DrawImageBandRLE(*this, x, y + yy, ib, 16);
				yy += ccy;
			}
		}
		else {
			ImageBuffer m(cx, cy);
			dd->Render(m);
			DrawImage(x, y, m);
		}
	}
	if(tonative)
		EndNative();
}

DataDrawer::~DataDrawer() {}

}
