#include <drx/Draw/Draw.h>

namespace drx {

#define LLOG(x)    // RLOG(x)
#define LTIMING(x) // RTIMING(x)
//#define BENCH

static StaticMutex sDrawLock;

INITBLOCK {
	Val::Register<Painting>("Painting");
	Val::Register<Drawing>("Drawing");
}

Color Draw::GetDefaultInk() const
{
	dword w = GetInfo();
	if(w & DRAWING)
		return DefaultInk();
	if(w & DOTS)
		return Black();
	return SColorText();
}

void Draw::SinCos(i32 angle, double& sina, double& cosa)
{
	angle = angle % 3600;
	switch(angle) {
	case    0: sina =  0; cosa =  1; break;
	case  900: sina =  1; cosa =  0; break;
	case 1800: sina =  0; cosa = -1; break;
	case 2700: sina = -1; cosa =  0; break;
	default:
		double a = angle * M_PI / 1800.0;
		sina = sin(a);
		cosa = cos(a);
		break;
	}
}

Draw::~Draw() {}

Size Draw::GetPixelsPerInch() const
{
	return IsNative() ? GetNativeDpi() : Dots() ? Size(600, 600) : Size(96, 96);
}

Size Draw::GetPageMMs() const
{
	return GetPageSize() * 254 / (10 * GetPixelsPerInch());
}

i32 Draw::GetNativeX(i32 x) const
{
	Size sz = GetNativeDpi();
	return Dots() && sz.cx != 600 ? iscale(x, sz.cx, 600) : x;
}

i32 Draw::GetNativeY(i32 y) const
{
	Size sz = GetNativeDpi();
	return Dots() && sz.cx != 600 ? iscale(y, sz.cy, 600) : y;
}

void Draw::Native(i32& x, i32& y) const
{
	x = GetNativeX(x);
	y = GetNativeY(y);
}

void Draw::Native(Point& p) const
{
	Native(p.x, p.y);
}

void Draw::Native(Size& sz) const
{
	Native(sz.cx, sz.cy);
}

void Draw::Native(Rect& r) const
{
	Native(r.left, r.top);
	Native(r.right, r.bottom);
}

#ifdef BENCH
static TimingInspector sDrawTiming("DRAW");
#endif

void EnterDraw() {
	sDrawLock.Enter();
#ifdef BENCH
	sDrawTiming.Start();
#endif
}

void LeaveDraw() {
#ifdef BENCH
	sDrawTiming.End();
#endif
	sDrawLock.Leave();
}

Size Draw::GetPageSize() const
{
	return Size(INT_MAX / 2, INT_MAX / 2);
}

Rect Draw::GetPaintRect() const
{
	return Rect(-(INT_MAX / 2), -(INT_MAX / 2), INT_MAX / 2, INT_MAX / 2);
}


void Draw::StartPage() {}
void Draw::EndPage() {}

Size Draw::GetNativeDpi() const { return Dots() ? Size(600, 600) : Size(96, 96); }
void Draw::BeginNative() {}
void Draw::EndNative() {}

i32 Draw::GetCloffLevel() const { return 0; }

void Draw::Escape(const Txt& data) {}

// -------------------------------

void Draw::SysDrawImageOp(i32 x, i32 y, const Image& img, Color color)
{
	NEVER();
}

void Draw::SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color)
{
	if(src == Rect(img.GetSize()))
		SysDrawImageOp(x, y, img, color);
	else {
		Clipoff(x, y, src.GetWidth(), src.GetHeight());
		SysDrawImageOp(-src.left, -src.top, img, color);
		End();
	}
}

void Draw::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color)
{
	LTIMING("DrawImageOp");
	LLOG("DrawImageOp " << RectC(x, y, cx, cy) << ", imgsize: " << img.GetSize() << ", src: " << src << ", color: " << color);
	bool tonative = !IsNative();
	if(tonative) {
		BeginNative();
		Native(x, y);
		Native(cx, cy);
	}
	Size sz = Size(cx, cy);
	if((cx > 2000 || cy > 1500) && IsNull(color) && IsPrinter()) {
		i32 yy = 0;
		ImageRaster ir(img);
		RescaleImage rm;
		rm.Create(Size(cx, cy), ir, src);
		while(yy < cy) {
			i32 ccy = min(cy - yy, 16);
			ImageBuffer ib(cx, ccy);
			for(i32 q = 0; q < ccy; q++)
				rm.Get(ib[q]);
			DrawImageBandRLE(*this, x, y + yy, ib, 16);
			yy += ccy;
		}
	}
	else
	if(src.GetSize() == sz)
		SysDrawImageOp(x, y, img, src, color);
	else {
		Image h = Rescale(img, Size(cx, cy), src);
		SysDrawImageOp(x, y, h, h.GetSize(), color);
	}
	if(tonative)
		EndNative();
}

