class BarPane : public ParentCtrl {
public:
	virtual void LeftDown(Point pt, dword keyflags);
	virtual void MouseMove(Point p, dword);

private:
	struct Item {
		Ctrl *ctrl;
		i32   gapsize;
	};
	Array<Item>    item;
	Vec<i32>    breakpos;
	bool           horz;
	Ctrl          *phelpctrl;
	i32            vmargin, hmargin;
	bool           menu;

	Size     LayOut(bool horz, i32 maxsize, bool repos);

public:
	Event<>  WhenLeftClick;

	void  PaintBar(Draw& w, const SeparatorCtrl::Style& ss,
	               const Val& pane, const Val& iconbar = Null, i32 iconsz = 0);

	void  IClear();
	void  Clear();
	bool  IsEmpty() const                    { return item.IsEmpty(); }

	void  Add(Ctrl *ctrl, i32 gapsize);
	void  AddBreak()                         { Add(NULL, Null); }
	void  AddGap(i32 gapsize)                { Add(NULL, gapsize); }

	void  Margin(i32 v, i32 h)               { vmargin = v; hmargin = h; }

	Size  Repos(bool horz, i32 maxsize);
	Size  GetPaneSize(bool _horz, i32 maxsize) const;

	i32   GetCount() const                   { return breakpos.GetCount() + 1; }

	void  SubMenu()                          { menu = true; }

	BarPane();
	virtual ~BarPane();
};

class Bar : public Ctrl {
public:
	struct Item {
		virtual Item& Text(tukk text);
		virtual Item& Key(dword key);
		virtual Item& Repeat(bool repeat = true);
		virtual Item& Image(const class Image& img);
		virtual Item& Check(bool check);
		virtual Item& Radio(bool check);
		virtual Item& Enable(bool _enable = true);
		virtual Item& Bold(bool bold = true);
		virtual Item& Tip(tukk tip);
		virtual Item& Help(tukk help);
		virtual Item& Topic(tukk topic);
		virtual Item& Description(tukk desc);
		virtual void  FinalSync();

		Item&   Label(tukk text);
		Item&   RightLabel(tukk text);

		Item& Key(KeyInfo& (*key)());

		Item();
		virtual ~Item();
	};

protected:
	virtual Item&  AddItem(Event<>  cb) = 0;
	virtual Item&  AddSubMenu(Event<Bar&> proc) = 0;
	virtual void   AddCtrl(Ctrl *ctrl, i32 gapsize) = 0;
	virtual void   AddCtrl(Ctrl *ctrl, Size sz) = 0;

	class ScanKeys;
	
	friend class MenuBar;

public:
	virtual bool   IsEmpty() const = 0;
	virtual void   Separator() = 0;

	static  Item&  NilItem();
	static bool    Scan(Event<Bar&> proc, dword key);

	void   Break();
	void   Gap(i32 size = 8);
	void   GapRight()                               { Gap(INT_MAX); }

	void   AddNC(Ctrl& ctrl);
	void   Add(Ctrl& ctrl)                          { AddCtrl(&ctrl, ctrl.GetMinSize()); }
	void   Add(Ctrl& ctrl, Size sz)                 { AddCtrl(&ctrl, sz); }
	void   Add(Ctrl& ctrl, i32 cx, i32 cy = 0)      { AddCtrl(&ctrl, Size(cx, cy)); }

	void   Add(bool en, Ctrl& ctrl)                     { Add(ctrl); ctrl.Enable(en); }
	void   Add(bool en, Ctrl& ctrl, Size sz)            { Add(ctrl, sz); ctrl.Enable(en); }
	void   Add(bool en, Ctrl& ctrl, i32 cx, i32 cy = 0) { Add(ctrl, cx, cy); ctrl.Enable(en); }

	Item&  Add(bool enable, tukk text, const DRX::Image& image, const Callback& callback);
	Item&  Add(bool enable, const Txt& text, const DRX::Image& image, const Callback& callback) { return Add(enable, ~text, image, callback); }
	Item&  Add(bool enable, KeyInfo& (*key)(), const DRX::Image& image, const Callback& callback);
	Item&  Add(tukk text, const DRX::Image& image, const Callback& callback);
	Item&  Add(const Txt& text, const DRX::Image& image, const Callback& callback) { return Add(~text, image, callback); }
	Item&  Add(KeyInfo& (*key)(), const DRX::Image& image, const Callback& callback);

