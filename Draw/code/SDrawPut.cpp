#include <drx/Draw/Draw.h>

namespace drx {

void SDraw::PutImage(Point p, const Image& img, const Rect& src, Color color)
{
	PutImage(p, (paintonly ? CachedSetColorKeepAlphaPaintOnly : CachedSetColorKeepAlpha)
	               (img, color), src);
}

void SDraw::SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color)
{
	Rect sr(Point(x, y) + cloff.Top().offset, (src & img.GetSize()).GetSize());
	const Vec<Rect>& clip = cloff.Top().clip;
	for(i32 i = 0; i < clip.GetCount(); i++) {
		Rect cr = clip[i] & sr;
		if(!cr.IsEmpty()) {
			Point p = cr.TopLeft();
			Rect r(cr.TopLeft() - sr.TopLeft() + src.TopLeft(), cr.GetSize());
			if(IsNull(color))
				PutImage(p, img, r);
			else
				PutImage(p, img, r, color);
		}
	}
}

void SDraw::SysDrawImageOp(i32 x, i32 y, const Image& img, Color color)
{
	SysDrawImageOp(x, y, img, img.GetSize(), color);
}

void SDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	if(IsNull(color))
		return;
	Rect r = RectC(x, y, cx, cy);
	r += cloff.Top().offset;
	const Vec<Rect>& clip = cloff.Top().clip;
	for(i32 i = 0; i < clip.GetCount(); i++) {
		Rect cr = clip[i] & r;
		if(!cr.IsEmpty())
			PutRect(cr, color);
	}
}

void SDraw::PutHorz(i32 x, i32 y, i32 cx)
{
	DrawRect(x, y, cx, 1, docolor);
}

void SDraw::PutVert(i32 x, i32 y, i32 cy)
{
	DrawRect(x, y, 1, cy, docolor);
}

}