// -------------------------------

void Draw::DrawRect(const Rect& rect, Color color)
{
	DrawRect(rect.left, rect.top, rect.GetWidth(), rect.GetHeight(), color);
}

void Draw::DrawRect(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	DrawRectOp(x, y, cx, cy, color);
}

void Draw::DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src)
{
	DrawImageOp(x, y, cx, cy, img, src, Null);
}

void Draw::DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img)
{
	DrawImage(x, y, cx, cy, img, img.GetSize());
}

void Draw::DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color)
{
	if(IsNull(color)) return;
	DrawImageOp(x, y, cx, cy, img, src, color);
}

void Draw::DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, Color color)
{
	if(IsNull(color)) return;
	DrawImage(x, y, cx, cy, img, img.GetSize(), color);
}

void Draw::DrawImage(const Rect& r, const Image& img, const Rect& src)
{
	DrawImage(r.left, r.top, r.Width(), r.Height(), img, src);
}

void Draw::DrawImage(const Rect& r, const Image& img)
{
	DrawImage(r.left, r.top, r.Width(), r.Height(), img);
}

void Draw::DrawImage(const Rect& r, const Image& img, const Rect& src, Color color)
{
	if(IsNull(color)) return;
	DrawImage(r.left, r.top, r.Width(), r.Height(), img, src, color);
}

void Draw::DrawImage(const Rect& r, const Image& img, Color color)
{
	if(IsNull(color)) return;
	DrawImage(r.left, r.top, r.Width(), r.Height(), img, color);
}

void Draw::DrawImage(i32 x, i32 y, const Image& img, const Rect& src)
{
	Size sz = src.GetSize();
	DrawImageOp(x, y, sz.cx, sz.cy, img, src, Null);
}

void Draw::DrawImage(i32 x, i32 y, const Image& img)
{
	Size sz = img.GetSize();
	DrawImageOp(x, y, sz.cx, sz.cy, img, img.GetSize(), Null);
}

void Draw::DrawImage(i32 x, i32 y, const Image& img, const Rect& src, Color color)
{
	if(IsNull(color)) return;
	DrawImageOp(x, y, src.Width(), src.Height(), img, src, color);
}

void Draw::DrawImage(i32 x, i32 y, const Image& img, Color color)
{
	if(IsNull(color)) return;
	Size sz = img.GetSize();
	DrawImageOp(x, y, sz.cx, sz.cy, img, sz, color);
}

void Draw::DrawData(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk type)
{
	DrawDataOp(x, y, cx, cy, data, type);
}

void Draw::DrawData(const Rect& r, const Txt& data, tukk type)
{
	DrawDataOp(r.left, r.top, r.GetWidth(), r.GetHeight(), data, type);
}

void Draw::DrawLine(Point p1, Point p2, i32 width, Color color)
{
	DrawLine(p1.x, p1.y, p2.x, p2.y, width, color);
}

void Draw::DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	DrawLineOp(x1, y1, x2, y2, width, ResolveInk(color));
}

void Draw::DrawPolyPolyline(const Point *vertices, i32 vertex_count,
                            i32k *counts, i32 count_count,
                            i32 width, Color color, Color doxor)
{
	DrawPolyPolylineOp(vertices, vertex_count, counts, count_count, width,
	                   ResolveInk(color), doxor);
}

void Draw::DrawPolyPolyline(const Vec<Point>& vertices, const Vec<i32>& counts,
                            i32 width, Color color, Color doxor)
{
	DrawPolyPolyline(vertices.Begin(), vertices.GetCount(),
		             counts.Begin(), counts.GetCount(),
		             width, color, doxor);
}

void Draw::DrawPolyline(const Point *vertices, i32 count,
                        i32 width, Color color, Color doxor)
{
	DrawPolyPolyline(vertices, count, &count, 1, width, color, doxor);
}

