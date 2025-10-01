namespace drx {

typedef const i8  LineStyle[];
typedef u8k* LineStyleArg;

extern LineStyle SolidLine;
extern LineStyle DashLine;
extern LineStyle DotLine;
extern LineStyle DashDotLine;
extern LineStyle DashDotDotLine;
extern LineStyle EmptyLine;

class LineDraw
{
public:
	LineDraw();
	LineDraw(Draw& draw, LineStyleArg pattern = SolidLine, Color color = Black, i32 width = -10, double dash = 0);

	operator bool () const                { return draw && !empty; }

	void           Set(Draw& draw, LineStyleArg pattern = SolidLine, Color color = Black, i32 width = -10, double dash = 0);

	bool           SetExtent(const Rect& rc);
	void           ClearExtent();

	void           Clear();
	void           Flush(); // flush polyline buffer

	void           MoveTo(Point pt)       { ASSERT(MoveToRaw); (this->*MoveToRaw)(pt); }
	void           LineTo(Point pt)       { ASSERT(LineToRaw); (this->*LineToRaw)(pt); }
	void           Close();

	LineDraw&      operator << (Point pt) { LineTo(pt); return *this; }

	void           Line(Point p, Point q) { MoveTo(p); LineTo(q); }

	void           Rectangle(const Rect& rc);

	void           ArcTo(Point p, i32 bulge, i32 levels = 10);
	void           Arc(Point p, Point q, i32 bulge, i32 levels = 10) { MoveTo(p); ArcTo(q, bulge, levels); }
	void           Circle(Point centre, i32 radius);
	void           Ellipse(const Rect& rc);

private:
	void           RecurseArc(Point to, i32 length, i32 bulge, i32 levels);
	void           MoveToEmpty(Point pt);
	void           MoveToSolid(Point pt);
	void           MoveToDashed(Point pt);
	void           LineToEmpty(Point pt);
	void           LineToSolidSimple(Point pt);
	void           LineToSolidClip(Point pt);
	void           LineToDashedSimple(Point pt);
	void           LineToDashedClip(Point pt);
	void           AddVectorThin(Point a, Point b);
	void           AddVectorThick(Point a, Point b);
	void           AddVectorArea(Point a, Point b);

private:
	void (LineDraw::*MoveToRaw)(Point pt);
	void (LineDraw::*LineToRaw)(Point pt);
	void (LineDraw::*AddVector)(Point a, Point b);

	Draw          *draw;
	i32            max_rad;
	i32            pen_width;
	Color          pen_color;
	Rect           clip;
	Point          first;
	Point          last;
	Vec<double> segments;
	const double  *active;
	double         remains;
	double         factor;
	bool           empty;
	i32            half_width;
	Vec<Point>  vertices;
	Vec<i32>    indices;
};

class Plotter
{
public:
	Plotter() : draw(0), ltop(0), ltopoint(0), path_map(0) {}

	Plotter(Draw& draw, const Rectf& src, const Rectf& dest,
		i32 flags = ISOTROPIC | MIRROR_Y, i32 reserve = 0, double map_meters = 1)
	{ Set(draw, src, dest, flags, reserve, map_meters); }

	Plotter(Draw& draw, Sizef scale, Pointf delta = Pointf(0, 0), i32 reserve = 0, double map_meters = 1)
	{ Set(draw, scale, delta, reserve, map_meters); }

	Plotter(Draw& draw, const Matrixf& matrix = Matrixf_1(), i32 reserve = 0, double map_meters = 1)
	{ Set(draw, matrix, reserve, map_meters); }

	Plotter(Draw& draw, const Matrixf& matrix, const Rect& clip, double map_meters = 1)
	{ Set(draw, matrix, clip, map_meters); }

	Plotter(const Plotter& info, const Rect& clip)
	{ Set(info, clip); }

	Plotter(const Plotter& info, i32 reserve)
	{ Set(info, reserve); }

	Plotter(const Plotter& info)
	{ Set(info); }

	enum
	{
		MIRROR_X  = 0x01,
		MIRROR_Y  = 0x02,
		ISOTROPIC = 0x04,
	};

	void                Set(Draw& draw, const Rectf& src, const Rectf& dest, i32 flags = ISOTROPIC | MIRROR_Y, i32 reserve = 0, double map_meters = 1);
	void                Set(Draw& draw, Sizef scale, Pointf delta = Pointf(0, 0), i32 reserve = 0, double map_meters = 1);
	void                Set(Draw& draw, const Matrixf& matrix = Matrixf_1(), i32 reserve = 0, double map_meters = 1);
	void                Set(Draw& draw, const Matrixf& matrix, const Rect& clip, double map_meters = 1);
	void                Set(const Plotter& info, const Rect& clip);
	void                Set(const Plotter& info, i32 reserve);
	void                Set(const Plotter& info)                          { *this = info; }
	void                SetDraw(Draw& draw, const Rect& clip, double map_meters);

	void                SetPixelsPerDot(double  _ppd)                     { pixels_per_dot = _ppd; }
	double              GetPixelsPerDot() const                           { return pixels_per_dot; }
	double              GetPixelsPerPoint() const                         { return pixels_per_dot * (600.0 / 72.0); }
	double              GetPixelsPerInch() const                          { return pixels_per_dot * 600.0; }
	double              GetPixelsPerMeter() const                         { return pixels_per_dot * (6e5 / 25.4); }

	void                SetMapMeters(double mm);
	double              GetPhysicalScale() const                          { return physical_scale; }

	Draw&               GetDraw()                                         { ASSERT(draw); return *draw; }
	void                SetXorMode(bool xm);

	Point               LtoPoint(Pointf pt) const                         { ASSERT(ltopoint); return (this->*ltopoint)(pt); }

	double              LtoP(double dist) const                           { return dist * measure; }
	double              PtoL(double dist) const                           { return dist / measure; }

	Pointf              LtoP(Pointf pt) const                             { ASSERT(ltop); return (this->*ltop)(pt); }
	Pointf              LtoPrel(Pointf pt) const                          { return pt % physical; }

	Pointf              PtoL(Pointf pt) const                             { return pt * logical; }
	Pointf              PtoLrel(Pointf pt) const                          { return pt % logical; }

	Rectf               LtoP(const Rectf& rc) const;
	Rectf               LtoPrel(const Rectf& rc) const;

	Rectf               PtoL(const Rectf& rc) const;
	Rectf               PtoLrel(const Rectf& rc) const;

	const Matrixf&      LtoP() const                                      { return physical; }
	const Matrixf&      PtoL() const                                      { return logical; }

	const Rectf&        GetLClip() const                                  { return logclip; }
	const Rect&         GetPClip() const                                  { return clip; }

	bool                IntersectsLClip(const Rectf& rc) const            { return logclip && rc; }
	bool                IntersectsPClip(const Rectf& rc) const            { return rc.left < clip.right && rc.top < clip.bottom && rc.right > clip.left && rc.bottom > clip.top; }
	bool                InLClip(const Rectf& rc) const                    { return logclip.Contains(rc); }
	bool                InPClip(const Rectf& rc) const                    { return rc.left >= clip.left && rc.top >= clip.top && rc.right <= clip.right && rc.bottom <= clip.bottom; }
	bool                InLClip(Pointf pt) const                          { return logclip.Contains(pt); }
	bool                InPClip(Pointf pt) const                          { return pt.x >= clip.left && pt.y >= clip.top && pt.x <= clip.right && pt.y <= clip.bottom; }
	bool                InLClipX(double x) const                          { return x >= logclip.left && x <= logclip.right; }
	bool                InLClipY(double y) const                          { return y >= logclip.top  && y <= logclip.bottom; }

	void                PathMap(const PathStyleMap *pm)                   { path_map = pm; }
	const PathStyleMap *GetPathMap() const                                { return path_map; }
	PathStyle           GetPath(const Txt& spec) const                 { ASSERT(path_map); return path_map -> Scan(spec); }

public:
	Draw               *draw;
	const PathStyleMap *path_map;
	double              pixels_per_dot;
	double              map_meters;
	Matrixf             physical;
	Matrixf             logical;
	Rectf               logclip;
	Rect                clip;
	double              logdiag;
	double              logprec;
	double              signed_measure;
	double              measure;
	double              physical_scale;
	bool                ortho;

private:
	Pointf              LtoPFull(Pointf pt) const                   { return pt * physical; }
	Pointf              LtoPOrtho(Pointf pt) const                  { return Pointf(pt.x * physical.x.x + physical.a.x, pt.y * physical.y.y + physical.a.y); }

	Point               LtoPointFull(Pointf pt) const;
	Point               LtoPointOrtho(Pointf pt) const;

	Pointf              (Plotter::*ltop)(Pointf pt) const;
	Point               (Plotter::*ltopoint)(Pointf pt) const;
};

void PlotDragRect(Plotter& plotter, const Rectf& rc);

class PlotterTool
{
public:
	typedef PlotterTool CLASSNAME;
	PlotterTool();
	virtual ~PlotterTool();

	Plotter&          GetPlotter()                      { return plotter; }
	const Plotter&    GetPlotter() const                { return plotter; }

	virtual void      Paint(); // flush & clear paint buffer
	virtual void      Clear(); // clear paint buffer

	void              MoveTo(double x, double y)        { MoveToRaw(Pointf(x, y)); }
	void              MoveTo(Pointf pt)                 { MoveToRaw(pt); }
	void              MoveRel(Pointf pt)                { MoveToRaw(pt + last_point); }
	Callback1<Pointf> GetMoveTo() const                 { return MoveToRaw; }

	void              LineTo(double x, double y)        { LineToRaw(Pointf(x, y)); }
	void              LineTo(Pointf pt)                 { LineToRaw(pt); }
	void              LineRel(Pointf pt)                { LineToRaw(pt + last_point); }
	Callback1<Pointf> GetLineTo() const                 { return LineToRaw; }

	void              DrawTo(double x, double y, bool d) { if(d) LineTo(x, y); else MoveTo(x, y); }
	void              DrawTo(Pointf pt, bool d)         { if(d) LineTo(pt); else MoveTo(pt); }
	void              DrawRel(Point pt, bool d)         { if(d) LineRel(pt); else MoveRel(pt); }

	void              Line(Pointf p, Pointf q)          { MoveTo(p); LineTo(q); }
	void              Line(double x1, double y1, double x2, double y2) { MoveTo(x1, y1); LineTo(x2, y2); }
	void              Line(const Array<Pointf>& points);

	virtual void      ArcTo(Pointf pt, double bulge, i32 levels = 10);

	void              Rectangle(double x1, double y1, double x2, double y2, bool r = false);
	void              Rectangle(Pointf p, Pointf q, bool r = false) { Rectangle(p.x, p.y, q.x, q.y, r); }
	void              Rectangle(const Rectf& rc, bool r = false)    { Rectangle(rc.left, rc.top, rc.right, rc.bottom, r); }

	void              Circle(double x, double y, double radius);
	void              Circle(Pointf centre, double radius) { Circle(centre.x, centre.y, radius); }

	void              Ellipse(const Rectf& rc);

	void              ArrowHead(Pointf P, Pointf Q, double length, double angle);
	void              Arrow(Pointf P, Pointf Q, double length, double angle);
	void              BiArrow(Pointf P, Pointf Q, double length, double angle);

protected:
	virtual bool      ArcToRaw(Pointf pt, double bulge);

protected:
	Plotter           plotter;
	Pointf            last_point;
	bool              clip_arcs;

	Callback1<Pointf> MoveToRaw;
	Callback1<Pointf> LineToRaw;
};

class PathTool : public PlotterTool
{
public:
	typedef PathTool CLASSNAME;
	PathTool();
	virtual ~PathTool();

	virtual void   Paint(); // flush & clear paint buffer
	virtual void   Clear(); // clear paint buffer

	void           Reset();
	bool           Set(const Plotter& info, const Txt& style = Null,
		Color color = Black, i32 width = -10, double dash = Null);

	bool           SetExtent(const Rectf& rc);
	void           ClearExtent();

public:
#ifdef PLOTTER_TIMING
	static i32     segments;
	static i32     drawn;
	static long    ticks;
#endif

protected:
	virtual bool   ArcToRaw(Pointf pt, double bulge);

	void           MoveToRawDummy(Pointf pt);
	void           MoveToRawSimple(Pointf pt);
	void           MoveToRawClip(Pointf pt);
	void           LineToRawDummy(Pointf pt);
	void           LineToRawSimple(Pointf pt);
	void           LineToRawClip(Pointf pt);

protected:
	bool           use_last;
	Pointf         last_phys;
	PathDraw       pathdraw;
	bool           dummy;
	bool           simple;
};

class AreaTool : public PlotterTool
{
public:
	typedef AreaTool CLASSNAME;
	AreaTool();
	virtual ~AreaTool();

	void           FlushFill();

	virtual void   Paint(); // flush & clear paint buffer
	virtual void   Clear(); // clear paint buffer

	void           Reset();
	bool           Set(const Plotter& info, Color fill_color = Black, zu64 fill_pattern = 0,
		const Txt& outline_pattern = Null, Color outline_color = Black,
		i32 outline_width = -10, double outline_dash = Null);

	bool           SetExtent(const Rectf& rc);
	void           ClearExtent();

	i32            GetPointPixels() const { return point_pixels; }
	Color          GetPointColor() const  { return point_color; }

	bool           IsFill() const { return !IsNull(fill_color); }
	bool           IsLine() const { return is_line; }
#ifdef _DEBUG
	void           ShowIndices();
#endif

	void           MoveToGhost(Pointf pt) { prev_ghost = true; MoveTo(pt); prev_ghost = false; }
	void           LineToGhost(Pointf pt) { prev_ghost = true; LineTo(pt); prev_ghost = false; }

protected:
	void           MoveToRawDummy(Pointf pt);
	void           MoveToRawSimple(Pointf pt);
	void           MoveToRawClip(Pointf pt);
	void           LineToRawDummy(Pointf pt);
	void           LineToRawSimple(Pointf pt);
	void           LineToRawClip(Pointf pt);

private:
	void           SkipTo(Point pt, bool clockwise);
	void           Flush();
	AreaTool&      Horz(i32 x);
	AreaTool&      Vert(i32 y);
	Point          ClipBind(Pointf pt) const
	{
		return Point(
			pt.x < plotter.clip.left ? plotter.clip.left : pt.x > plotter.clip.right  ? plotter.clip.right  : fround(pt.x),
			pt.y < plotter.clip.top  ? plotter.clip.top  : pt.y > plotter.clip.bottom ? plotter.clip.bottom : fround(pt.y));
	}

private:
	enum { FLUSH_BATCH = 6000 };
	Vec<Point>  vertices;
	Vec<i32>    counts;
	Vec<i32>    disjunct_counts;
	Vec<i32>    ghost_lines;
	bool           prev_ghost;
	i32            begin_index;
	i32            disjunct_begin_index;
	Pointf         last_phys;
	Pointf         last_start;
	Pointf         clip_center;
	bool           last_in;
	bool           std_pen;
	bool           simple;
	bool           is_line;
	bool           dummy;

	i32            point_pixels;
	Color          point_color;
	i32            outline_pixels;
	PathDraw       raw_outline;
	Color          fill_outline_color;
	i32            fill_outline_style;
	Color          thick_outline_color;
	zu64         fill_pattern;
	Color          fill_color;
};

class MarkTool {
public:
	class Marker {
	public:
		Marker() {}
		virtual ~Marker() {}

		virtual i32  GetSize() const = 0;
		virtual void Paint(Draw& draw, const Vec<Point>& pt) = 0;
	};

	static One<Marker> Empty();
	static One<Marker> Square(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Triangle(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Circle(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Cross(i32 size = 10, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Diamond(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Hexagon(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Star(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Square45(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Nabla(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> ThickCross(i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);
	static One<Marker> Letter(char ascii, i32 angle = 0, Font font = StdFont(), Color color = LtRed, Color outline = Black);
	static One<Marker> Picture(Image pic, i32 size = 0, Color color = LtRed, bool antialias = false, bool ignore_hotspot = false);
	// angle in degrees, 360 = full circle

	enum { CIRCLE, SQUARE, TRIANGLE, CROSS, DIAMOND, HEXAGON, STAR, SQUARE45, NABLA, THICK_CROSS, STD_MARK_COUNT }; // standard markers
	static One<Marker> StandardMarker(i32 type, i32 size = 10, Color color = LtRed, Color outline = Black, i32 outline_width = 0);

	MarkTool();
	virtual ~MarkTool();

	void             Reset();
	bool             Set(const Plotter& info, One<Marker> marker);
	bool             SetExtent(const Rectf& rc);
	void             ClearExtent();

	virtual void     Paint(); // flush & clear paint buffer
	virtual void     Clear(); // clear paint buffer
	void             Flush();

	void             Put(Pointf pt)  { ASSERT(PutRaw); (this->*PutRaw)(pt); }
	void             Put(const Array<Pointf>& pt);

private:
	void             PutDummy(Pointf pt);
	void             PutClip(Pointf pt);
	void             PutSimple(Pointf pt);

private:
	void (MarkTool::*PutRaw)(Pointf pt);
	Vec<Point>    vertices;
	One<Marker>      marker;
	Plotter          plotter;
	i32              size;
	enum { BUF_COUNT = 8192 };
};

class TextTool
{
public:
	TextTool();
	virtual ~TextTool();

	void Set(const Plotter& info, Font font = Arial(0), Color color = Black, Color outline = Null,
		double angle = 0, Alignment x_align = ALIGN_CENTER, Alignment y_align = ALIGN_CENTER,
		bool flip = false, i32 angle_prec = 24);
	// angle in radians

	void             SetFont(Font _font);
	void             SetColor(Color _color)          { color   = _color; }
	void             SetOutline(Color _outline)      { outline = _outline; }
	void             SetAngle(double _angle)         { angle   = _angle; }
	void             SetXAlign(Alignment _x)         { x_align = _x; }
	void             SetYAlign(Alignment _y)         { y_align = _y; }
	void             SetFlip(bool _flip)             { flip    = _flip; }

	void             Put(const Txt& text, Pointf pt, double angle);
	void             Put(const Txt& text, Pointf pt) { Put(text, pt, angle); }
	void             PutFlip(const Txt& text, Pointf pt, double angle);

	virtual void     Paint(); // flush & clear paint buffer
	virtual void     Clear(); // clear paint buffer

public:
	Plotter          plotter;
	Font             font;
	Color            color;
	Color            outline;
	double           angle;
	Alignment        x_align;
	Alignment        y_align;
	bool             flip;
	i32              angle_prec;

private:
	void             Flush();
	void             ClearCache();

private:
	struct Type
	{
		i32   height;
		short widths[256];

		Size  GetTextSize(const Txt& text) const;
	};

	struct Item
	{
		Item(Point point, const Txt& text, Color color, Color outline)
		: point(point), text(text), color(color), outline(outline) {}

		i32    GetSize() const { return sizeof(*this) + text.GetLength(); }

		Point  point;
		Txt text;
		Color  color;
		Color  outline;
	};

	enum { CACHE_LIMIT = 1000000, FONT_LIMIT = 100 };
	i32              cache_size;
	i32              font_index;
	ArrayMap<Font, Type> font_map;
	VecMap<i32, Array<Item> > cache;
};

}
