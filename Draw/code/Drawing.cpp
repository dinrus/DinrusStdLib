#include <drx/Draw/Draw.h>

namespace drx {

enum {
	DRAWING_BEGIN               = 1,
	DRAWING_OFFSET              = 2,
	DRAWING_CLIP                = 3,
	DRAWING_CLIPOFF             = 4,
	DRAWING_EXCLUDECLIP         = 5,
	DRAWING_INTERSECTCLIP       = 6,
	DRAWING_END                 = 7,
	DRAWING_DRAWRECT            = 8,
	DRAWING_DRAWIMAGE           = 9,
	DRAWING_DRAWMONOIMAGE       = 10,
	DRAWING_DRAWDRAWING         = 11,
	DRAWING_DRAWLINE            = 12,
	DRAWING_DRAWELLIPSE         = 13,
	DRAWING_DRAWTEXT            = 14,
	DRAWING_DRAWARC             = 15,
	DRAWING_DRAWPOLYPOLYLINE    = 16,
	DRAWING_DRAWPOLYPOLYPOLYGON = 17,
	DRAWING_DRAWDATA            = 18,
	DRAWING_DRAWPAINTING        = 19,
	DRAWING_ESCAPE              = 20,
};


#define LLOG(x)  // DLOG(x)

#ifdef COMPILER_MSC
#pragma warning(disable: 4700)
#endif

static void StreamUnpackPoints(Stream& stream, Point *out, i32 count)
{
	if(count == 0)
		return;
#ifdef CPU_LITTLE_ENDIAN
	if(sizeof(Point) == 8) {
		stream.Get(out, 8 * count);
		return;
	}
#endif
	Point *end = out + count;
	u8 *top = reinterpret_cast<u8 *>(end) - count * 8;
	stream.Get(top, count * 8);
	for(; out < end; out++, top += 8) {
		out -> x = (short)Peek32le(top + 0);
		out -> y = (short)Peek32le(top + 4);
	}
}

static void StreamPackPoints(Stream& stream, const Point *in, i32 count)
{
#ifdef CPU_LITTLE_ENDIAN
	if(sizeof(Point) == 8) {
		stream.Put(in, 8 * count);
		return;
	}
#endif
	enum { PART = 1024 };
	u8 part[PART * 8];
	while(count > 0)
	{
		i32 part_count = min<i32>(count, PART);
		for(u8 *pp = part, *pe = pp + 8 * part_count; pp < pe; pp += 8, in++) {
			Poke32le(pp + 0, in -> x);
			Poke32le(pp + 4, in -> y);
		}
		stream.Put(part, part_count * 4);
		count -= part_count;
	}
}

static void StreamUnpackInts(Stream& stream, i32 *out, i32 count)
{
#ifdef CPU_LITTLE_ENDIAN
	if(sizeof(i32) == 4) {
		stream.Get(out, count * 4);
		return;
	}
#endif
	while(count--)
		*out++ = stream.Get32le();
}

static void StreamPackInts(Stream& stream, i32k *in, i32 count)
{
#ifdef CPU_LITTLE_ENDIAN
	if(sizeof(i32) == 4) {
		stream.Put(in, count * 4);
		return;
	}
#endif
	while(count--)
		stream.Put32le(*in++);
}

// ------------------------------

Stream& DrawingDraw::DrawingOp(i32 code)
{
	drawing / code;
	return drawing;
}

dword DrawingDraw::GetInfo() const
{
	return DRAWING|(dots ? DOTS : 0);
}

Size DrawingDraw::GetPageSize() const
{
	return size;
}

Rect DrawingDraw::GetPaintRect() const
{
	return Rect(size);
}

void DrawingDraw::BeginOp()
{
	DrawingOp(DRAWING_BEGIN);
}

void DrawingDraw::OffsetOp(Point p)
{
	DrawingOp(DRAWING_OFFSET) % p;
}

bool DrawingDraw::ClipOp(const Rect& r)
{
	DrawingOp(DRAWING_CLIP) % const_cast<Rect&>(r);
	return true;
}

bool DrawingDraw::ClipoffOp(const Rect& r)
{
	DrawingOp(DRAWING_CLIPOFF) % const_cast<Rect&>(r);
	return true;
}

void DrawingDraw::EndOp()
{
	DrawingOp(DRAWING_END);
}

bool DrawingDraw::ExcludeClipOp(const Rect& r)
{
	DrawingOp(DRAWING_EXCLUDECLIP) % const_cast<Rect&>(r);
	return true;
}

bool DrawingDraw::IntersectClipOp(const Rect& r)
{
	DrawingOp(DRAWING_INTERSECTCLIP) % const_cast<Rect&>(r);
	return true;
}

bool DrawingDraw::IsPaintingOp(const Rect& r) const
{
	return true;
}

void DrawingDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	if(!IsNull(color))
		DrawingOp(DRAWING_DRAWRECT) % x % y % cx % cy % color;
}