	Item&  Add(bool enable, tukk text, const Callback& callback);
	Item&  Add(bool enable, const Txt& text, const Callback& callback) { return Add(enable, ~text, callback); }
	Item&  Add(bool enable, KeyInfo& (*key)(), const Callback& callback);
	Item&  Add(tukk text, const Callback& callback);
	Item&  Add(const Txt& text, const Callback& callback) { return Add(~text, callback); }
	Item&  Add(KeyInfo& (*key)(), const Callback& callback);

	Item&  Add(bool enable, tukk text, const DRX::Image& image, const Function<void ()>& fn);
	Item&  Add(bool enable, const Txt& text, const DRX::Image& image, const Function<void ()>& fn) { return Add(enable, ~text, image, fn); }
	Item&  Add(bool enable, KeyInfo& (*key)(), const DRX::Image& image, const Function<void ()>& fn);
	Item&  Add(tukk text, const DRX::Image& image, const Function<void ()>& fn);
	Item&  Add(const Txt& text, const DRX::Image& image, const Function<void ()>& fn) { return Add(~text, image, fn); }
//	Item&  Add(const Txt& text, const DRX::Image& image, const Function<void ()>& fn);
	Item&  Add(KeyInfo& (*key)(), const DRX::Image& image, const Function<void ()>& fn);

	Item&  Add(bool enable, tukk text, const Function<void ()>& fn);
	Item&  Add(bool enable, const Txt& text, const Function<void ()>& fn) { return Add(enable, ~text, fn); }
	Item&  Add(bool enable, KeyInfo& (*key)(), const Function<void ()>& fn);
	Item&  Add(tukk text, const Function<void ()>& fn);
	Item&  Add(const Txt& text, const Function<void ()>& fn) { return Add(~text, fn); }
	Item&  Add(KeyInfo& (*key)(), const Function<void ()>& fn);

	void   MenuSeparator();
	void   MenuBreak();
	void   MenuGap(i32 size = 8);
	void   MenuGapRight()                           { MenuGap(INT_MAX); }

	void   AddMenu(Ctrl& ctrl);
	void   AddMenu(Ctrl& ctrl, Size sz);
	void   AddMenu(Ctrl& ctrl, i32 cx, i32 cy = 0)  { AddMenu(ctrl, Size(cx, cy)); }

	Item&  AddMenu(bool enable, tukk text, const DRX::Image& image, const Callback& callback);
	Item&  AddMenu(bool enable, const Txt& text, const DRX::Image& image, const Callback& callback) { return AddMenu(enable, ~text, image, callback); }
	Item&  AddMenu(bool enable, KeyInfo& (*key)(), const DRX::Image& image, const Callback& callback);
	Item&  AddMenu(tukk text, const DRX::Image& image, const Callback& callback);
	Item&  AddMenu(const Txt& text, const DRX::Image& m, const Callback& c) { return AddMenu(~text, m, c); }
	Item&  AddMenu(KeyInfo& (*key)(), const DRX::Image& m, const Callback& c);

	Item&  AddMenu(bool enable, tukk text, const DRX::Image& image, const Function<void ()>& fn);
	Item&  AddMenu(bool enable, const Txt& text, const DRX::Image& image, const Function<void ()>& fn) { return AddMenu(enable, ~text, image, fn); }
	Item&  AddMenu(bool enable, KeyInfo& (*key)(), const DRX::Image& image, const Function<void ()>& fn);
	Item&  AddMenu(tukk text, const DRX::Image& image, const Function<void ()>& fn);
	Item&  AddMenu(const Txt& text, const DRX::Image& m, const Function<void ()>& fn) { return AddMenu(~text, m, fn); }
	Item&  AddMenu(KeyInfo& (*key)(), const DRX::Image& m, const Function<void ()>& fn);

