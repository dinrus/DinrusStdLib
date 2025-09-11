#include <drx/Draw/Draw.h>

namespace drx {

struct cDrawRasterData : DataDrawer {
	i32                cx;
	TxtStream       ss;
	One<StreamRaster>  raster;
	RescaleImage       si;

	virtual void Open(const Txt& data, i32 cx, i32 cy);
	virtual void Render(ImageBuffer& ib);
};

void cDrawRasterData::Open(const Txt& data, i32 _cx, i32 cy)
{
	cx = _cx;
	ss.Open(data);
	raster = StreamRaster::OpenAny(ss);
	if(raster)
		si.Create(Size(cx, cy), *raster, raster->GetSize());
}

void cDrawRasterData::Render(ImageBuffer& ib)
{
	for(i32 y = 0; y < ib.GetHeight(); y++)
		si.Get(ib[y]);
}

INITBLOCK
{
	DataDrawer::Register<cDrawRasterData>("image_data");
};

void DrawRasterData(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& data)
{
	w.DrawData(x, y, cx, cy, data, "image_data");
}

}