void DrawingDraw::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color)
{
	Rect s = src;
	DrawingOp(DRAWING_DRAWIMAGE) % x % y % cx % cy % s % color;
	val.Add(img);
}

void DrawingDraw::DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id)
{
	Txt h = id;
	DrawingOp(DRAWING_DRAWDATA) % x % y % cx % cy % h;
	val.Add(data);
}

void DrawingDraw::Escape(const Txt& data)
{
	DrawingOp(DRAWING_ESCAPE);
	val.Add(data);
}

void DrawingDraw::DrawDrawingOp(const Rect& target, const Drawing& w)
{
	DrawingOp(DRAWING_DRAWDRAWING) % const_cast<Rect&>(target);
	val.Add(w);
}

void DrawingDraw::DrawPaintingOp(const Rect& target, const Painting& w)
{
	DrawingOp(DRAWING_DRAWPAINTING) % const_cast<Rect&>(target);
	val.Add(w);
}

void DrawingDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	DrawingOp(DRAWING_DRAWLINE) % x1 % y1 % x2 % y2 % width % color;
}

void DrawingDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                 i32k *counts, i32 count_count,
	                                 i32 width, Color color, Color doxor)
{
	ASSERT(count_count > 0 && vertex_count > 0);
	if(vertex_count < 2 || IsNull(color))
		return;
	DrawingOp(DRAWING_DRAWPOLYPOLYLINE);
	i32 version = 2;
	drawing / version;
	drawing % width % color % doxor;
	drawing % vertex_count % count_count;
	StreamPackPoints(drawing, vertices, vertex_count);
	StreamPackInts(drawing, counts, count_count);
}

void DrawingDraw::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                   i32k *subpolygon_counts, i32 subpolygon_count_count,
	                   i32k *disjunct_polygon_counts, i32 disjunct_polygon_count_count,
	                   Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	if(vertex_count == 0)
		return;
	DrawingOp(DRAWING_DRAWPOLYPOLYPOLYGON);
	i32 version = 2;
	drawing / version;
	drawing % color % width % outline % pattern % doxor;
	drawing % vertex_count % subpolygon_count_count % disjunct_polygon_count_count;
	StreamPackPoints(drawing, vertices, vertex_count);
	StreamPackInts(drawing, subpolygon_counts, subpolygon_count_count);
	StreamPackInts(drawing, disjunct_polygon_counts, disjunct_polygon_count_count);
}

void DrawingDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor)
{
	DrawingOp(DRAWING_DRAWELLIPSE) % const_cast<Rect&>(r) % color / pen % pencolor;
}

void DrawingDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	DrawingOp(DRAWING_DRAWARC) % const_cast<Rect&>(rc) % start % end % color % width;
}

static_assert(sizeof(wchar) == 4, "sizeof wchar");

void DrawingDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink,
                             i32 n, i32k *dx) {
	if(IsNull(ink)) return;
	if(n < 0)
		n = strlen__((wtukk )text);
	if(n == 0)
		return;
	Stream& s = DrawingOp(DRAWING_DRAWTEXT);
	u8 cs = CHARSET_UTF32;
	s % x % y % angle % font % ink / n % cs;
#ifdef CPU_LE
	s.Put(text, n * sizeof(wchar));
#else
	#error big endiand not supported
#endif
	bool dxb = dx;
	s % dxb;
	if(dx) {
		i32 *w = const_cast<i32 *>(dx);
		while(n--)
			s / *w++;
	}
}

Drawing DrawingDraw::GetResult()
{
	Drawing out;
	out.size = size;
	LLOG("GetResult size: " << size);
	out.data = drawing.GetResult();
	out.val = val;
	return out;
}

// ------------------------------

