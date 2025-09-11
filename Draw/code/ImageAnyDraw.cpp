#include <drx/Draw/Draw.h>

namespace drx {

dword ImageAnyDraw::GetInfo() const
{
	return draw->GetInfo();
}

Size ImageAnyDraw::GetPageSize() const
{
	return draw->GetPageSize();
}

void ImageAnyDraw::BeginOp()
{
	return draw->BeginOp();
}

void ImageAnyDraw::EndOp()
{
	return draw->EndOp();
}

void ImageAnyDraw::OffsetOp(Point p)
{
	return draw->OffsetOp(p);
}

bool ImageAnyDraw::ClipOp(const Rect& r)
{
	return draw->ClipOp(r);
}

bool ImageAnyDraw::ClipoffOp(const Rect& r)
{
	return draw->ClipoffOp(r);
}

bool ImageAnyDraw::ExcludeClipOp(const Rect& r)
{
	return draw->ExcludeClipOp(r);
}

bool ImageAnyDraw::IntersectClipOp(const Rect& r)
{
	return draw->IntersectClipOp(r);
}

bool ImageAnyDraw::IsPaintingOp(const Rect& r) const
{
	return draw->IsPaintingOp(r);
}

Rect ImageAnyDraw::GetPaintRect() const
{
	return draw->GetPaintRect();
}

void ImageAnyDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	draw->DrawRectOp(x, y, cx, cy, color);
}

void ImageAnyDraw::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color)
{
	draw->DrawImageOp(x, y, cx, cy, img, src, color);
}

void ImageAnyDraw::DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id)
{
	draw->DrawDataOp(x, y, cx, cy, data, id);
}

void ImageAnyDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	draw->DrawLineOp(x1, y1, x2, y2, width, color);
}

void ImageAnyDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
                                      i32k *counts, i32 count_count, i32 width,
                                      Color color, Color doxor)
{
	draw->DrawPolyPolylineOp(vertices, vertex_count, counts, count_count, width, color, doxor);
}

void ImageAnyDraw::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count, i32k *subpolygon_counts, i32 scc, i32k *disjunct_polygon_counts, i32 dpcc, Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	draw->DrawPolyPolyPolygonOp(vertices, vertex_count, subpolygon_counts, scc,
	                            disjunct_polygon_counts, dpcc, color, width, outline,
	                            pattern, doxor);
}

void ImageAnyDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	draw->DrawArcOp(rc, start, end, width, color);
}

void ImageAnyDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor)
{
	draw->DrawEllipseOp(r, color, pen, pencolor);
}

void ImageAnyDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink,
                              i32 n, i32k *dx)
{
	draw->DrawTextOp(x, y, angle, text, font, ink, n, dx);
}

void ImageAnyDraw::DrawDrawingOp(const Rect& target, const Drawing& w)
{
	draw->DrawDrawingOp(target, w);
}

void ImageAnyDraw::DrawPaintingOp(const Rect& target, const Painting& w)
{
	draw->DrawPaintingOp(target, w);
}

Draw *(*sCreateImageDraw)(Size sz);
Image (*sExtractImageDraw)(Draw *w);

void ImageAnyDrawPainter(Draw *(*f)(Size sz), Image (*e)(Draw *w))
{
	sCreateImageDraw = f;
	sExtractImageDraw = e;
}

void ImageAnyDrawSystem(Draw *(*f)(Size sz), Image (*e)(Draw *w))
{
	if(!sCreateImageDraw) {
		sCreateImageDraw = f;
		sExtractImageDraw = e;
	}
}

bool ImageAnyDraw::IsAvailable()
{
	return sCreateImageDraw;
}

void ImageAnyDraw::Init(Size sz)
{
	ASSERT(sCreateImageDraw);
	draw = (*sCreateImageDraw)(sz);
	ASSERT(draw);
}

ImageAnyDraw::operator Image() const
{
	return (*sExtractImageDraw)(draw);
}

ImageAnyDraw::ImageAnyDraw(Size sz)
{
	Init(sz);
}

ImageAnyDraw::ImageAnyDraw(i32 cx, i32 cy)
{
	Init(Size(cx, cy));
}

ImageAnyDraw::~ImageAnyDraw()
{
	delete draw;
}

}