void Draw::DrawPolyline(const Vec<Point>& vertices,
                        i32 width, Color color, Color doxor)
{
	DrawPolyline(vertices.Begin(), vertices.GetCount(), width, color, doxor);
}

void Draw::DrawPolyPolyPolygon(const Point *vertices, i32 vertex_count,
                              i32k *subpolygon_counts, i32 subpolygon_count_count,
                              i32k *disjunct_polygon_counts,
                              i32 disjunct_polygon_count_count, Color color,
                              i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolyPolyPolygonOp(vertices, vertex_count, subpolygon_counts, subpolygon_count_count,
	                      disjunct_polygon_counts, disjunct_polygon_count_count, ResolveInk(color),
	                      width, outline, pattern, doxor);
}

void Draw::DrawPolyPolyPolygon(const Vec<Point>& vertices,
                           const Vec<i32>& subpolygon_counts,
                           const Vec<i32>& disjunct_polygon_counts,
                           Color color, i32 width, Color outline,
                           zu64 pattern, Color doxor)
{
	DrawPolyPolyPolygon(vertices.Begin(), vertices.GetCount(),
	                    subpolygon_counts.Begin(), subpolygon_counts.GetCount(),
	                    disjunct_polygon_counts.Begin(), disjunct_polygon_counts.GetCount(),
	                    color, width, outline, pattern, doxor);
}

void Draw::DrawPolyPolygon(const Point *vertices, i32 vertex_count,
                       i32k *subpolygon_counts, i32 subpolygon_count_count,
                       Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolyPolyPolygon(vertices, vertex_count,
	                    subpolygon_counts, subpolygon_count_count, &vertex_count, 1,
	                    color, width, outline, pattern, doxor);
}

void Draw::DrawPolyPolygon(const Vec<Point>& vertices, const Vec<i32>& subpolygon_counts,
                       Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolyPolygon(vertices.Begin(), vertices.GetCount(),
	                subpolygon_counts.Begin(), subpolygon_counts.GetCount(),
	                color, width, outline, pattern, doxor);
}

void Draw::DrawPolygons(const Point *vertices, i32 vertex_count,
                    i32k *polygon_counts, i32 polygon_count_count,
                    Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolyPolyPolygon(vertices, vertex_count,
	                    polygon_counts, polygon_count_count,
	                    polygon_counts, polygon_count_count,
	                    color, width, outline, pattern, doxor);
}

void Draw::DrawPolygons(const Vec<Point>& vertices, const Vec<i32>& polygon_counts,
                    Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolygons(vertices.Begin(), vertices.GetCount(),
                 polygon_counts.Begin(), polygon_counts.GetCount(),
                 color, width, outline, pattern, doxor);
}

void Draw::DrawPolygon(const Point *vertices, i32 vertex_count,
                   Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolyPolyPolygon(vertices, vertex_count, &vertex_count, 1, &vertex_count, 1,
	                    color, width, outline, pattern, doxor);
}

void Draw::DrawPolygon(const Vec<Point>& vertices,
                       Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	DrawPolygon(vertices.Begin(), vertices.GetCount(), color, width, outline, pattern, doxor);
}

void Draw::DrawEllipse(i32 x, i32 y, i32 cx, i32 cy, Color color, i32 pen, Color pencolor)
{
	DrawEllipse(RectC(x, y, cx, cy), color, pen, pencolor);
}

void Draw::DrawEllipse(const Rect& r, Color color, i32 pen, Color pencolor)
{
	DrawEllipseOp(r, ResolveInk(color), pen, ResolveInk(pencolor));
}

void Draw::DrawArc(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	DrawArcOp(rc, start, end, width, ResolveInk(color));
}

void Draw::Offset(i32 x, i32 y)
{
	Offset(Point(x, y));
}

bool Draw::Clip(i32 x, i32 y, i32 cx, i32 cy)
{
	return Clip(RectC(x, y, cx, cy));
}

bool Draw::Clipoff(i32 x, i32 y, i32 cx, i32 cy)
{
	return Clipoff(RectC(x, y, cx, cy));
}

bool Draw::ExcludeClip(i32 x, i32 y, i32 cx, i32 cy)
{
	return ExcludeClip(RectC(x, y, cx, cy));
}

bool Draw::IntersectClip(i32 x, i32 y, i32 cx, i32 cy)
{
	return IntersectClip(RectC(x, y, cx, cy));
}