struct Draw::DrawingPos : TxtStream {
	Size    source;
	Size    target;
	Point   srcoff;
	Point   trgoff;

	Vec<Point> stk;

	bool  Identity() const                               { return source == target; }

	i32   GetX(i32 x) const;
	i32   GetY(i32 y) const;
	i32   GetCx(i32 cx) const;
	i32   GetCy(i32 cy) const;
	i32   GetW(i32 w) const;
	Point Get(i32 x, i32 y) const;
	Point Get(Point p) const;
	Rect  Get(const Rect& r) const;
	Rect  Get(i32 x, i32 y, i32 cx, i32 cy) const;

	Point operator()(i32 x, i32 y) const                 { return Get(x, y); }
	Point operator()(Point p) const                      { return Get(p); }
	Rect  operator()(const Rect& r) const                { return Get(r); }
	Rect  operator()(i32 x, i32 y, i32 cx, i32 cy) const { return Get(x, y, cx, cy); }

	void TransformX(i32& x) const                        { x = GetX(x); }
	void TransformY(i32& y) const                        { y = GetY(y); }
	void TransformW(i32& w) const                        { w = GetW(w); }
	void Transform(i32& x, i32& y) const                 { TransformX(x); TransformY(y); }
	void Transform(Point& p) const                       { p = Get(p); }
	void Transform(Rect& r) const                        { r = Get(r); }

	Rect  GetRect();

	void  Push();
	void  Pop();

	DrawingPos(const Txt& src) : TxtStream(src) {}
};

void Draw::DrawingPos::Push()
{
	stk.Add(srcoff);
	stk.Add(trgoff);
}

void Draw::DrawingPos::Pop()
{
	trgoff = stk.Pop();
	srcoff = stk.Pop();
}

Rect Draw::DrawingPos::GetRect()
{
	Rect r(0, 0, 0, 0);
	*this % r;
	return Get(r);
}

i32   Draw::DrawingPos::GetX(i32 x) const {
	return iscale(x + srcoff.x, target.cx, source.cx) - trgoff.x;
}

i32   Draw::DrawingPos::GetY(i32 y) const {
	return iscale(y + srcoff.y, target.cy, source.cy) - trgoff.y;
}

i32   Draw::DrawingPos::GetCx(i32 cx) const {
	return iscale(cx, target.cx, source.cx);
}

i32   Draw::DrawingPos::GetCy(i32 cy) const {
	return iscale(cy, target.cy, source.cy);
}

i32   Draw::DrawingPos::GetW(i32 w) const {
	return iscale(w, target.cx + target.cy, source.cx + source.cy);
}

Point Draw::DrawingPos::Get(i32 x, i32 y) const {
	return Point(GetX(x), GetY(y));
}

Point Draw::DrawingPos::Get(Point p) const {
	return Get(p.x, p.y);
}

Rect  Draw::DrawingPos::Get(const Rect& r) const {
	return Rect(GetX(r.left), GetY(r.top), GetX(r.right), GetY(r.bottom));
}

Rect  Draw::DrawingPos::Get(i32 x, i32 y, i32 cx, i32 cy) const {
	return Get(RectC(x, y, cx, cy));
}

