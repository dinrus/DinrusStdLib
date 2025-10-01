#include "IconDes.h"

namespace drx {

void IconDraw::PutHorz(i32 x, i32 y, i32 cx)
{
	if(y < 0 || y >= image.GetHeight())
		return;
	i32 r = minmax(x + cx, 0, image.GetWidth());
	x = minmax(x, 0, image.GetWidth());
	cx = r - x;
	if(cx <= 0)
		return;
	drx::Fill(image[y] + x, docolor, cx);
}

void IconDraw::PutVert(i32 x, i32 y, i32 cy)
{
	if(x < 0 || x >= image.GetWidth())
		return;
	i32 b = minmax(y + cy, 0, image.GetHeight());
	y = minmax(y, 0, image.GetHeight());
	cy = b - y;
	if(cy <= 0)
		return;
	RGBA *t = image[y] + x;
	while(cy-- > 0) {
		*t = docolor;
		t += image.GetWidth();
	}
}

void IconDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	docolor = color;
	i32 b = minmax(y + cy, 0, image.GetHeight());
	y = minmax(y, 0, image.GetHeight());
	cy = b - y;
	while(cy-- > 0)
		PutHorz(x, y++, cx);
}

void IconDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	docolor = color;
	Width(width);
	Move(Point(x1, y1));
	Line(Point(x2, y2));
}

void IconDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor)
{
	if(!IsNull(color)) {
		docolor = color;
		Polygon().Ellipse(r).Fill();
	}
	if(!IsNull(pen) && !IsNull(pencolor)) {
		docolor = pencolor;
		Width(pen);
		Ellipse(r);
	}
}

}
