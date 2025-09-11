#ifndef _CtrlLib_MultiButton_h_
#define _CtrlLib_MultiButton_h_

class MultiButton : public Ctrl {
public:
	virtual void  Paint(Draw& w);
	virtual void  MouseMove(Point p, dword flags);
	virtual void  LeftDown(Point p, dword flags);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  MouseLeave();
	virtual void  CancelMode();
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual void  SetData(const Val& data);
	virtual Val GetData() const;
	virtual Size  GetMinSize() const;
	virtual i32   OverPaint() const;

public:
	struct Style : public ChStyle<Style> {
		Val edge[4];
		Val coloredge;
		bool  activeedge;
		Val look[4];
		Val left[4]; // leftmost button on the left side
		Val lmiddle[4]; // other buttons on the left size
		Val right[4]; // rightmost button on the right side
		Val rmiddle[4]; // other buttons on the right size
		Val simple[4];
		i32   border;
		Val trivial[4];
		i32   trivialborder;
		Color monocolor[4]; // color of mono images and text in nonframe situation
		Color fmonocolor[4]; // color of mono images and text in frame situation
		Point pressoffset; // offset of images and text if button is pressed
		Val sep1, sep2;
		i32   sepm;
		i32   stdwidth; // standard width of button
		bool  trivialsep;
		bool  usetrivial;
		Rect  margin;
		i32   overpaint;
		i32   loff, roff;
		Color error;
		bool  clipedge; // Clip border edge so that it does not paint area where are buttons
		Color paper; // normal paper color for DropList (std SColorPaper)
	};

	class SubButton {
		friend class MultiButton;

		Txt       label;
		Txt       tip;
		MultiButton *owner = nullptr;
		Image        img;
		i32          cx;
		bool         main;
		bool         left;
		bool         monoimg;
		bool         enabled;
		bool         visible;

		void Refresh();

	public:
		Event<>  WhenPush;
		Event<>  WhenClick;

		SubButton& SetImage(const Image& m);
		SubButton& SetMonoImage(const Image& m);
		SubButton& SetStdImage();
		SubButton& SetLabel(tukk text);
		SubButton& Left(bool b = true);
		SubButton& Width(i32 w);
		SubButton& Enable(bool b);
		SubButton& Show(bool b);
		SubButton& Tip(tukk s)                     { tip = s; return *this; }
		SubButton& Main(bool b = true);

		Event<>  operator<<=(Event<>  cb)                 { WhenPush = cb; return cb; }

		SubButton();
	};

private:
	enum {
		NONE = -2,
		MAIN = -1,
		LB_IMAGE = 5, // image <-> text space
		LB_MARGIN = 10
	};

	virtual bool Frame();

	DisplayPopup     info;
	Array<SubButton> buttons;

	Val            value;
	Val            error;
	Rect16           pushrect;

	const Display   *display;
	const Convert   *convert;
	const Style     *style;

	i32              valuecy;
	Color            paper = Null;

	i16            hl;
	bool             push:1;
	bool             nobg:1;
	bool             droppush:1;

	i32  FindButton(i32 px);
	void Margins(i32& l, i32& r);
	Rect ContentRect();
	void GetLR(i32& lx, i32& rx);
	bool GetPos(SubButton& b,  i32& lx, i32& rx, i32& x, i32& cx, i32 px = -1);
	void GetPos(i32 ii, i32& x, i32& cx);
	i32  ChState(i32 i);
	Rect GetMargin();
	void Lay(Rect& r, bool minsize = false);
	bool ComplexFrame();
	bool Metrics(i32& border, i32& lx, i32 &rx, const Rect& r);
	bool Metrics(i32& border, i32& lx, i32 &rx);
	void SyncInfo();
	Rect Paint0(Draw& w, bool getcr);
	void DoPush(i32 i);
	void MultiButtons();
	SubButton& Button(i32 i) const;
	bool HasMain() const;
	void MainPush();
	void MainClick();

	friend class SubButton;
	friend class MultiButtonFrame;

protected:
	enum {
		ATTR_TIP = Ctrl::ATTR_LAST,
		ATTR_LAST
	};

	virtual void DropPush();

public:
	Event<> WhenPush;
	Event<> WhenClick;

	static const Style& StyleDefault();
	static const Style& StyleFrame();

	bool IsTrivial() const;

	void Reset();

	void PseudoPush(i32 bi);
	void PseudoPush();

	SubButton& AddButton();
	SubButton& InsertButton(i32 i);
	void       RemoveButton(i32 i);
	i32        GetButtonCount() const;
	const MultiButton::SubButton& GetButton(i32 i) const;
	SubButton& GetButton(i32 i);
	SubButton& MainButton();

	Rect  GetPushScreenRect() const                  { return pushrect; }

	const Display& GetDisplay() const                { return *display; }
	const Convert& GetConvert() const                { return *convert; }
	const Val&   Get() const                       { return value; }
	
	void  Error(const Val& v)                      { error = v; Refresh(); }
	
	void  SetPaper(Color c)                          { paper = c; }

	MultiButton& SetDisplay(const Display& d);
	MultiButton& NoDisplay();
	MultiButton& SetConvert(const Convert& c);
	MultiButton& SetValCy(i32 cy);
	MultiButton& Set(const Val& v, bool update = true);
	MultiButton& Tip(tukk s)                  { SetTextAttr(ATTR_TIP, s); return *this; }
	MultiButton& NoBackground(bool b = true);

	MultiButton& SetStyle(const Style& s)            { style = &s; Refresh(); return *this; }
	
	void SetupDropPush()                             { droppush = true; }

	MultiButton();
};

class MultiButtonFrame : public MultiButton, public CtrlFrame {
public:
	virtual void  FrameLayout(Rect& r);
	virtual void  FrameAddSize(Size& sz);
	virtual void  FrameAdd(Ctrl& parent);
	virtual void  FrameRemove();

private:
	virtual bool Frame();

public:
	void AddTo(Ctrl& w);
};

#endif