	Item&  Add(bool enable, tukk text, const Callback1<Bar&>& proc);
	Item&  Add(bool enable, const Txt& text, const Callback1<Bar&>& proc) { return Add(enable, ~text, proc); }
	Item&  Add(tukk text, const Callback1<Bar&>& proc);
	Item&  Add(const Txt& text, const Callback1<Bar&>& proc) { return Add(~text, proc); }
	Item&  Add(bool enable, tukk text, const DRX::Image& image, const Callback1<Bar&>& proc);
	Item&  Add(bool enable, const Txt& text, const DRX::Image& image, const Callback1<Bar&>& proc) { return Add(enable, ~text, proc); }
	Item&  Add(tukk text, const DRX::Image& image, const Callback1<Bar&>& proc);
	Item&  Add(const Txt& text, const DRX::Image& image, const Callback1<Bar&>& proc) { return Add(~text, image, proc); }
	Item&  Sub(bool enable, tukk text, const Function<void (Bar&)>& submenu);
	Item&  Sub(bool enable, const Txt& text, const Function<void (Bar&)>& submenu) { return Sub(enable, ~text, submenu); }
	Item&  Sub(tukk text, const Function<void (Bar&)>& submenu);
	Item&  Sub(const Txt& text, const Function<void (Bar&)>& submenu) { return Sub(~text, submenu); }
	Item&  Sub(bool enable, tukk text, const DRX::Image& image, const Function<void (Bar&)>& submenu);
	Item&  Sub(bool enable, const Txt& text, const DRX::Image& image, const Function<void (Bar&)>& submenu) { return Sub(enable, ~text, image, submenu); }
	Item&  Sub(tukk text, const DRX::Image& image, const Function<void (Bar&)>& submenu);
	Item&  Sub(const Txt& text, const DRX::Image& image, const Function<void (Bar&)>& submenu) { return Sub(~text, image, submenu); }

	void   ToolSeparator();
	void   ToolBreak();
	void   ToolGap(i32 size = 8);
	void   ToolGapRight()                           { ToolGap(INT_MAX); }

	void   AddTool(Ctrl& ctrl);
	void   AddTool(Ctrl& ctrl, Size sz);
	void   AddTool(Ctrl& ctrl, i32 cx, i32 cy = 0)  { AddTool(ctrl, Size(cx, cy)); }

	Item&  Add(const DRX::Image& image, Event<>  callback);
	Item&  Add(bool enable, const DRX::Image& image, Event<>  callback);

	virtual void AddKey(dword key, Event<>  cb);
	        void AddKey(KeyInfo& (*key)(), Event<>  cb);

	virtual bool IsMenuBar() const                  { return false; }
	virtual bool IsToolBar() const                  { return false; }
	virtual bool IsScanKeys() const                 { return false; }
	virtual bool IsScanHelp() const                 { return false; }

	typedef Bar CLASSNAME;
	Bar();
	virtual ~Bar();
};

class BarCtrl : public Bar, public CtrlFrame {
public:
	virtual void   Layout();

	virtual void   FrameLayout(Rect& r);
	virtual void   FrameAddSize(Size& sz);
	virtual void   FrameAdd(Ctrl& ctrl);
	virtual void   FrameRemove();

	virtual bool   IsEmpty() const;
	virtual void   Separator();

protected:
	virtual void   AddCtrl(Ctrl *ctrl, i32 gapsize);
	virtual void   AddCtrl(Ctrl *ctrl, Size sz);

private:
	class SizeCtrl : public ParentCtrl {
	public:
		virtual Size GetMinSize() const;

	private:
		Size size;

	public:
		void  SetSize(Size sz)             { size = sz; }

		SizeCtrl()                         { NoWantFocus(); }
	};

	i32                  sii;
	Array<SeparatorCtrl> separator;
	i32                  zii;
	Array<SizeCtrl>      sizer;
	i32                  align;

protected:
	BarPane pane;
	i32     ssize;
	i32     wrap;
	i32     lsepm, rsepm;
	const SeparatorCtrl::Style *sepstyle;

	void     SyncBar();

	void     IClear();
	void     IFinish();

	void     Clear();

	virtual Val GetBackground() const;

	friend class BarPane;

public:
	Event<const Txt&> WhenHelp;
	Event<>  WhenLeftClick;

	static BarCtrl *GetBarCtrlParent(Ctrl *child);
	static void     SendHelpLine(Ctrl *q);
	static void     ClearHelpLine(Ctrl *q);

