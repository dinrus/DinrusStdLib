class ScrollBar : public FrameCtrl<Ctrl>, private VirtualButtons {
public:
	virtual void Layout();
	virtual Size GetStdSize() const;
	virtual void Paint(Draw& draw);
	virtual void LeftDown(Point p, dword);
	virtual void MouseMove(Point p, dword);
	virtual void MouseEnter(Point p, dword);
	virtual void MouseLeave();
	virtual void LeftUp(Point p, dword);
	virtual void LeftRepeat(Point p, dword);
	virtual void MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual void CancelMode();

	virtual void FrameLayout(Rect& r);
	virtual void FrameAddSize(Size& sz);

	virtual Image MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags);

	virtual i32                  ButtonCount() const;
	virtual Rect                 ButtonRect(i32 i) const;
	virtual const Button::Style& ButtonStyle(i32 i) const;
	virtual bool                 ButtonEnabled(i32 i) const;

	virtual void ButtonPush(i32 i);
	virtual void ButtonRepeat(i32 i);

public:
	struct Style : ChStyle<Style> {
		Color bgcolor;
		i32 barsize, arrowsize, thumbmin, overthumb, thumbwidth;
		bool through;
		Val vupper[4], vthumb[4], vlower[4];
		Val hupper[4], hthumb[4], hlower[4];
		Button::Style up, down, left, right;
		Button::Style up2, down2, left2, right2;
		bool          isup2, isdown2, isleft2, isright2;
	};

private:
	i32     thumbpos;
	i32     thumbsize;
	i32     delta;

	enum { PREV, PREV2, NEXT, NEXT2, BUTTONCOUNT };
//	Button  prev, prev2, next, next2;
	i32     pagepos;
	i32     pagesize;
	i32     totalsize;
	i32     linesize;
	i32     minthumb;
	i8    push;
	i8    light;
	bool    horz:1;
	bool    jump:1;
	bool    track:1;
	bool    autohide:1;
	bool    autodisable:1;
	bool    is_active:1;

	const Style *style;

	Rect    Slider(i32& cc) const;
	Rect    Slider() const;
	i32&    HV(i32& h, i32& v) const;
	i32     GetHV(i32 h, i32 v) const;
	Rect    GetPartRect(i32 p) const;
	void    Bounds();
	bool    SetThumb(i32 _thumbpos, i32 _thumbsize);
	void    Drag(Point p);
	i32     GetMousePart();
	i32     GetRange() const;

	void    Position();
	void    Uset(i32 a);

	i32     ScrollBarSize() const           { return style->barsize; }

public:
	Event<>  WhenScroll;
	Event<>  WhenVisibility;
	Event<>  WhenLeftClick;

	bool    IsHorz() const                  { return horz; }
	bool    IsVert() const                  { return !horz; }

	void    Set(i32 pagepos, i32 pagesize, i32 totalsize);

	bool    Set(i32 pagepos);
	void    SetPage(i32 pagesize);
	void    SetTotal(i32 totalsize);
	
	bool    IsActive() const                { return is_active; }

	bool    ScrollInto(i32 pos, i32 linesize);
	bool    ScrollInto(i32 pos)             { return ScrollInto(pos, linesize); }

	bool    VertKey(dword key, bool homeend = true);
	bool    HorzKey(dword key);

	void    PrevLine();
	void    NextLine();
	void    PrevPage();
	void    NextPage();
	void    Begin();
	void    End();

	void    Wheel(i32 zdelta, i32 lines);
	void    Wheel(i32 zdelta);

	Size    GetViewSize() const;
	Size    GetReducedViewSize() const;

	i32     Get() const                     { return pagepos; }
	i32     GetPage() const                 { return pagesize; }
	i32     GetTotal() const                { return totalsize; }
	i32     GetLine() const                 { return linesize; }
	
	Rect    GetSliderRect() const           { return Slider(); }
	i32     GetSliderPos(i32 pos) const;

	static const Style& StyleDefault();

	ScrollBar& Horz(bool b = true)          { horz = b; Refresh(); RefreshLayout(); return *this; }
	ScrollBar& Vert()                       { return Horz(false); }

	ScrollBar& SetLine(i32 _line)           { linesize = _line; return *this; }

	ScrollBar& Track(bool b = true)         { track = b; return *this; }
	ScrollBar& NoTrack()                    { return Track(false); }
	ScrollBar& Jump(bool b = true)          { jump = b; return *this; }
	ScrollBar& NoJump(bool b = true)        { return Jump(false); }
	ScrollBar& AutoHide(bool b = true);
	ScrollBar& NoAutoHide()                 { return AutoHide(false); }
	bool       IsAutoHide() const           { return autohide; }
	ScrollBar& AutoDisable(bool b = true);
	ScrollBar& NoAutoDisable()              { return AutoDisable(false); }
	ScrollBar& MinThumb(i32 sz)             { minthumb = sz; return *this; }
	ScrollBar& SetStyle(const Style& s);

	operator i32() const                    { return pagepos; }
	i32 operator=(i32 pagepos)              { Set(pagepos); return pagepos; }

	ScrollBar();
	virtual ~ScrollBar();
};

inline i32 ScrollBarSize()                  { return ScrollBar::StyleDefault().barsize; }//!!

class VScrollBar : public ScrollBar {
public:
	i32 operator=(i32 pagepos)              { Set(pagepos); return pagepos; }
};

class HScrollBar : public ScrollBar {
public:
	i32 operator=(i32 pagepos)              { Set(pagepos); return pagepos; }

	HScrollBar() { Horz(); }
};

class SizeGrip : public FrameRight<Ctrl> {
public:
	virtual void  Paint(Draw& w);
	virtual void  LeftDown(Point p, dword);
	virtual Image CursorImage(Point p, dword);

public:
	SizeGrip();
	virtual ~SizeGrip();
};