bool Draw::IsPainting(i32 x, i32 y, i32 cx, i32 cy) const
{
	return IsPainting(RectC(x, y, cx, cy));
}

static void  (*sIgfn)(ImageBuffer& ib, const Painting& pw, Size sz, Point pos, i32 mode);
static void  (*sIwfn)(ImageBuffer& ib, const Drawing& p, i32 mode);
static Image (*sRG)(Point at, i32 angle, i32 chr, Font fnt, Color color, Size sz);

void RegisterPaintingFns__(void (*ig)(ImageBuffer& ib, const Painting& pw, Size sz, Point pos, i32 mode),
                           void (*iw)(ImageBuffer& ib, const Drawing& p, i32 mode),
                           Image (*rg)(Point at, i32 angle, i32 chr, Font fnt, Color color, Size sz))
{
	sIgfn = ig;
	sIwfn = iw;
	sRG = rg;
}

bool HasPainter()
{
	return sIgfn && sIwfn && sRG;
}

Image RenderGlyphByPainter(Point at, i32 angle, i32 chr, Font fnt, Color color, Size sz)
{
	return sRG ? (*sRG)(at, angle, chr, fnt, color, sz) : Image();
}

void PaintImageBuffer(ImageBuffer& ib, const Painting& p, Size sz, Point pos, i32 mode)
{
	if(sIgfn)
		(*sIgfn)(ib, p, sz, pos, mode);
}

void PaintImageBuffer(ImageBuffer& ib, const Painting& p, i32 mode)
{
	PaintImageBuffer(ib, p, ib.GetSize(), Point(0, 0), mode);
}

void PaintImageBuffer(ImageBuffer& ib, const Drawing& iw, i32 mode)
{
	if(sIwfn)
		(*sIwfn)(ib, iw, mode);
}

Drawing AsDrawing(const Painting& pw)
{
	Size sz = pw.GetSize();
	DrawingDraw dw(sz);
	dw.DrawPainting(sz, pw);
	return dw.GetResult();
}

void Draw::DrawPaintingOp(const Rect& target, const Painting& pw)
{
	if(!HasPainter())
		return;
	Size sz = target.GetSize();
	if(sz.cx > 2000 || sz.cy > 1500) {
		i32 yy = 0;
		while(yy < sz.cy) {
			i32 ccy = min(sz.cy - yy, 100);
			ImageBuffer ib(sz.cx, ccy);
			Fill(~ib, IsPrinter() ? White() : Null, ib.GetLength());
			PaintImageBuffer(ib, pw, sz, Point(0, yy), MODE_ANTIALIASED);
			DrawImageBandRLE(*this, target.left, target.top + yy, ib, 16);
			yy += ccy;
		}
	}
	else {
		ImageBuffer ib(sz);
		Fill(~ib, IsPrinter() ? White() : Null, ib.GetLength());
		PaintImageBuffer(ib, pw, sz, Point(0, 0), MODE_ANTIALIASED);
		DrawImage(target.left, target.top, ib);
	}
}

void Draw::DrawPainting(i32 x, i32 y, i32 cx, i32 cy, const Painting& ig)
{
	DrawPainting(RectC(x, y, cx, cy), ig);
}

void Draw::DrawPainting(i32 x, i32 y, const Painting& iw)
{
	Size sz = iw.GetSize();
	DrawPainting(RectC(x, y, sz.cx, sz.cy), iw);
}

// ---------------------------

dword NilDraw::GetInfo() const { return DOTS; }
Size NilDraw::GetPageSize() const { return Size(0, 0); }
void NilDraw::BeginOp() {}
void NilDraw::EndOp() {}
void NilDraw::OffsetOp(Point p) {}
bool NilDraw::ClipOp(const Rect& r) { return false; }
bool NilDraw::ClipoffOp(const Rect& r) { return false; }
bool NilDraw::ExcludeClipOp(const Rect& r) { return false; }
bool NilDraw::IntersectClipOp(const Rect& r) { return false; }
bool NilDraw::IsPaintingOp(const Rect& r) const { return false; }
Rect NilDraw::GetPaintRect() const { return Rect(0, 0, 0, 0); }
void NilDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color) {}
void NilDraw::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color) {}
void NilDraw::DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id) {}
void NilDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color) {}
void NilDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count, i32k *counts, i32 count_count, i32 width, Color color, Color doxor) {}
void NilDraw::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count, i32k *subpolygon_counts, i32 scc, i32k *disjunct_polygon_counts, i32 dpcc, Color color, i32 width, Color outline, zu64 pattern, Color doxor) {}
void NilDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color) {}
void NilDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor) {}
void NilDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink, i32 n, i32k *dx) {}
void NilDraw::DrawDrawingOp(const Rect& target, const Drawing& w) {}
void NilDraw::DrawPaintingOp(const Rect& target, const Painting& w) {}