	enum {
		BAR_LEFT, BAR_RIGHT, BAR_TOP, BAR_BOTTOM
	};

	void  PaintBar(Draw& w, const SeparatorCtrl::Style& ss,
	               const Val& pane, const Val& iconbar = Null, i32 iconsz = 0);

	i32      GetHeight() const           { return pane.GetPaneSize(true, INT_MAX).cy; }

	BarCtrl& Align(i32 align);
	BarCtrl& Top()                       { return Align(BAR_TOP); }
	BarCtrl& Bottom()                    { return Align(BAR_BOTTOM); }
	BarCtrl& Left()                      { return Align(BAR_LEFT); }
	BarCtrl& Right()                     { return Align(BAR_RIGHT); }
	i32      GetAlign() const            { return align; }

	BarCtrl& Wrap(i32 q = 1)             { wrap = q; SyncBar(); return *this; }
	BarCtrl& NoWrap()                    { return Wrap(-1); }

	typedef BarCtrl CLASSNAME;

	BarCtrl();
	virtual ~BarCtrl();
};

class MenuItemBase;

CtrlFrame& MenuFrame();

class MenuBar : public BarCtrl {
public:
	virtual void  LeftDown(Point, dword);
	virtual bool  Key(dword key, i32 count);
	virtual bool  HotKey(dword key);
	virtual void  ChildGotFocus();
	virtual void  ChildLostFocus();
	virtual void  Deactivate();
	virtual void  CancelMode();
	virtual void  Paint(Draw& w);
	virtual bool  IsMenuBar() const                  { return true; }
	virtual bool  IsEmpty() const;
	virtual void  Separator();

protected:
	virtual Item& AddItem(Event<> cb);
	virtual Item& AddSubMenu(Event<Bar&> proc);
	virtual Val GetBackground() const;

public:
	struct Style : ChStyle<Style> {
		Val item; // hot menu item background in popup menu
		Val topitem[3]; // top menu item background normal/hot/pressed
		Val topbar; // deprecated
		Color menutext; // normal state popup menu item text
		Color itemtext; // hot state popup menu item text
		Color topitemtext[3]; // top menu item text normal/hot/pressed
		SeparatorCtrl::Style breaksep; // separator between menu bars
		Val look; // top menu background
		Val arealook; // top menu backgroung if arealook and in frame (can be null, then 'look')
		Val popupframe; // static frame of whole popup menu
		Val popupbody; // background of whole popup menu
		Val popupiconbar; // if there is special icon background in popup menu
		SeparatorCtrl::Style separator;
		Size  maxiconsize; // limit of icon size
		i32   leftgap; // between left border and icon
		i32   textgap;
		i32   lsepm;
		i32   rsepm;
		Point pullshift; // offset of submenu popup
		bool  opaquetest; // If true, topmenu item can change hot text color
		Val icheck; // background of Check (or Radio) item with image
	};

private:
	Array<MenuItemBase> item;

	MenuBar     *parentmenu;
	MenuBar     *submenu;
	Ctrl        *submenuitem;
	Ptr<Ctrl>    restorefocus;
	bool         doeffect;
	Font         font;
	i32          leftgap;
	i32          lock;
	const Style *style;
	i32          arealook;
	Size         maxiconsize;
	LookFrame    frame;
	bool         nodarkadjust;
	bool         action_taken = false; // local menu resulted in action invoked (not cancel)

#ifdef GUI_COCOA
	One<Bar>     host_bar;
	bool ExecuteHostBar(Ctrl *owner, Point p);
	void CreateHostBar(One<Bar>& bar);
#endif

	friend class MenuItemBase;
	friend class MenuItem;
	friend class SubMenuBase;
	friend class TopSubMenuItem;
	friend class SubMenuItem;

