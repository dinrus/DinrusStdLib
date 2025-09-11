#include "CtrlCore.h"

#ifdef GUI_WIN

namespace drx {

#define LLOG(x)      // LOG(x)
#define LTIMING(x)   // RTIMING(x)

void SystemDraw::BeginOp()
{
	LTIMING("Begin");
	GuiLock __;
	Cloff& w = cloff.Add();
	w.org = actual_offset;
	w.drawingclip = drawingclip;
	w.hrgn = CreateRectRgn(0, 0, 0, 0);
	ASSERT(w.hrgn);
	i32	q = ::GetClipRgn(handle, w.hrgn);
	ASSERT(q >= 0);
	if(q == 0) {
		DeleteObject(w.hrgn);
		w.hrgn = NULL;
	}
}

void SystemDraw::OffsetOp(Point p)
{
	GuiLock __;
	Begin();
	actual_offset += p;
	drawingclip -= p;
	LTIMING("Offset");
	SetOrg();
}

bool SystemDraw::ClipOp(const Rect& r)
{
	GuiLock __;
	Begin();
	LTIMING("Clip");
	return IntersectClip(r);
}

bool SystemDraw::ClipoffOp(const Rect& r)
{
	GuiLock __;
	Begin();
	LTIMING("Clipoff");
	LLOG("ClipoffOp " << r << ", GetClip() = " << GetClip() << ", actual_offset = " << actual_offset);
	actual_offset += r.TopLeft();
	bool q = IntersectClip(r);
	drawingclip -= r.TopLeft();
	SetOrg();
	LLOG("//ClipoffOp, GetClip() = " << GetClip() << ", actual_offset = " << actual_offset);
	return q;
}

void SystemDraw::EndOp()
{
	GuiLock __;
	LTIMING("End");
	ASSERT(cloff.GetCount());
	Cloff& w = cloff.Top();
	actual_offset = w.org;
	drawingclip = w.drawingclip;
	::SelectClipRgn(handle, w.hrgn);
	SetOrg();
	if(w.hrgn)
		::DeleteObject(w.hrgn);
	cloff.Drop();
}

bool SystemDraw::ExcludeClipOp(const Rect& r)
{
	GuiLock __;
#ifdef TARGET_WINCE
	i32 q = ExcludeClipRect(handle, r.left, r.top, r.right, r.bottom);
#else
	LTIMING("ExcludeClip");
	if(r.Contains(drawingclip))
		drawingclip = Rect(0, 0, 0, 0);
	Rect rr = LPtoDP(r);
	HRGN hrgn = ::CreateRectRgnIndirect(rr);
	i32 q = ::ExtSelectClipRgn(handle, hrgn, RGN_DIFF);
	ASSERT(q != ERROR);
	::DeleteObject(hrgn);
#endif
	return q == SIMPLEREGION || q == COMPLEXREGION;
}

bool SystemDraw::IntersectClipOp(const Rect& r)
{
	GuiLock __;
#ifdef TARGET_WINCE
	i32 q = IntersectClipRect(handle, r.left, r.top, r.right, r.bottom);
#else
	LTIMING("Intersect");
	drawingclip &= r;
	Rect rr = LPtoDP(r);
	HRGN hrgn = ::CreateRectRgnIndirect(rr);
	i32 q = ::ExtSelectClipRgn(handle, hrgn, RGN_AND);
	ASSERT(q != ERROR);
	::DeleteObject(hrgn);
#endif
	return q == SIMPLEREGION || q == COMPLEXREGION;
}

bool SystemDraw::IsPaintingOp(const Rect& r) const
{
	GuiLock __;
	LTIMING("IsPainting");
	return ::RectVisible(handle, r);
}

Rect SystemDraw::GetPaintRect() const
{
	GuiLock __;
	LTIMING("GetPaintRect");
	return drawingclip;
}

void SystemDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	GuiLock __;
	LTIMING("DrawRect");
	LLOG("DrawRect " << RectC(x, y, cx, cy) << ": " << color);
	if(IsNull(color)) return;
	if(cx <= 0 || cy <= 0) return;
	if(color == InvertColor)
		::PatBlt(handle, x, y, cx, cy, DSTINVERT);
	else {
		SetColor(color);
		::PatBlt(handle, x, y, cx, cy, PATCOPY);
	}
}

void SystemDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	GuiLock __;
	if(IsNull(width) || IsNull(color)) return;
	SetDrawPen(width, color);
	::MoveToEx(handle, x1, y1, NULL);
	::LineTo(handle, x2, y2);
}

#ifndef TARGET_WINCE

void SystemDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
                            i32k *counts, i32 count_count,
	                        i32 width, Color color, Color doxor)
{
	GuiLock __;
	ASSERT(count_count > 0 && vertex_count > 0);
	if(vertex_count < 2 || IsNull(color) || IsNull(width))
		return;
	bool is_xor = !IsNull(doxor);
	if(is_xor)
		color = Color(color.GetR() ^ doxor.GetR(), color.GetG() ^ doxor.GetG(), color.GetB() ^ doxor.GetB());
	if(is_xor)
		SetROP2(GetHandle(), R2_XORPEN);
	SetDrawPen(width, color);
	if(count_count == 1)
		::Polyline(GetHandle(), (const POINT *)vertices, vertex_count);
	else
		::PolyPolyline(GetHandle(), (const POINT *)vertices,
		               (const dword *)counts, count_count);
	if(is_xor)
		SetROP2(GetHandle(), R2_COPYPEN);
}