dword DrawProxy::GetInfo() const
{
	return ptr->GetInfo();
}

Size DrawProxy::GetPageSize() const
{
	return ptr->GetPageSize();
}

void DrawProxy::StartPage()
{
	ptr->StartPage();
}

void DrawProxy::EndPage()
{
	ptr->EndPage();
}

void DrawProxy::BeginOp()
{
	ptr->BeginOp();
}

void DrawProxy::EndOp()
{
	ptr->EndOp();
}

void DrawProxy::OffsetOp(Point p)
{
	ptr->OffsetOp(p);
}

bool DrawProxy::ClipOp(const Rect& r)
{
	return ptr->ClipOp(r);
}

bool DrawProxy::ClipoffOp(const Rect& r)
{
	return ptr->ClipoffOp(r);
}

bool DrawProxy::ExcludeClipOp(const Rect& r)
{
	return ptr->ExcludeClipOp(r);
}

bool DrawProxy::IntersectClipOp(const Rect& r)
{
	return ptr->IntersectClipOp(r);
}

bool DrawProxy::IsPaintingOp(const Rect& r) const
{
	return ptr->IsPaintingOp(r);
}

Rect DrawProxy::GetPaintRect() const
{
	return ptr->GetPaintRect();
}

void DrawProxy::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	ptr->DrawRectOp(x, y, cx, cy, color);
}

void DrawProxy::SysDrawImageOp(i32 x, i32 y, const Image& img, Color color)
{
	ptr->SysDrawImageOp(x, y, img, color);
}

void DrawProxy::SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color)
{
	ptr->SysDrawImageOp(x, y, img, src, color);
}

void DrawProxy::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color)
{
	ptr->DrawImageOp(x, y, cx, cy, img, src, color);
}

void DrawProxy::DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id)
{
	ptr->DrawDataOp(x, y, cx, cy, data, id);
}

void DrawProxy::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	ptr->DrawLineOp(x1, y1, x2, y2, width, color);
}

void DrawProxy::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count, i32k *counts,
                                   i32 count_count, i32 width, Color color, Color doxor)
{
	ptr->DrawPolyPolylineOp(vertices, vertex_count, counts, count_count, width, color, doxor);
}

void DrawProxy::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count, i32k *subpolygon_counts,
                                      i32 scc, i32k *disjunct_polygon_counts, i32 dpcc, Color color,
                                      i32 width, Color outline, zu64 pattern, Color doxor)
{
	ptr->DrawPolyPolyPolygonOp(vertices, vertex_count, subpolygon_counts, scc, disjunct_polygon_counts,
	                           dpcc, color, width, outline, pattern, doxor);
}

void DrawProxy::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	ptr->DrawArcOp(rc, start, end, width, color);
}

void DrawProxy::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor)
{
	ptr->DrawEllipseOp(r, color, pen, pencolor);
}

void DrawProxy::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink, i32 n, i32k *dx)
{
	ptr->DrawTextOp(x, y, angle, text, font, ink, n, dx);
}

void DrawProxy::DrawDrawingOp(const Rect& target, const Drawing& w)
{
	ptr->DrawDrawingOp(target, w);
}

void DrawProxy::DrawPaintingOp(const Rect& target, const Painting& w)
{
	ptr->DrawPaintingOp(target, w);
}

Size DrawProxy::GetNativeDpi() const
{
	return ptr->GetNativeDpi();
}

void DrawProxy::BeginNative()
{
	ptr->BeginNative();
}

void DrawProxy::EndNative()
{
	ptr->EndNative();
}

i32 DrawProxy::GetCloffLevel() const
{
	return ptr->GetCloffLevel();
}

void DrawProxy::Escape(const Txt& data)
{
	ptr->Escape(data);
}

}