	void     SetParentMenu(MenuBar *parent)    { parentmenu = parent; style = parent->style; }
	MenuBar *GetParentMenu()                   { return parentmenu; }
	void     SetActiveSubmenu(MenuBar *sm, Ctrl *menuitem);
	MenuBar *GetActiveSubmenu()                { return submenu; }
	MenuBar *GetMasterMenu();
	MenuBar *GetLastSubmenu();
	void     DelayedClose();
	void     KillDelayedClose();
	void     SubmenuClose();
	void     PostDeactivate();
	void     SyncState();
	void     SetupRestoreFocus();
	void     PopUp(Ctrl *owner, Point p, Size rsz = Size(0, 0));

protected:
	enum {
		TIMEID_STOP = BarCtrl::TIMEID_COUNT,
		TIMEID_SUBMENUCLOSE,
		TIMEID_POST,
		TIMEID_COUNT
	};

public:
	Event<>  WhenSubMenuOpen;
	Event<>  WhenSubMenuClose;

	static i32 GetStdHeight(Font font = StdFont());

	void     CloseMenu();

	void     Set(Event<Bar&> menu);
	void     Post(Event<Bar&> bar);

	void     PopUp(Point p)                         { PopUp(GetActiveCtrl(), p); }
	void     PopUp()                                { PopUp(GetMousePos()); }

	bool     Execute(Ctrl *owner, Point p);
	bool     Execute(Point p)                       { return Execute(GetActiveCtrl(), p); }
	bool     Execute()                              { return Execute(GetMousePos()); }

	static bool Execute(Ctrl *owner, Event<Bar&> proc, Point p);
	static bool Execute(Event<Bar&> proc, Point p)  { return Execute(GetActiveCtrl(), proc, p); }
	static bool Execute(Event<Bar&> proc)           { return Execute(proc, GetMousePos()); }

	void     Clear();

	static const Style& StyleDefault();

	MenuBar& LeftGap(i32 cx)                        { leftgap = cx; return *this; }
	MenuBar& SetFont(Font f)                        { font = f; return *this; }
	MenuBar& SetStyle(const Style& s);
	Font     GetFont() const                        { return font; }
	MenuBar& AreaLook(i32 q = 1)                    { arealook = q; Refresh(); return *this; }
	MenuBar& MaxIconSize(Size sz)                   { maxiconsize = sz; return *this; }
	Size     GetMaxIconSize() const                 { return maxiconsize; }
	MenuBar& NoDarkAdjust(bool b = true)            { nodarkadjust = b; return *this; }
#ifdef GUI_COCOA
	MenuBar& UppMenu()                              { host_bar.Clear(); return *this; }
#endif

	typedef MenuBar CLASSNAME;

	MenuBar();
	virtual ~MenuBar();
};

class ToolButton : public Ctrl, public Bar::Item {
	using Ctrl::Key;

public:
	virtual void   Paint(Draw& w);
	virtual void   MouseEnter(Point, dword);
	virtual void   MouseLeave();
	virtual Size   GetMinSize() const;
	virtual void   LeftDown(Point, dword);
	virtual void   LeftRepeat(Point, dword);
	virtual void   LeftUp(Point, dword);
	virtual bool   HotKey(dword key);
	virtual Txt GetDesc() const;
	virtual i32    OverPaint() const;

	virtual Bar::Item& Text(tukk text);
	virtual Bar::Item& Key(dword key);
	virtual Bar::Item& Repeat(bool repeat = true);
	virtual Bar::Item& Image(const DRX::Image& img);
	virtual Bar::Item& Enable(bool _enable = true);
	virtual Bar::Item& Tip(tukk tip);
	virtual Bar::Item& Help(tukk help);
	virtual Bar::Item& Topic(tukk help);
	virtual Bar::Item& Description(tukk desc);
	virtual Bar::Item& Radio(bool check);
	virtual Bar::Item& Check(bool check);
	virtual void       FinalSync();

public:
	struct Style : ChStyle<Style> {
		Val  look[6];
		Font   font;
		Color  textcolor[6];
		bool   light[6];
		i32    contrast[6];
		Point  offset[6];
		i32    overpaint;
	};

protected:
	Txt  text;
	Txt  tiptext;
	dword   accel;
	bool    checked;
	bool    paint_checked;
	bool    repeat;

	u8    kind;
	Size    minsize;
	Size    maxiconsize;
	bool    nodarkadjust;

	const Style      *style;

private:
	DRX::Image img;

	void       SendHelpLine();
	void       ClearHelpLine();
	void       UpdateTip();

public:
	enum Kind { NOLABEL, RIGHTLABEL, BOTTOMLABEL };

