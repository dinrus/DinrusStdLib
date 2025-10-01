#include <drx/Draw/Draw.h>

namespace drx {

Raster::Info::Info()
{
	bpp = 24;
	colors = 1 << 24;
	dots = Size(0, 0);
	hotspot = Point(0, 0);
	kind = IMAGE_OPAQUE;
	orientation = FLIP_NONE;
}

const RasterFormat *Raster::GetFormat()
{
	return NULL;
}

void Raster::SeekPage(i32 page)
{
	ASSERT(page == 0);
}

i32 Raster::GetActivePage() const
{
	return 0;
}

i32 Raster::GetPageCount()
{
	return 1;
}

i32 Raster::GetPageAspect(i32 page)
{
	return 0;
}

i32 Raster::GetPageDelay(i32 page)
{
	return 0;
}

Rect Raster::GetPageRect(i32 n)
{
	Size sz = GetSize();
	return Rect(0, 0, sz.cx, sz.cy);
}

i32 Raster::GetPageDisposal(i32 n)
{
	return 0;	
}

Val Raster::GetMetaData(Txt id)
{
	return Val();
}

void Raster::EnumMetaData(Vec<Txt>& id_list)
{
	id_list.Clear();
}

void Raster::Line::Pick(Line&& b)
{
	data = b.data;
	fmtdata = b.fmtdata;
	raster = b.raster;
	free = b.free;
	fmtfree = b.fmtfree;
	const_cast<Line *>(&b)->free = const_cast<Line *>(&b)->fmtfree = false;
#ifdef _DEBUG
	const_cast<Line *>(&b)->data = NULL;
	const_cast<Line *>(&b)->fmtdata = NULL;
#endif
}

void Raster::Line::MakeRGBA() const
{
	ASSERT(fmtdata && raster);
	i32 cx = raster->GetWidth();
	const RasterFormat *f = raster->GetFormat();
	if(f) {
		RGBA *rgba = new RGBA[cx];
		free = true;
		f->Read(rgba, fmtdata, cx, raster->GetPalette());
		data = rgba;
	}
	else
		data = (const RGBA *)fmtdata;
}

Raster::Info Raster::GetInfo()
{
	Info f;
	f.bpp = 32;
	f.colors = 256*256*256;
	f.dots = Size(0, 0);
	f.hotspot = Point(0, 0);
	f.kind = IMAGE_ALPHA;
	return f;
}

bool Raster::Create() { return true; }

bool Raster::IsError() { return false; }

i32   Raster::GetPaletteCount() { return 0; }

const RGBA *Raster::GetPalette() { return NULL; }

Image Raster::GetImage(i32 x, i32 y, i32 cx, i32 cy, const Gate<i32, i32> progress)
{
	Size size = GetSize();
	y = minmax(y, 0, size.cy);
	i32 yy = minmax(y + cy, y, size.cy);
	x = minmax(x, 0, size.cx);
	cx = minmax(x + cx, x, size.cx) - x;
	ImageBuffer b(cx, yy - y);
	RGBA* t = b;
	i32 y0 = y;
	while(y < yy) {
		if(progress(y - y0, yy - y0))
			return Null;
		memcpy_t(t, ~GetLine(y) + x, cx);
		t += cx;
		y++;
	}
	Info f = GetInfo();
	b.SetHotSpot(f.hotspot - Point(x, y0));
	if(size.cx && size.cy)
		b.SetDots(Size(f.dots.cx * cx / size.cx, f.dots.cy * cy / size.cy));
	b.SetKind(f.kind);

	if(IsError())
		return Image();
	
	Image img = b;
	return FlipImage(img, f.orientation);
}

Image Raster::GetImage(const Gate<i32, i32> progress)
{
	Size sz = GetSize();
	return GetImage(0, 0, sz.cx, sz.cy, progress);
}

Raster::~Raster() {}

Raster::Line ImageRaster::GetLine(i32 line)
{
	return Line(img[line], false);
}

Size ImageRaster::GetSize()
{
	return img.GetSize();
}

Raster::Info ImageRaster::GetInfo()
{
	Raster::Info f = Raster::GetInfo();
	f.dots = img.GetDots();
	f.hotspot = img.GetHotSpot();
	f.kind = img.GetKind();
	return f;
}

MemoryRaster::MemoryRaster()
: size(0, 0)
{
}

void MemoryRaster::Load(Raster& raster)
{
	info = raster.GetInfo();
	size = raster.GetSize();
	palette.SetCount(raster.GetPaletteCount());
	if(!palette.IsEmpty())
		memcpy_t(palette.begin(), raster.GetPalette(), palette.GetCount());
	lines.SetCount(size.cy);
	if(const RasterFormat *fmt = raster.GetFormat()) {
		format = *fmt;
		i32 rowbytes = format.GetByteCount(size.cx);
		for(i32 i = 0; i < size.cy; i++) {
			lines[i].Alloc(rowbytes);
			memcpy_t(~lines[i], raster.GetLine(i).GetRawData(), rowbytes);
		}
	}
	else {
		format.SetRGBA();
		i32 rowbytes = sizeof(RGBA) * size.cx;
		for(i32 i = 0; i < size.cy; i++) {
			lines[i].Alloc(rowbytes);
			memcpy_t((RGBA *)~lines[i], raster.GetLine(i).GetRGBA(), rowbytes);
		}
	}
}

Raster::Line MemoryRaster::GetLine(i32 line)
{
	if(format.IsRGBA())
		return Line((const RGBA *)~lines[line], false);
	else
		return Line(~lines[line], this, false);
}

i32 MemoryRaster::GetLength() const
{
	return size.cy * (format.IsRGBA()
		? size.cx * sizeof(RGBA)
		: ((size.cx * info.bpp + 31) >> 5) * 4);
}

bool StreamRaster::Open(Stream& _s)
{
	s = &_s;
	error = !Create();
	return !error;
}

bool StreamRaster::IsError()
{
	return error || !s || s->IsError();
}

Image StreamRaster::Load(Stream& s, const Gate<i32, i32> progress)
{
	if(Open(s)) {
		Image img = GetImage(progress);
		if(!IsError())
			return img;
	}
	return Image();
}

Image StreamRaster::LoadFile(tukk fn, const Gate<i32, i32> progress)
{
	FileIn in(fn);
	return in ? Load(in, progress) : Image();
}

Image StreamRaster::LoadTxt(const Txt& s, const Gate<i32, i32> progress)
{
	TxtStream ss(s);
	return Load(ss, progress);
}

static StaticCriticalSection sAnyRaster;

Vec<uk >& StreamRaster::Map()
{
	static Vec<uk > x;
	return x;
}

void StreamRaster::AddFormat(RasterFactory factory)
{
	INTERLOCKED_(sAnyRaster)
		Map().Add((uk )factory);
}

One<StreamRaster> StreamRaster::OpenAny(Stream& s)
{
	INTERLOCKED_(sAnyRaster)
		for(i32 i = 0; i < Map().GetCount(); i++) {
			z64 p = s.GetPos();
			One<StreamRaster> raster = (*RasterFactory(Map()[i]))();
			s.ClearError();
			if(raster->Open(s))
				return raster;
			s.ClearError();
			s.Seek(p);
		}
	return NULL;
}

Image StreamRaster::LoadAny(Stream& s, Gate<i32, i32> progress)
{
	One<StreamRaster> r = OpenAny(s);
	return r ? r->GetImage(progress) : Image();
}

Image StreamRaster::LoadFileAny(tukk fn, Gate<i32, i32> progress)
{
	FileIn in(fn);
	return LoadAny(in, progress);
}

Image StreamRaster::LoadTxtAny(const Txt& s, Gate<i32, i32> progress)
{
	TxtStream ss(s);
	return LoadAny(ss, progress);
}

}
