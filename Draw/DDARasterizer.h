class DDARasterizer {
	struct Segment : Moveable<Segment> {
		i32  l;
		i32  h;
		bool flag;
		
		bool operator<(const Segment& b) const { return h < b.h; }
	};
	
	struct Segments;
	
	i32           cy;
	i32           diry;
	i32           dirx;
	One<Segments> pseg;
	Point         p0, p1;
	Point         j1, j2;
	Point         b1, b2;
	i32           width;
	bool          close;

	void AHorz(i32 x, i32 y, i32 cx);
	void AVert(i32 x, i32 y, i32 cy);
	void DoLine(Point p1, Point p2, bool last);
	void FatLine(Point p2);

public:
	virtual void PutHorz(i32 x, i32 y, i32 cx) = 0;
	virtual void PutVert(i32 x, i32 y, i32 cy) = 0;
	
	i32  GetDirx() const                      { return dirx; }
	i32  GetDiry() const                      { return diry; }
	
	DDARasterizer& Move(Point p);
	DDARasterizer& Line(Point p);
	DDARasterizer& Close();
	
	DDARasterizer& Polygon();
	DDARasterizer& Fill();
	
	DDARasterizer& Ellipse(const Rect& rect);
	
	DDARasterizer& Width(i32 width_)          { width = width_; return *this; }

	void Cy(i32 cy_)                          { cy = cy_; }
	
	DDARasterizer();
	virtual ~DDARasterizer();
};
