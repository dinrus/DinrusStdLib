struct LinearPathConsumer {
	virtual void Move(const Pointf& p) = 0;
	virtual void Line(const Pointf& p) = 0;
	virtual void End();
};

void ApproximateQuadratic(LinearPathConsumer& t,
                          const Pointf& p1, const Pointf& p2, const Pointf& p3,
                          double tolerance);
void ApproximateCubic(LinearPathConsumer& t,
                      const Pointf& x0, const Pointf& x1, const Pointf& x2, const Pointf& x,
                      double tolerance);

struct LinearPathFilter : LinearPathConsumer {
	virtual void End();

	LinearPathConsumer *target;

	void PutMove(const Pointf& p)               { target->Move(p); }
	void PutLine(const Pointf& p)               { target->Line(p); }
	void PutEnd()                               { target->End(); }
};

class Stroker : public LinearPathFilter {
public:
	virtual void Move(const Pointf& p);
	virtual void Line(const Pointf& p);
	virtual void End();

private:
	double w2;
	double qmiter;
	double fid;
	double tw;

	Pointf p0, v0, o0, a0, b0;
	Pointf p1, v1, o1, a1, b1;
	Pointf p2;
	i32    linecap;
	i32    linejoin;
	Rectf  preclip;
	i32    lines;

	void   Finish();
	void   Round(const Pointf& p, const Pointf& v1, const Pointf& v2, double r);
	void   Cap(const Pointf& p0, const Pointf& v0, const Pointf& o0,
	           const Pointf& a0, const Pointf& b0);
	bool   PreClipped(Pointf p2, Pointf p3);

public:
	void Init(double width, double miterlimit, double tolerance, i32 linecap, i32 linejoin, const Rectf& preclip);
};

class Dasher : public LinearPathFilter {
public:
	virtual void Move(const Pointf& p);
	virtual void Line(const Pointf& p);

private:
	const Vec<double> *pattern;
	i32            patterni;
	double         sum, rem;
	bool           flag;
	Pointf         p0;

	void    Put(const Pointf& p);

public:
	void Init(const Vec<double>& pattern, double distance);
};

struct Transformer : public LinearPathFilter {
public:
	virtual void Move(const Pointf& p);
	virtual void Line(const Pointf& p);

private:
	const Xform2D& xform;

public:
	Transformer(const Xform2D& xform) : xform(xform) {}
};

class Rasterizer : public LinearPathConsumer {
public:
	virtual void Move(const Pointf& p);
	virtual void Line(const Pointf& p);

private:
	struct Cell : Moveable<Cell> {
		i16 x;
		i16 cover;
		i32   area;

		bool operator<(const Cell& b) const { return x < b.x; }
    };

    enum { SVO_ALLOC = 15 };

	struct CellArray {
		i32    count;
		i32    alloc;
		union {
			Cell *ptr;
			Cell  svo[SVO_ALLOC];
		};

		Cell *Get()         { return alloc == SVO_ALLOC ? svo : ptr; }

		CellArray()         { count = 0; alloc = SVO_ALLOC; }
	};

	Rectf                   cliprect;
	Pointf                  p0;
	Buffer<CellArray>       cell;

	i32                     min_y;
	i32                     max_y;
	Size                    sz;
	i32                     mx;

	void  Init();
	Cell *AddCells(i32 y, i32 n);
	void  AddCells2(CellArray& a, i32 n);
	void  RenderHLine(i32 ey, i32 x1, i32 y1, i32 x2, i32 y2);
	void  LineClip(double x1, double y1, double x2, double y2);
	i32   CvX(double x);
	i32   CvY(double y);
	void  CvLine(double x1, double y1, double x2, double y2);
	bool  BeginRender(i32 y, const Cell *&c, const Cell *&e);
	void  Free();

	static i32 Q8Y(double y) { return i32(y * 256 + 0.5); }
	i32 Q8X(double x)        { return i32(x * mx + 0.5); }

public:
	struct Filler {
		virtual void Start(i32 x, i32 len) = 0;
		virtual void Render(i32 val) = 0;
		virtual void Render(i32 val, i32 len) = 0;
		virtual void End();
	};

	void LineRaw(i32 x1, i32 y1, i32 x2, i32 y2);

	void  SetClip(const Rectf& rect);
	Rectf GetClip() const                     { return cliprect; }

	i32  MinY() const                         { return min_y; }
	i32  MaxY() const                         { return max_y; }
	bool NotEmpty(i32 y)                      { return cell[y].count; }
	void Render(i32 y, Filler& g, bool evenodd);

	void Reset();

	void Create(i32 cx, i32 cy, bool subpixel);

	Rasterizer(i32 cx, i32 cy, bool subpixel) { Create(cx, cy, subpixel); }
	Rasterizer()                              { sz = Size(0, 0); }
	~Rasterizer()                             { Free(); }
};

class LinearInterpolator {
	struct Dda2 {
		i32 count, lift, rem, mod, p;

		void  Set(i32 a, i32 b, i32 len);
		i32   Get();
	};

	Xform2D xform;
	Dda2    ddax, dday;

	static i32 Q8(double x) { return i32(256 * x + 0.5); }

public:
	void   Set(const Xform2D& m)                    { xform = m; }

	void   Begin(i32 x, i32 y, i32 len);
	Point  Get();
};

void ApproximateChar(LinearPathConsumer& t, Pointf at, i32 ch, Font fnt, double tolerance);
