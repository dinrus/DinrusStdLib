struct SDraw : Draw, DDARasterizer {
	virtual dword GetInfo() const;
	
	virtual void BeginOp();
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual void OffsetOp(Point p);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual void EndOp();
	virtual bool IsPaintingOp(const Rect& r) const;

	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, Color color);
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color);
	virtual void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);

	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink, i32 n, i32k *dx);

	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count, i32k *subpolygon_counts, i32 scc, i32k *disjunct_polygon_counts, i32 dpcc, Color color, i32 width, Color outline, zu64 pattern, Color doxor);
	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count, i32k *counts, i32 count_count, i32 width, Color color, Color doxor);
	
	virtual void PutHorz(i32 x, i32 y, i32 cx);
	virtual void PutVert(i32 x, i32 y, i32 cy);

private:
	struct Cloff {
		Vec<Rect> clip;
		Point        offset;
		
		void operator<<=(const Cloff& b)      { clip <<= b.clip; offset = b.offset; }
	};
	
	bool  paintonly;
	
	Array<Cloff> cloff;
	
	Color docolor;

public:
	virtual void  PutImage(Point p, const Image& img, const Rect& src) = 0;
	virtual void  PutImage(Point p, const Image& img, const Rect& src, Color color);
	virtual void  PutRect(const Rect& r, Color color) = 0;
	virtual Image RenderGlyph(Point at, i32 angle, i32 chr, Font fnt, Color color, Size sz);
	
	Point GetOffset() const                                       { return cloff.Top().offset; }

	void Init(const Rect& r);
	void Init(const Vec<Rect>& rs, i32 height, Point offset = Point(0, 0));
	
	void PaintOnly()                                              { paintonly = true; }

	SDraw()                                                       { paintonly = false; }
};

class SImageDraw1 : public SDraw {
protected:
	ImageBuffer ib;
	friend class SImageDraw;

public:
	virtual void  PutImage(Point p, const Image& m, const Rect& src);
	virtual void  PutRect(const Rect& r, Color color);

	void Create(Size sz);
};

class SImageDraw : public SImageDraw1 {
	SImageDraw1 alpha;
	
	bool has_alpha;

public:
	Draw& Alpha();

	operator Image() const;
	
	SImageDraw(Size sz);
	SImageDraw(i32 cx, i32 cy);
};