void Draw::DrawDrawingOp(const Rect& target, const Drawing& w) {
#ifdef _DEBUG
	i32 cl = GetCloffLevel();
#endif
	DrawingPos ps(w.data);
	ps.srcoff = ps.trgoff = Point(0, 0);
	ps.target = target.Size();
	ps.source = w.size;
	LLOG("DrawDrawingOp size: " << w.size);
	if(ps.target.cx == 0 || ps.target.cy == 0 || ps.source.cx == 0 || ps.source.cy == 0)
		return;
	Clipoff(target);
	Rect r, r1;
	i32   x, y, cx, cy, width, vertex_count, count_count;
	Color color, pencolor, doxor;
	Image img;
	Drawing dw;
	Painting sw;
	Point p, p1;
	i32 vi = 0;
	while(!ps.IsEof()) {
		i32 code;
		ps / code;
		switch(code) {
		case DRAWING_BEGIN:
			Begin();
			ps.Push();
			break;
		case DRAWING_OFFSET:
			ps % p;
			p1 = ps(p);
			Offset(p1);
			ps.Push();
			ps.srcoff += p;
			ps.trgoff += p1;
			break;
		case DRAWING_CLIP:
			Clip(ps.GetRect());
			ps.Push();
			break;
		case DRAWING_CLIPOFF:
			ps % r;
			r1 = ps(r);
			Clipoff(r1);
			ps.Push();
			ps.srcoff += r.TopLeft();
			ps.trgoff += r1.TopLeft();
			break;
		case DRAWING_EXCLUDECLIP:
			ExcludeClip(ps.GetRect());
			break;
		case DRAWING_INTERSECTCLIP:
			IntersectClip(ps.GetRect());
			break;
		case DRAWING_END:
			End();
			ps.Pop();
			break;
		case DRAWING_DRAWRECT:
			ps % x % y % cx % cy % color;
			DrawRect(ps(x, y, cx, cy), color);
			break;
		case DRAWING_DRAWIMAGE:
			ps % x % y % cx % cy;
			if(w.val.GetCount())
				img = w.val[vi++];
			else
				ps % img;
			ps % r % color;
			DrawImageOp(ps.GetX(x), ps.GetY(y), ps.GetCx(cx), ps.GetCy(cy), img, r, color);
			break;
		case DRAWING_DRAWDATA:
			{
				Txt data, id;
				ps % x % y % cx % cy % id;
				if(w.val.GetCount())
					data = w.val[vi++];
				else
					ps % data;
				DrawData(ps(x, y, cx, cy), data, id);
			}
			break;
		case DRAWING_ESCAPE:
			Escape(w.val[vi++]);
			break;
		case DRAWING_DRAWDRAWING:
			if(w.val.GetCount())
				dw = w.val[vi++];
			else
				ps % dw;
			DrawDrawing(ps.GetRect(), dw);
			break;
		case DRAWING_DRAWPAINTING:
			if(w.val.GetCount())
				sw = w.val[vi++];
			else
				ps % sw;
			DrawPainting(ps.GetRect(), sw);
			break;
		case DRAWING_DRAWLINE:
			ps % x % y % cx % cy % width % color;
			DrawLine(ps(x, y), ps(cx, cy), width > 0 ? ps.GetW(width) : width, color);
			break;
		case DRAWING_DRAWELLIPSE:
			r = ps.GetRect();
			ps % color / width % pencolor;
			DrawEllipse(r, color, width > 0 ? ps.GetW(width) : width, pencolor);
			break;
#ifndef TARGET_WINCE
		case DRAWING_DRAWARC:
			r = ps.GetRect();
			ps % p % p1 % color % width;
			DrawArc(r, ps(p), ps(p1), width > 0 ? ps.GetW(width) : width, color);
			break;
		case DRAWING_DRAWPOLYPOLYLINE:
			{
				i32 version;
				ps / version;
				ps % width % color % doxor;
				ps % vertex_count % count_count;
				Buffer<Point> vertices(vertex_count);
				Buffer<i32> counts(count_count);
				StreamUnpackPoints(ps, vertices, vertex_count);
				StreamUnpackInts(ps, counts, count_count);
				if(!ps.Identity()) {
					for(Point *p = vertices, *e = p + vertex_count; p < e; p++)
						ps.Transform(*p);
					if(width > 0)
						ps.TransformW(width);
				}
				DrawPolyPolyline(vertices, vertex_count, counts, count_count, width, color, doxor);
			}
			break;
		case DRAWING_DRAWPOLYPOLYPOLYGON:
			{
				Color outline;
				zu64 pattern = 0;
				i32 subpolygon_count_count, disjunct_polygon_count_count;
				i32 version = 2;
				ps / version;
				ps % color % width % outline % pattern % doxor;
				ps % vertex_count % subpolygon_count_count % disjunct_polygon_count_count;
				Buffer<Point> vertices(vertex_count);
				Buffer<i32> subpolygon_counts(subpolygon_count_count);
				Buffer<i32> disjunct_polygon_counts(disjunct_polygon_count_count);
				StreamUnpackPoints(ps, vertices, vertex_count);
				StreamUnpackInts(ps, subpolygon_counts, subpolygon_count_count);
				StreamUnpackInts(ps, disjunct_polygon_counts, disjunct_polygon_count_count);
				if(!ps.Identity()) {
					for(Point *p = vertices, *e = p + vertex_count; p < e; p++)
						ps.Transform(*p);
					ps.TransformW(width);
				}
				DrawPolyPolyPolygon(vertices, vertex_count,
				                    subpolygon_counts, subpolygon_count_count,
				                    disjunct_polygon_counts, disjunct_polygon_count_count,
				                    color, width, outline, pattern, doxor);
			}
			break;
#endif
		case DRAWING_DRAWTEXT:
			{
				i32 n, angle;
				Font font;
				Color ink;
				u8 cs = 0;
				ps % x % y % angle % font % ink / n % cs;
				if(font.GetHeight() == 0) {
					FontInfo fi = font.Info();
					font.Height(fi.GetHeight() - fi.GetInternal());
				}
				WTxt text;
				if(cs == CHARSET_UTF32) {
					Buffer<wchar> txt(n);
					ps.Stream::Get(txt, n * sizeof(wchar));
					text = WTxt(txt, n);
				}
				else
				if(cs == CHARSET_UTF8) { // backward compatibility "CHARSET_UNICODE"
					Buffer<char16> txt(n);
					ps.Stream::GetW(txt, n);
					text = ToUtf32(txt, n);
				}
				else {
					Buffer<char> txt(n);
					ps.Stream::Get(txt, n);
					text = ToUnicode(txt, n, cs);
				}
				LLOG("wsDrawText \"" << WTxt(text, n)
				     << "\" at: (" << x << ", " << y << ", " << angle << ")");
				bool dxb = false;
				ps % dxb;
				Buffer<i32> dx(n);
				i32 *wd = dx;
				i32 nn = n;
				angle %= 3600;
				if(ps.Identity()) {
					if(dxb) {
						while(nn--)
							ps / *wd++;
						DrawText(x, y, angle, text, font, ink, dx);
					}
					else
						DrawText(x, y, angle, text, font, ink);
				}
				else {
					wtukk wp = ~text;
					double q = Length((Sizef)ps.target) / Length((Sizef)ps.source);
					double px = 0;
					while(nn--) {
						i32 cx;
						if(dxb)
							ps / cx;
						else
							cx = font[*wp++];
						double npx = q * cx + px;
						*wd++ = (i32)npx - (i32)px;
						px = npx;
					}
					i32 ht = (i32)(font.GetHeight() * min(double(ps.target.cx) / ps.source.cx, double(ps.target.cy) / ps.source.cy));
					font.Width((i32)q * font.GetWidth()).Height(ht ? ht : 1);
					DrawText(ps.GetX(x), ps.GetY(y), angle, text, font, ink, dx);
				}
			}
		}
	}
//	LOGEND();
	End();
#ifdef _DEBUG
	ASSERT(GetCloffLevel() == cl);
#endif
}

