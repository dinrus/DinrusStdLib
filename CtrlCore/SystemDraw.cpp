#include "CtrlCore.h"

namespace drx {

#define LTIMING(x)

ImageBuffer::ImageBuffer(ImageDraw& iw)
{
	Image m = iw;
	Set(m);
}

void ImageAnyDrawSystem(Draw *(*f)(Size sz), Image (*e)(Draw *w));

static Draw *sCD(Size sz)
{
	return new ImageDraw(sz);
}

static Image sED(Draw *w)
{
	ImageDraw *ip = dynamic_cast<ImageDraw *>(w);
	return ip ? (Image)(*ip) : Image();
}

INITBLOCK {
	ImageAnyDrawSystem(sCD, sED);
}

void SetSurface(SystemDraw& w, const Rect& dest, const RGBA *pixels, Size psz, Point poff);

void SetSurface(Draw& w, const Rect& dest, const RGBA *pixels, Size srcsz, Point poff)
{
	SystemDraw *sw = dynamic_cast<SystemDraw *>(&w);
	if(sw && sw->CanSetSurface())
		SetSurface(*sw, dest, pixels, srcsz, poff);
	else {
		ImageBuffer ib(dest.GetWidth(), dest.GetHeight());
		for(i32 i = 0; i < ib.GetHeight(); i++) {
			i32 sl = poff.y + i;
			if(i >= 0 && i < srcsz.cy)
				Copy(ib[i], pixels + srcsz.cx * sl + poff.x,
				     minmax(srcsz.cx - poff.x, 0, min(dest.GetWidth(), srcsz.cx)));
		}
		w.DrawImage(dest.left, dest.top, ib);
	}
}

void SetSurface(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const RGBA *pixels)
{
	SetSurface(w, RectC(x, y, cx, cy), pixels, Size(cx, cy), Point(0, 0));
}

SystemDraw& ScreenInfo();

bool BackDraw::IsPaintingOp(const Rect& r) const
{
	Rect rr = r + GetOffset();
	if(!rr.Intersects(size))
		return false;
	return painting ? painting->IsPainting(rr + painting_offset) : true;
}

BackDraw::BackDraw()
{
	painting = NULL;
	painting_offset = Point(0, 0);
}

BackDraw::~BackDraw()
{
	Destroy();
}

}