class ScrollBars : public CtrlFrame {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FrameAddSize(Size& sz);
	virtual void FramePaint(Draw& w, const Rect& r);
	virtual void FrameAdd(Ctrl& ctrl);
	virtual void FrameRemove();

protected:
	Ctrl      *box;
	ParentCtrl the_box;
	
	
	StaticRect box_bg;
	i32        box_type;
	SizeGrip   grip;

	void    Scroll();

public:
	HScrollBar x;
	VScrollBar y;

	Event<>    WhenScroll;
	Event<>    WhenLeftClick;

	void    Set(Point pos, Size page, Size total);
	bool    Set(Point pos);
	bool    Set(i32 x, i32 y);
	void    SetPage(Size page);
	void    SetPage(i32 cx, i32 cy);
	void    SetTotal(Size total);
	void    SetTotal(i32 cx, i32 cy);

	void    SetX(i32 pos, i32 page, i32 total)       { x.Set(pos, page, total); }
	bool    SetX(i32 _x)                             { return x.Set(_x); }
	void    SetPageX(i32 cx)                         { x.SetPage(cx); }
	void    SetTotalX(i32 cx)                        { x.SetTotal(cx); }

	void    SetY(i32 pos, i32 page, i32 total)       { y.Set(pos, page, total); }
	bool    SetY(i32 _y)                             { return y.Set(_y); }
	void    SetPageY(i32 cy)                         { y.SetPage(cy); }
	void    SetTotalY(i32 cy)                        { y.SetTotal(cy); }

	bool    ScrollInto(Point pos, Size linesize);
	bool    ScrollInto(const Rect& r)                { return ScrollInto(r.TopLeft(), r.Size()); }
	bool    ScrollInto(Point pos);

	bool    ScrollIntoX(i32 pos, i32 linesize)       { return x.ScrollInto(pos, linesize); }
	bool    ScrollIntoX(i32 pos)                     { return x.ScrollInto(pos); }
	bool    ScrollIntoY(i32 pos, i32 linesize)       { return y.ScrollInto(pos, linesize); }
	bool    ScrollIntoY(i32 pos)                     { return y.ScrollInto(pos); }

	bool    Key(dword key);

	void    LineUp()                                 { y.PrevLine(); }
	void    LineDown()                               { y.NextLine(); }
	void    PageUp()                                 { y.PrevPage(); }
	void    PageDown()                               { y.NextPage(); }
	void    VertBegin()                              { y.Begin(); }
	void    VertEnd()                                { y.End(); }

	void    LineLeft()                               { x.PrevLine(); }
	void    LineRight()                              { x.NextLine(); }
	void    PageLeft()                               { x.PrevPage(); }
	void    PageRight()                              { x.NextPage(); }
	void    HorzBegin()                              { x.Begin(); }
	void    HorzEnd()                                { x.End(); }

	void    WheelX(i32 zdelta)                       { x.Wheel(zdelta); }
	void    WheelY(i32 zdelta)                       { y.Wheel(zdelta); }

	Size    GetViewSize() const;
	Size    GetReducedViewSize() const;

	Point   Get() const                              { return Point(x, y); }
	i32     GetX() const                             { return x; }
	i32     GetY() const                             { return y; }
	Size    GetPage() const                          { return Size(x.GetPage(), y.GetPage()); }
	Size    GetTotal() const                         { return Size(x.GetTotal(), y.GetTotal()); }
	Size    GetLine() const                          { return Size(x.GetLine(), y.GetLine()); }

	void    ShowX(bool show)                         { x.Show(show); }
	void    HideX()                                  { ShowX(false); }
	void    ShowY(bool show)                         { y.Show(show); }
	void    HideY()                                  { ShowY(false); }
	void    Show(bool show = true)                   { x.Show(show); y.Show(show); }
	void    Hide()                                   { Show(false); }
	
	ScrollBars& SetLine(i32 linex, i32 liney);
	ScrollBars& SetLine(Size line)                   { return SetLine(line.cx, line.cy); }
	ScrollBars& SetLine(i32 line)                    { return SetLine(line, line); }

	ScrollBars& Track(bool b = true);
	ScrollBars& NoTrack()                            { return Track(false); }
	ScrollBars& Jump(bool b = true);
	ScrollBars& NoJump(bool b = true)                { return Jump(false); }
	ScrollBars& AutoHide(bool b = true);
	ScrollBars& NoAutoHide()                         { return AutoHide(false); }
	ScrollBars& AutoDisable(bool b = true);
	ScrollBars& NoAutoDisable()                      { return AutoDisable(false); }

	ScrollBars& NormalBox();
	ScrollBars& NoBox();
	ScrollBars& FixedBox();

	ScrollBars& Box(Ctrl& box);
	ScrollBars& WithSizeGrip();

	ScrollBars& SetStyle(const ScrollBar::Style& s)  { x.SetStyle(s); y.SetStyle(s); return *this; }

	operator Point() const                           { return Get(); }
	Point operator=(Point p)                         { Set(p); return p; }

	ScrollBars();
	virtual ~ScrollBars();
};

class Scroller {
	Point psb;

public:
	void Scroll(Ctrl& p, const Rect& rc, Point newpos, Size cellsize = Size(1, 1));
	void Scroll(Ctrl& p, const Rect& rc, i32 newpos, i32 linesize = 1);
	void Scroll(Ctrl& p, Point newpos);
	void Scroll(Ctrl& p, i32 newposy);

	void Set(Point pos)          { psb = pos; }
	void Set(i32 pos)            { psb = Point(0, pos); }
	void Clear()                 { psb = Null; }

	Scroller()                   { psb = Null; }
};
