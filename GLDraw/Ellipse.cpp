#include "GLDraw.h"

namespace drx {

void GLDrawEllipse(const GLContext2D& dd, Pointf center, Sizef radius, Color fill_color, double width,
                   Color line_color, const Vec<double>& dash, double distance)
{
	GL_TIMING("Ellipse");
	static GLVertexData fill, line;
	i32k N = 200;
	
	ONCELOCK {
		Vec<Vec<Pointf>> p;
		p.Add();
		for(i32 i = 0; i < N; i++)
			p.Top().Add(Polar(i * M_2PI / N));
		GLPolygons(fill, p);
		Pointf f = p.Top()[0];
		p.Top().Add(f);
		GLPolylines(line, p);
	}
	
	Sizef r = radius - Sizef(width, width);
	if(r.cx > 0 && r.cy > 0 && !IsNull(fill_color))
		GLDrawPolygons(dd, center, fill, r, fill_color);
	r = radius - Sizef(width / 2, width / 2);
	if(width && !IsNull(line_color)) {
		if(dash.GetCount()) {
			GL_TIMING("Ellipse Dash");
			Vec<Pointf> line;
			for(i32 i = 0; i < N; i++)
				line.Add(r * Polar(i * M_2PI / N) + center);
			Vec<Vec<Pointf>> ll;
			DashPolyline(ll, line, dash, 0);
			GLVertexData data;
			GLPolylines(data, ll);
			GLDrawPolylines(dd, Pointf(0, 0), data, Sizef(1, 1), width, line_color);
		}
		else
			GLDrawPolylines(dd, center, line, r, width, line_color);
	}
}

void GLDrawEllipse(const GLContext2D& dd, Pointf center, Sizef radius, Color fill_color, double width,
                   Color line_color)
{
	static Vec<double> empty;
	GLDrawEllipse(dd, center, radius, fill_color, width, line_color, empty, 0);
}

};
