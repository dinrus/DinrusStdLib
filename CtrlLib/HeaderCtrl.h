class HeaderCtrl : public Ctrl, public CtrlFrame {
public:
	virtual void  CancelMode();
	virtual void  Paint(Draw& draw);
	virtual Image CursorImage(Point p, dword keyflags);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  LeftDouble(Point p, dword keyflags);
	virtual void  LeftDrag(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword keyflags);
	virtual void  MouseLeave();
	virtual void  LeftUp(Point, dword);
	virtual void  RightDown(Point, dword);
	virtual void  Serialize(Stream& s);
	virtual void  Layout();

	virtual void  FrameAdd(Ctrl& parent);
	virtual void  FrameRemove();
	virtual void  FrameLayout(Rect& r);
	virtual void  FrameAddSize(Size& sz);

public:
	struct Style : ChStyle<Style> {
		Val look[4];
		i32   gridadjustment;
		bool  pressoffset;
	};

	class Column : public LabelBase {
	protected:
		virtual void  LabelUpdate();

		Txt      tip;
		HeaderCtrl *header;
		double      ratio;
		i32         min, max;
		i32         margin;
		Color       paper;
		i32         index;
		bool        visible;

		void        Paint(bool& first, Draw& w,
		                  i32 x, i32 y, i32 cx, i32 cy, bool disabled, bool push, bool hl);

		friend class HeaderCtrl;

	public:
		Event<>     WhenLeftClick;
		Event<>     WhenLeftDouble;
		Event<>     WhenAction;
		Event<Bar&> WhenBar;

		Column&  Min(i32 _min)                     { min = _min; return *this; }
		Column&  Max(i32 _max)                     { max = _max; return *this; }
		Column&  MinMax(i32 m, i32 n)              { return Min(m).Max(n); }
		Column&  Fixed(i32 f)                      { return MinMax(f, f); }
		Column&  Tip(tukk s)                { tip = s; return *this; }
		Column&  SetPaper(Color c)                 { paper = c; return *this; }
		Column&  SetRatio(double ratio);
		Column&  SetMargin(i32 m);

		void     Show(bool b = true);
		void     Hide()                            { Show(false); }

		i32      GetMargin() const                 { return margin + 2; }
		Color    GetPaper() const                  { return paper; }
		i32      GetIndex() const                  { return index; }
		bool     IsVisible() const                 { return visible; }
		double   GetRatio() const                  { return ratio; }

		Column();
	};

	friend class Column;

	enum { PROPORTIONAL, REDUCELAST, REDUCENEXT, SCROLL, FIXED };

protected:

	Array<Column> col;
	HScrollBar    sb;

	mutable i32          oszcx;
	mutable Vec<Rect> tabrect;

	i32   split, pushi, li, ti;
	bool  isdrag;
	Image dragtab;
	i32   dragd, dragx;
	Rect  colRect;
	bool  push:1;
	bool  track:1;
	bool  invisible:1;
	bool  moving:1;
	u8  mode;
	i32   light;
	i32   height;
	const Style *style;
	bool  autohidesb;

	double Denominator() const;
	void   Reduce(i32 q, double& delta, double rs, i32 szcx, bool checkmin);
	i32    GetNextTabWidth(i32 i);
	i32    GetLastVisibleTab();
	void   WScroll();
	void   Scroll();
	void   ScrollVisibility();
	void   DoSbTotal();
	void   SbTotal();
	void   SetTabWidth0(i32 i, i32 cx);
	i32    SumMin(i32 from);
	i32    SumMax(i32 from);
	void   Distribute(const Vec<i32>& sci, double delta);
	void   ReCompute();
	void   InvalidateDistribution()                              { oszcx = -1; }
	void   RefreshDistribution();
	Vec<i32> GetVisibleCi(i32 from);
	
	friend class ArrayCtrl;

public:
	Event<>       WhenLayout;
	Event<>       WhenScroll;
	Event<>       WhenScrollVisibility;

	Rect          GetTabRect(i32 i);

	const Column& Tab(i32 i) const                        { return col[i]; }
	Column&       Tab(i32 i);

	Column&       Add(tukk text, double ratio = 0);
	Column&       Add();
	const Column& operator[](i32 i) const                 { return col[i]; }
	i32           GetCount() const                        { return col.GetCount(); }
	void          Reset();

	void          ShowTab(i32 i, bool show = true);
	void          HideTab(i32 i)                          { ShowTab(i, false); }
	bool          IsTabVisible(i32 i)                     { return col[i].visible; }
	void          SetTabRatio(i32 i, double ratio);
	double        GetTabRatio(i32 i) const                { return InvZxf(col[i].ratio); }
	void          SetTabWidth(i32 i, i32 cx);
	i32           GetTabWidth(i32 i);
	
	void          SwapTabs(i32 first, i32 second);
	void          MoveTab(i32 from, i32 to);
	i32           GetTabIndex(i32 i) const                { return col[i].index; }
	i32           FindIndex(i32 ndx) const;

	void          StartSplitDrag(i32 s);
	i32           GetSplit(i32 x);

	i32           GetScroll() const                       { return sb; }
	bool          IsScroll() const                        { return sb.IsShown(); }

	void          SetHeight(i32 cy);
	i32           GetHeight() const;
	
	i32           GetMode() const                         { return mode; }
	
	static const Style& StyleDefault();

	HeaderCtrl&   Invisible(bool inv);
	HeaderCtrl&   Track(bool _track = true)               { track = _track; sb.Track(track); return *this; }
	HeaderCtrl&   NoTrack()                               { return Track(false); }
	HeaderCtrl&   Proportional();
	HeaderCtrl&   ReduceNext();
	HeaderCtrl&   ReduceLast();
	HeaderCtrl&   Absolute();
	HeaderCtrl&   Fixed();
	HeaderCtrl&   SetStyle(const Style& s)                { style = &s; Refresh(); return *this; }
	HeaderCtrl&   Moving(bool b = true)                   { moving = b; return *this; }
	HeaderCtrl&   AutoHideSb(bool b = true)               { autohidesb = b; SbTotal(); return *this; }
	HeaderCtrl&   NoAutoHideSb()                          { return AutoHideSb(false); }
	HeaderCtrl&   HideSb(bool b = true)                   { sb.Show(!b); return *this; }

	HeaderCtrl&   SetScrollBarStyle(const ScrollBar::Style& s)   { sb.SetStyle(s); return *this; }

	static i32 GetStdHeight()                             { return Draw::GetStdFontCy() + 4; }

	typedef HeaderCtrl CLASSNAME;

	HeaderCtrl();
	virtual ~HeaderCtrl();
};
