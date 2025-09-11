#include <drx/Draw/Draw.h>

namespace drx {

dword SDraw::GetInfo() const
{
	return DRAWTEXTLINES;
}

void SDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	// TODO
}

void SDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor)
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

void SDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	if(IsNull(width) || IsNull(color))
		return;
	Width(width);
	docolor = color;
	Move(Point(x1, y1));
	Line(Point(x2, y2));
}

void SDraw::DrawPolyPolyPolygonOp(const Point *vertices0, i32 /*vertex_count*/,
                                  i32k *subpolygon_counts0,
                                  i32 /*scc*/, i32k *disjunct_polygon_counts0, i32 dpcc0,
                                  Color color, i32 width, Color outline,
                                  zu64 /*pattern*/, Color /*doxor*/) // Last two parameters ignored
{
	for(i32 pass = 0; pass < 1 + (width > 0); pass++) {
		const Point *vertices = vertices0;
		i32k *subpolygon_counts = subpolygon_counts0;
		i32k *disjunct_polygon_counts = disjunct_polygon_counts0;
		i32 dpcc = dpcc0;
		while(--dpcc >= 0) {
			const Point *sp = vertices;
			vertices += *disjunct_polygon_counts++;
			if(pass == 0)
				Polygon();
			else {
				docolor = outline;
				Width(width);
			}
			while(sp < vertices) {
				const Point *pp = sp;
				sp += *subpolygon_counts++;
				Move(*pp);
				while(++pp < sp)
					Line(*pp);
				Close();
			}
			if(pass == 0 && !IsNull(color)) {
				docolor = color;
				Fill();
			}
		}
	}
}

void SDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
                               i32k *counts, i32 count_count, i32 width, Color color,
                               Color /*doxor*/) // Last parameter ignored
{
	if(width == 0)
		width = 1;
	if(IsNull(color))
		return;
	Width(width);
	docolor = color;
	while(--count_count >= 0) {
		const Point *lp = vertices;
		vertices += *counts++;
		Move(*lp);
		while(++lp < vertices)
			Line(*lp);
	}
}

}