void Draw::DrawDrawing(i32 x, i32 y, i32 cx, i32 cy, const Drawing& w) {
	DrawDrawing(RectC(x, y, cx, cy), w);
}

void Draw::DrawDrawing(i32 x, i32 y, const Drawing& iw)
{
	Size sz = iw.GetSize();
	DrawDrawing(RectC(x, y, sz.cx, sz.cy), iw);
}

void  DrawingDraw::Create(i32 cx, i32 cy, bool dots_) {
	Create(Size(cx, cy), dots_);
}

void  DrawingDraw::Create(Size sz, bool dots_) {
	drawing.Create();
	size = sz;
	dots = dots_;
	val.Clear();
	LLOG("DrawingDraw::Create, sz = " << sz << ", dots = " << dots << " -> clip = " << GetClip());
}

DrawingDraw::DrawingDraw()
{
}

DrawingDraw::DrawingDraw(Size sz, bool dots_) {
	Create(sz, dots_);
}

DrawingDraw::DrawingDraw(i32 cx, i32 cy, bool dots_) {
	Create(cx, cy, dots_);
}

Size Drawing::RatioSize(i32 cx, i32 cy) const {
	return GetRatioSize(GetSize(), cx, cy);
}

void Drawing::Append(Drawing& dw)
{
	if(IsNull(size))
		size = dw.size;
	data << dw.data;
	for(i32 i = 0; i < dw.val.GetCount(); i++)
		val.Add(dw.val[i]);
}

void Drawing::Serialize(Stream& s)
{
	if(val.GetCount())
		size.cy |= 0x80000000;
	s % size;
	s % data;
	if(size.cy & 0x80000000) {
		size.cy &= ~0x80000000;
		s % val;
	}
}

}
