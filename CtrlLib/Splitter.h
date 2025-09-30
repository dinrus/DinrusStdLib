class Splitter : public Ctrl {
public:
	virtual void   Layout();
	virtual void   Paint(Draw& draw);
	virtual void   MouseMove(Point p, dword keyflags);
	virtual void   LeftDown(Point p, dword keyflags);
	virtual void   LeftUp(Point p, dword keyflags);
	virtual Image  CursorImage(Point p, dword keyflags);
	virtual void   Serialize(Stream& s);

public:
	struct Style : ChStyle<Style> {
		Val  vert[2], horz[2];
		i32    width;
		bool   dots;
	};

protected: // Because of docking... (will be private)
	Vec<i32> pos;
	Vec<i32> mins;
	Vec<i32> minpx;
	i32         style;
	i32         mouseindex;
	bool        vert;
	i32         inset;
	const Style *chstyle;

	i32         FindIndex(Point client) const;
	i32         GetMins(i32 i) const;
	i32         GetBarWidth() const;
	void        SyncMin();

	static void PaintDots(Draw& w, const Rect& r, bool vert);
	
	friend class SplitterFrame;

public:
	Event<>   WhenSplitFinish;

	void      Set(Ctrl& l, Ctrl& r);

	Splitter& SetPos(i32 newpos, i32 index = 0);
	i32       GetPos(i32 index = 0) const          { return index < pos.GetCount() ? pos[index] : 10000; }

	i32       GetCount() const                     { return GetChildCount(); }

	i32       PosToClient(i32 pos) const;
	i32       ClientToPos(Point client) const;

	void      Zoom(i32 i);
	void      NoZoom()                             { Zoom(-1); }
	i32       GetZoom() const                      { return style; }

	void      SetMin(i32 i, i32 w)                 { mins.At(i, 0) = w; SyncMin(); }
	void      SetMinPixels(i32 i, i32 w)           { minpx.At(i, 0) = w; SyncMin(); }
	
	i32       GetSplitWidth() const;

	void      Add(Ctrl& pane);
	Splitter& operator<<(Ctrl& pane)               { Add(pane); return *this; }
	void      Insert(i32 pos, Ctrl& pane);
	void      Remove(Ctrl& pane);
	void      Swap(Ctrl& pane, Ctrl& newpane);

	static const Style& StyleDefault();

	Splitter& Vert(Ctrl& top, Ctrl& bottom);
	Splitter& Horz(Ctrl& left, Ctrl& right);
	Splitter& Vert()                               { vert = true; Layout(); return *this; }
	Splitter& Horz()                               { vert = false; Layout(); return *this; }
	bool      IsHorz() const                       { return !vert; }
	bool      IsVert() const                       { return vert; }
	Splitter& SetStyle(const Style& s);

	void      Clear();
	void      Reset();

	Splitter();
	virtual ~Splitter();
};

class SplitterFrame : public CtrlFrame, private Ctrl {
public:
	virtual void FrameAdd(Ctrl& parent);
	virtual void FrameRemove();
	virtual void FrameAddSize(Size& sz);
	virtual void FrameLayout(Rect& r);

	virtual void Paint(Draw& draw);
	virtual void LeftDown(Point p, dword keyflags);
	virtual void MouseMove(Point p, dword keyflags);
	virtual void LeftUp(Point p, dword keyflags);
	virtual Image CursorImage(Point p, dword keyflags);

private:
	Point ref;
	Size  parentsize;
	i32   type, minsize, sizemin;
	i32   size, size0;
	const Splitter::Style *style;

	i32   BoundSize();

public:
	enum { LEFT, TOP, RIGHT, BOTTOM };

	void Serialize(Stream& s);

	SplitterFrame& Set(Ctrl& c, i32 size, i32 type);
	SplitterFrame& Left(Ctrl& c, i32 size)    { return Set(c, size, LEFT); }
	SplitterFrame& Top(Ctrl& c, i32 size)     { return Set(c, size, TOP); }
	SplitterFrame& Right(Ctrl& c, i32 size)   { return Set(c, size, RIGHT); }
	SplitterFrame& Bottom(Ctrl& c, i32 size)  { return Set(c, size, BOTTOM); }

	SplitterFrame& MinSize(i32 sz)            { minsize = sz; return *this; }
	SplitterFrame& SizeMin(i32 sz)            { sizemin = sz; return *this; }
	SplitterFrame& SetStyle(const Splitter::Style& s);

	i32  GetType() const                      { return type; }
	i32  GetSize() const                      { return size; }
	void SetSize(i32 sz)                      { size = sz; RefreshParentLayout(); }

	void Show(bool show = true)				  { Ctrl::Show(show); }
	void Hide()								  { Ctrl::Hide(); }
	bool IsShown()							  { return Ctrl::IsShown(); }

	SplitterFrame();
};
