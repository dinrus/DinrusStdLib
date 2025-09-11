namespace drx {

class PlotterTool;

#define CATCH_PATH_MAP 0 // 1 = turn path catching on

#if CATCH_PATH_MAP
	#define PATH_MAP_NULL NULL
#else
	#define PATH_MAP_NULL &PathStyleMap::App()
#endif

const double STD_CHAMFER = 1.5;

class PathStyleMisc
{
public:
	PathStyleMisc() : width(1), begin(0), segment(1), end(0),
		miter(MITER_SHARP), chamfer(STD_CHAMFER) {}

public:
	double begin, segment, end;
	double width;
	double chamfer; // relative chamfer size
	enum { MITER_ROUND, MITER_SHARP, MITER_FLAT };
	i32    miter;
};

class PathStyle : public PathStyleMisc, MoveableAndDeepCopyOption<PathStyle>
{
public:
	PathStyle() {}
	PathStyle(pick_ PathStyle& style);
	PathStyle(const PathStyle& ps, i32) : PathStyleMisc(ps), traces(ps.traces, 0) {}

	PathStyle&    operator = (pick_ PathStyle& style);

	Txt        Encode() const;
	const char   *Decode(tukk s); // returns NULL on error, otherwise end of part

	bool          IsEmpty() const             { return traces.IsEmpty(); }
	bool          IsSolid(double& wd, Color& co) const;

	void          Serialize(Stream& stream);

	static const PathStyle& empty();
	static const PathStyle& solid();
	static const PathStyle& dash();
	static const PathStyle& dot();
	static const PathStyle& dash_dot();
	static const PathStyle& dash_dot_dot();

public:
	enum // track styles
	{
		LEFT         = 0x00000001,
		LEFT_TOP     = 0x00000002,
		LEFT_BOTTOM  = 0x00000004,
		RIGHT        = 0x00000008,
		RIGHT_TOP    = 0x00000010,
		RIGHT_BOTTOM = 0x00000020,

		PREC         = 5,
	};

	class Trace {
	public:
		Trace() {}
		Trace(double left, double left_top, double left_bottom,
			double right, double right_top, double right_bottom,
			Color color = Null)
			: left(left), left_top(left_top), left_bottom(left_bottom)
			, right(right), right_top(right_top), right_bottom(right_bottom)
			, color(color) {}

		void          Serialize(Stream& stream);

		Txt        Encode() const;
		const char   *Decode(tukk p);

		Rectf         GetExtent() const                    { return Rectf(left, Top(), right, Bottom()); }
		void          Inflate(Pointf amount);
		bool          Contains(Pointf pt) const;
		bool          ContainsHorz(double x) const         { return left <= x && right >= x; }
		bool          ContainsHorz(double l, double r) const { return left <= l && right >= r; }
		double        GetDistance(Pointf pt) const;
		i32           GetTrackStyle(Pointf pt, Pointf tolerance, Pointf& start) const;
		static Image  GetTrackCursor(i32 trackinfo);
		Trace&        Track(Pointf delta, i32 style); // -1 = move
		Trace&        Move(Pointf delta)                      { return Track(delta, -1); }
		Trace&        Move(double dx, double dy)           { return Track(Pointf(dx, dy), -1); }
		Trace&        Bind(const Rectf& rc);
		Trace&        TraceColor(Color c)                  { color = c; return *this; }
		Trace&        Crop(double l, double r);
		Trace&        CropRel(double l, double r)          { return Crop(l, r).Move(-l, 0); }
		Trace         GetTrack(Pointf delta, i32 style) const { Trace t(*this); t.Track(delta, style); return t; }
		Trace         GetMove(Pointf delta) const             { return GetTrack(delta, -1); }
		Trace         GetMove(double dx, double dy) const  { Trace t(*this); t.Track(Pointf(dx, dy), -1); return t; }
		Trace         GetTraceColor(Color c) const         { Trace t(*this); t.TraceColor(c); return t; }
		Trace         GetCrop(double l, double r) const    { Trace t(*this); t.Crop(l, r); return t; }
		Trace         GetCropRel(double l, double r) const { Trace t(*this); t.CropRel(l, r); return t; }

		void          Paint(PlotterTool& tool, bool reduce = false, DRX::Color outline = Null) const;

		double        Width() const                        { return right - left; }
		double        Top() const                          { return min(left_top, right_top); }
		double        Bottom() const                       { return max(left_bottom, right_bottom); }
		double        LeftHeight() const                   { return left_bottom - left_top; }
		double        RightHeight() const                  { return right_bottom - right_top; }
		double        TopHeight() const                    { return right_top - left_top; }
		double        BottomHeight() const                 { return right_bottom - left_bottom; }

		bool          IsHorzEmpty() const                  { return right <= left; }
		bool          IsVertEmpty() const                  { return left_bottom <= left_top && right_bottom <= right_top; }
		bool          IsBothEmpty() const                  { return IsHorzEmpty() && IsVertEmpty(); }
		bool          IsAreaEmpty() const                  { return IsHorzEmpty() || IsVertEmpty(); }

		Pointf        LeftTop() const                      { return Pointf(left, left_top); }
		Pointf        LeftCenter() const                   { return Pointf(left, (left_top + left_bottom) / 2); }
		Pointf        LeftBottom() const                   { return Pointf(left, left_bottom); }
		Pointf        CenterTop() const                    { return Pointf((left + right) / 2, (left_top + right_top) / 2); }
		Pointf        CenterBottom() const                 { return Pointf((left + right) / 2, (left_bottom + right_bottom) / 2); }
		Pointf        RightTop() const                     { return Pointf(right, right_top); }
		Pointf        RightCenter() const                  { return Pointf(right, (right_top + right_bottom) / 2); }
		Pointf        RightBottom() const                  { return Pointf(right, right_bottom); }

		bool          IsEqual(const Trace& t) const;

	public:
		double        left, left_top, left_bottom, right, right_top, right_bottom;
		Color         color; // Null = default line color
	};

	Array<Trace>      traces;

	class Clip
	{
	public:
		Clip() {}

		void          Serialize(Stream& stream);

		Rectf         GetExtent() const;

		void          Write()   { WriteClipboardFormat(*this); }
		bool          Read()    { return ReadClipboardFormat(*this); }
		static bool   IsAvail() { return IsClipboardFormatAvailable<Clip>(); }

		Array<Trace>  traces;
	};

private:
	void          Encode(Txt& out, char tag, double size, const Array<Trace>& list) const;
	const char   *Decode(tukk s, char tag, double& size, Array<Trace>& list);
};

inline bool operator == (const PathStyle::Trace& a, const PathStyle::Trace& b) { return  a.IsEqual(b); }
inline bool operator != (const PathStyle::Trace& a, const PathStyle::Trace& b) { return !a.IsEqual(b); }

class PathStyleMap : Moveable<PathStyleMap>, DeepCopyOption<PathStyleMap>
{
public:
	PathStyleMap() : dirty(false) {}
	PathStyleMap(const PathStyleMap& m, i32)
		: name(m.name), map(m.map, 0), dirty(m.dirty) {}

	void             Serialize(Stream& stream);

	void             SetName(Txt n)                                { name = n; } // todo: dirty ?
	Txt           GetName() const                                  { return name; }

	void             Set(Txt sn, const PathStyle& style)           { map.GetAdd(sn) <<= style; sort.Clear(); }
	void             Remove(Txt sn)                                { i32 i = map.Find(sn); if(i >= 0) { map.Remove(i); sort.Clear(); } }
	void             Rename(i32 i, Txt new_name)                   { map.SetKey(i, new_name); sort.Clear(); }

	void             Touch(bool t = true)                             { dirty = t; }
	bool             IsDirty() const                                  { return dirty; }

	PathStyle        Scan(Txt name, const PathStyle& dflt = PathStyle::solid()) const;
	// accepts NULL this

	const PathStyle& Get(Txt name, const PathStyle& dflt = PathStyle::solid()) const { return map.Get(name, dflt); }
	const PathStyle& operator [] (Txt name) const                  { return Get(name); }

	bool             IsEmpty() const                                  { return map.IsEmpty(); }
	i32              GetCount() const                                 { return map.GetCount(); }
	const Vec<i32>& GetSort() const;
	i32              GetSort(i32 si) const                            { return GetSort()[si]; }
	Txt           GetSortName(i32 si) const                        { return map.GetKey(GetSort(si)); }
	const PathStyle& GetSortStyle(i32 si) const                       { return map[GetSort(si)]; }

	i32              FindSortName(Txt name, i32 exclude = -1) const;
	Txt           GetUniqueName(Txt prefix) const;

	Txt           Export() const;
	void             Import(Txt s, bool update_existing = true); // throw Exc

	static PathStyleMap& App();

public:
	Txt           name;
	ArrayMap<Txt, PathStyle> map;
	mutable Vec<i32> sort;
	bool             dirty;
};

//#define DEBUG_DRAW // comment to turn draw debugging off

class PathDraw
{
public:
	PathDraw();
	PathDraw(Draw& draw, const PathStyle& style, Color color = Black, double width = -10, double dash = Null, bool closed = false);

	void                Clear();
	void                Set(Draw& draw, const PathStyle& style, Color color = Black, double width = -10, double dash = Null, bool closed = false);

	bool                IsEmpty() const                      { return empty; }

	bool                SetExtent(const Rect& rc);
	void                ClearExtent()                        { SetExtent(Null); }

	void                MoveTo(Point pt)                     { (this ->* moveto)(pt); }
	void                MoveTo(i32 x, i32 y)                 { (this ->* moveto)(Point(x, y)); }
	void                LineTo(Point pt)                     { (this ->* lineto)(pt); }
	void                LineTo(i32 x, i32 y)                 { (this ->* lineto)(Point(x, y)); }

	void                Line(Point p, Point q)               { MoveTo(p); LineTo(q); }
	void                Line(i32 x1, i32 y1, i32 x2, i32 y2) { MoveTo(Point(x1, y1)); LineTo(Point(x2, y2)); }

	void                Rectangle(const Rect& rc);
	void                Rectangle(i32 x, i32 y, i32 cx, i32 cy) { Rectangle(RectC(x, y, cx, cy)); }

	void                ArcTo(Point pt, i32 bulge);
	void                ArcTo(i32 x, i32 y, i32 bulge)       { ArcTo(Point(x, y), bulge); }
	void                Arc(Point p, Point q, i32 bulge)     { MoveTo(p); ArcTo(q, bulge); }
	void                Arc(i32 x1, i32 y1, i32 x2, i32 y2, i32 bulge) { MoveTo(Point(x1, y1)); ArcTo(Point(x2, y2), bulge); }

	void                Circle(Point pt, i32 radius);

	void                Paint();

public:
	enum { LINE_BATCH = 6000, POLY_BATCH = 6000 };

	class Output
	{
	public:
		Output(Color color, i32 width, Draw& draw) : color(color), width(width), draw(draw) {}

		bool          MustFlush() const  { return vertices.GetCount() >= LINE_BATCH; }
		void          CheckFlush()       { if(MustFlush()) Flush(); }
		void          Flush();
		void          AddFirst(Point pt);
		void          AddNext(Point pt)  { if(vertices.Top() != pt) { vertices.Add(pt); counts.Top()++; } }
		void          AddNext(const Vec<Point>& list);
		void          AddThick(void (*ln)(Output& out, Point a, Point b), Point start, const Vec<Point>& list);
		Point        *AddSeg(i32 segments, i32 counts);
		Point        *AddSeg(i32 count);

	public:
		Color         color;
		i32           width;
		Draw&         draw;

	private:
		Vec<Point> vertices;
		Vec<i32>   counts;
	};

private:
	struct Split;
	struct Segment;
	enum PART { PART_BEGIN, PART_OPEN, PART_SEGMENT, PART_SKIPEND, PART_END };

	void                MoveToSimple(Point pt);
	void                LineToSimple(Point pt);
	void                MoveToThick(Point pt);
	void                LineToThick(Point pt);
	void                MoveToFull(Point pt);
	void                LineToFull(Point pt);

	void                CalcLineBegin();
	void                AddSegment(const PathStyle::Trace& trace, bool infinite);
	void                Restart();
	void                FlushLines();
	void                LineToRaw();
	Split               SplitLine();
	void                SetSegment(const Segment& s, PART part);
	void                AddPos()                             { pos[3] = pos[2]; pos[2] = pos[1]; pos[1] = pos[0]; }
	void                AddPos(Point p)                      { AddPos(); pos[0] = p; }
	void                FlushSplit(double keep_length);
	void                AddSplit()                           { CheckSplit(); Split s = SplitLine(); split_buffer.AddTail(s); part_total += s.total; CheckSplit(); }
	void                CheckSplit();
	void                RecurseArc(Point next, i32 length, i32 bulge, i32 levels);

	bool                DoBegin();

private:
	const PathStyle    *style;
	Draw               *draw;
	Color               std_color;
	struct Trace
	{
		i32    start, left_top, left_bottom, right_top, right_bottom, width;
		i32    index; // output stream index
		Color  color;
		double pos, top_step, bottom_step;
		Point  last_top, last_bottom;
		void (*hline)(Output& out, Point a, Point b);
		bool   vline, left_ground, right_ground;
	};
	struct Linear
	{
		Linear(i32 top = 0, i32 bottom = 0, Color color = Null, i32 index = 0)
			: top(top), bottom(bottom), color(color), index(index), hline(0) {}
		i32    top, bottom;
		void (*hline)(Output& out, Point a, Point b);
		Point  last_top, last_bottom;
		Color  color;
		i32    index;
		bool   ground;
	};
	struct Segment
	{
		Segment() {}
		Segment(i32 top, i32 bottom, i32 start, i32 end, double length)
			: top(top), bottom(bottom), start(start), end(end), length(length) {}

		bool   IsEmpty() const { return start == end; }

		i32    top;    // top y
		i32    bottom; // bottom y
		i32    start;  // start index in traces array
		i32    end;    // end index in traces array
		double length;
	};
	struct Split
	{
		Split() {}
		Split(double begin, double end, double total, Point pos)
			: begin(begin), end(end), total(total), pos(pos) {}

		double GetStartPos(double excess) const { return 1 - (excess - end) / (total - begin - end); }
		double GetEndPos(double excess) const   { return GetStartPos(excess >= total - begin ? total : excess); }
		double begin, end, total;
		Point  pos; // segment end vertex
	};

	void    (PathDraw::*moveto)(Point pt);
	void    (PathDraw::*lineto)(Point pt);
	void              (*one_thick)(Output& out, Point a, Point b);
	Output             *one_output;
	BiArray<Split>      split_buffer;
//	double              avail_length;
	double              width; // pixel width
	double              dash; // pixel dash length
	double              chamfer_ratio; // a^b < -chamfer_ratio * fabs(a%b) -> chamfer
	double              chamfer_dist;  // B' = B + y*a0.Left() + chamfer_dist*a0
	i32                 clip_radius;   // maximum line radius (distance from original line)
	Rect                clip_rect;
	PART                part, init_part;
	bool                closed;
	bool                empty, init_empty;
	bool                calc_miter;
	Array<Trace>        traces;
	Segment             seg_begin, seg_segment, seg_end, seg, init_seg;
	i32                 next_index, clip_index;
	i32                 miter;
	Array<Linear>       segment_lines;
	Vec<i32>         open_traces;
	double              part_total;
	double              skip_size; // pixels left until beginning of next trace in pixels
	Point              *pos, raw_pos[4], split_pos[4]; // previous positions
	Point               close_line[3];
	Pointf              old_up;
	double              old_ulen;
	bool                old_axb_left;
	ArrayMap<Color, Output> outputs;
};

class PathStyleDisplay : public Display
{
public:
	PathStyleDisplay(const PathStyleMap *path_map = PATH_MAP_NULL);
	virtual ~PathStyleDisplay();

	PathStyleDisplay&   PathMap(const PathStyleMap *pm) { path_map = pm; return *this; }
	const PathStyleMap *GetPathMap() const              { return path_map; }

	virtual void        Paint(Draw& draw, const Rect& rc, const Val& v, Color i, Color p, dword s) const;

protected:
	const PathStyleMap *path_map;
};

}