	void  ResetKeepStyle();
	void  Reset();

	static const Style& StyleDefault();
	static const Style& StyleSolid();

	bool		IsChecked()				 { return checked; }
	DRX::Image  GetImage() const;

	ToolButton& SetStyle(const Style& s);
	ToolButton& MinSize(Size sz)         { minsize = sz; return *this; }
	ToolButton& MaxIconSize(Size sz)     { maxiconsize = sz; return *this; }
	ToolButton& Kind(i32 _kind);
	ToolButton& Label(tukk text, i32 kind);
	ToolButton& Label(tukk text);
	ToolButton& NoDarkAdjust(bool b = true) { nodarkadjust = b; return *this; }

	ToolButton();
	virtual ~ToolButton();
};

void PaintBarArea(Draw& w, Ctrl *x, const Val& look, i32 bottom = Null);

class ToolBar : public BarCtrl {
public:
	virtual bool HotKey(dword key);
	virtual void Paint(Draw& w);

protected:
	virtual Item& AddItem(Event<>  cb);
	virtual Item& AddSubMenu(Event<Bar&> proc);

public:
	struct Style : ChStyle<Style> {
		ToolButton::Style    buttonstyle;
		Size                 buttonminsize;
		Size                 maxiconsize;
		i32                  buttonkind;
		Val                look, arealook;
		SeparatorCtrl::Style breaksep;
		SeparatorCtrl::Style separator;
	};

private:
	i32               ii;
	Array<ToolButton> item;
	i32               lock;
	Event<Bar&>   proc;
	const Style      *style;
	i32               arealook;

	Size              buttonminsize;
	Size              maxiconsize;
	i32               kind;
	bool              nodarkadjust;

protected:
	enum {
		TIMEID_POST = BarCtrl::TIMEID_COUNT,
		TIMEID_COUNT
	};

public:
	virtual bool IsToolBar() const                  { return true; }

	static i32 GetStdHeight();

	void Clear();
	void Set(Event<Bar&> bar);
	void Post(Event<Bar&> bar);

	static const Style& StyleDefault();

	ToolBar& SetStyle(const Style& s)               { style = &s; Refresh(); return *this; }

	ToolBar& ButtonMinSize(Size sz)                 { buttonminsize = sz; return *this; }
	ToolBar& MaxIconSize(Size sz)                   { maxiconsize = sz; return *this; }
	ToolBar& ButtonKind(i32 _kind)                  { kind = _kind; return *this; }
	ToolBar& AreaLook(i32 q = 1)                    { arealook = q; Refresh(); return *this; }
	ToolBar& NoDarkAdjust(bool b = true)            { nodarkadjust = b; return *this; }

	typedef ToolBar  CLASSNAME;

	ToolBar();
	virtual ~ToolBar();
};

class StaticBarArea : public Ctrl {
public:
	virtual void Paint(Draw& w);

private:
	bool upperframe;

public:
	StaticBarArea& UpperFrame(bool b) { upperframe = b; Refresh(); return *this; }
	StaticBarArea& NoUpperFrame()     { return UpperFrame(false); }

	StaticBarArea();
};

class LRUList {
	Vec<Txt> lru;
	i32            limit;
	void           Select(Txt s, Event<const Txt&> WhenSelect);

public:
	static i32 GetStdHeight();

	void        Serialize(Stream& stream);

	void        operator()(Bar& bar, Event<const Txt&> WhenSelect, i32 count = INT_MAX, i32 from = 0);

	void        NewEntry(const Txt& path);
	void        RemoveEntry(const Txt& path);

	i32         GetCount() const                        { return lru.GetCount(); }

	LRUList&    Limit(i32 _limit)                       { limit = _limit; return *this; }
	i32         GetLimit() const                        { return limit; }

	typedef LRUList CLASSNAME;

	LRUList()   { limit = 6; }
};

class ToolTip : public Ctrl {
public:
	virtual void Paint(Draw& w);
	virtual Size GetMinSize() const;

private:
	Txt  text;

public:
	void   Set(tukk _text)        { text = _text; }
	Txt Get() const                   { return text; }

	void PopUp(Ctrl *owner, Point p, bool effect);

	ToolTip();
};

void PerformDescription();
