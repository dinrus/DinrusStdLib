class ToolWin : public TopWindow, public CtrlFrame {
public:
	virtual void   FrameAddSize(Size& sz);
	virtual void   FrameLayout(Rect& r);
	virtual void   FramePaint(Draw& w, const Rect& r);

	virtual void   MouseMove(Point p, dword keyflags);
	virtual void   LeftUp(Point p, dword keyflags);
	virtual Image  CursorImage(Point p, dword keyflags);
	virtual Image  FrameMouseEvent(i32 event, Point p, i32 zdelta, dword keyflags);

private:
	Point      p0;
	Rect       rect0;
	Point      dragdir;
	ToolButton close;

	void DoClose();

	Point GetDir(Point p) const;
	i32   GetTitleCy() const;
	i32   GetBorder() const;
	Rect  GetMargins() const;
	Size  AddMargins(Size sz) const;

	void StartMouseDrag0();

	typedef ToolWin CLASSNAME;

public:
	virtual void MoveBegin();
	virtual void Moving();
	virtual void MoveEnd();

	void StartMouseDrag();
	void SetClientRect(Rect r);

	void PlaceClientRect(Rect r);

	ToolWin();
};