static void DrawPolyPolyPolygonRaw(
	SystemDraw& draw, const Point *vertices, i32 vertex_count,
	i32k *subpolygon_counts, i32 subpolygon_count_count,
	i32k *disjunct_polygon_counts, i32 disjunct_polygon_count_count)
{
	GuiLock __;
	for(i32 i = 0; i < disjunct_polygon_count_count; i++, disjunct_polygon_counts++)
	{
		i32 poly = *disjunct_polygon_counts;
		i32 sub = 1;
		if(*subpolygon_counts < poly) {
			if(disjunct_polygon_count_count > 1)
			{
				i32k *se = subpolygon_counts;
				i32 total = 0;
				while(total < poly)
					total += *se++;
				sub = (i32)(se - subpolygon_counts);
			}
			else
				sub = subpolygon_count_count;
		}
		ASSERT(sizeof(POINT) == sizeof(Point)); // modify algorithm when not
		if(sub == 1)
			Polygon(draw, (const POINT *)vertices, poly);
		else
			PolyPolygon(draw, (const POINT *)vertices, subpolygon_counts, sub);
		vertices += poly;
		subpolygon_counts += sub;
	}
}

void SystemDraw::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	i32k *subpolygon_counts, i32 subpolygon_count_count,
	i32k *disjunct_polygon_counts, i32 disjunct_polygon_count_count,
	Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	GuiLock __;
	if(vertex_count == 0)
		return;
	bool is_xor = !IsNull(doxor);
	HDC hdc = GetHandle();
	if(pattern) {
		i32 old_rop = GetROP2(hdc);
		HGDIOBJ old_brush = GetCurrentObject(hdc, OBJ_BRUSH);
		word wpat[8] = {
			(u8)(pattern >> 56), (u8)(pattern >> 48), (u8)(pattern >> 40), (u8)(pattern >> 32),
			(u8)(pattern >> 24), (u8)(pattern >> 16), (u8)(pattern >> 8), (u8)(pattern >> 0),
		};
		HBITMAP bitmap = CreateBitmap(8, 8, 1, 1, wpat);
		HBRUSH brush = ::CreatePatternBrush(bitmap);
		COLORREF old_bk = GetBkColor(hdc);
		COLORREF old_fg = GetTextColor(hdc);
		if(!is_xor) {
			SetROP2(hdc, R2_MASKPEN);
			SelectObject(hdc, brush);
			SetTextColor(hdc, Black());
			SetBkColor(hdc, White());
			SetDrawPen(PEN_NULL, Black);
			DrawPolyPolyPolygonRaw(*this, vertices, vertex_count,
				subpolygon_counts, subpolygon_count_count,
				disjunct_polygon_counts, disjunct_polygon_count_count);
			SetROP2(hdc, R2_MERGEPEN);
			SetTextColor(hdc, color);
			SetBkColor(hdc, Black());
		}
		else {
			SetROP2(hdc, R2_XORPEN);
			SetTextColor(hdc, COLORREF(color) ^ COLORREF(doxor));
			SelectObject(hdc, brush);
		}
		DrawPolyPolyPolygonRaw(*this, vertices, vertex_count,
			subpolygon_counts, subpolygon_count_count,
			disjunct_polygon_counts, disjunct_polygon_count_count);
		SelectObject(hdc, old_brush);
		SetTextColor(hdc, old_fg);
		SetBkColor(hdc, old_bk);
		SetROP2(hdc, old_rop);
		DeleteObject(brush);
		DeleteObject(bitmap);
		if(!IsNull(outline)) {
			SetColor(Null);
			SetDrawPen(width, outline);
			ASSERT(sizeof(POINT) == sizeof(Point));
			DrawPolyPolyPolygonRaw(*this, vertices, vertex_count,
				subpolygon_counts, subpolygon_count_count,
				disjunct_polygon_counts, disjunct_polygon_count_count);
		}
	}
	else { // simple fill
		SetDrawPen(IsNull(outline) ? PEN_NULL : width, Nvl(outline, Black));
		i32 old_rop2;
		if(is_xor) {
			color = Color(color.GetR() ^ doxor.GetR(), color.GetG() ^ doxor.GetG(), color.GetB() ^ doxor.GetB());
			old_rop2 = SetROP2(hdc, R2_XORPEN);
		}
		SetColor(color);
		DrawPolyPolyPolygonRaw(*this, vertices, vertex_count,
			subpolygon_counts, subpolygon_count_count,
			disjunct_polygon_counts, disjunct_polygon_count_count);
		if(is_xor)
			SetROP2(hdc, old_rop2);
	}
}

void SystemDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	GuiLock __;
	SetDrawPen(width, color);
	::Arc(GetHandle(), rc.left, rc.top, rc.right, rc.bottom, start.x, start.y, end.x, end.y);
}

#endif

void SystemDraw::DrawEllipseOp(const Rect& r, Color color, i32 width, Color pencolor)
{
	GuiLock __;
	SetColor(color);
	SetDrawPen(width, pencolor);
	::Ellipse(GetHandle(), r.left, r.top, r.right, r.bottom);
}

}

#endif
