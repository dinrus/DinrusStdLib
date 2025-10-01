class Pusher : public Ctrl {
public:
	virtual void   CancelMode();
	virtual void   LeftDown(Point, dword);
	virtual void   MouseMove(Point, dword);
	virtual void   MouseLeave();
	virtual void   LeftRepeat(Point, dword);
	virtual void   LeftUp(Point, dword);
	virtual void   GotFocus();
	virtual void   LostFocus();
	virtual void   State(i32);
	virtual Txt GetDesc() const;
	virtual bool   Key(dword key, i32);
	virtual bool   HotKey(dword key);
	virtual dword  GetAccessKeys() const;
	virtual void   AssignAccessKeys(dword used);

private:
	bool    push:1;
	bool    keypush:1;
	bool    clickfocus:1;

	void    EndPush();

protected:
	i32     accesskey;
	Txt  label;
	Font    font;

	void    KeyPush();
	bool    IsPush() const                                  { return push || keypush; }
	bool    IsKeyPush()                                     { return keypush; }
	bool    FinishPush();

protected:
	virtual void  RefreshPush();
	virtual void  RefreshFocus();
	virtual void  PerformAction();

public:
	Pusher&  SetFont(Font fnt);
	Pusher&  SetLabel(tukk text);
	Pusher&  ClickFocus(bool cf = true);
	Pusher&  NoClickFocus()                                 { return ClickFocus(false); }
	bool     IsClickFocus() const                           { return clickfocus; }
	
	Font     GetFont() const                                { return font; }
	Txt   GetLabel() const                               { return label; }

	void     PseudoPush();

	i32      GetVisualState() const;

	Event<>  WhenPush;
	Event<>  WhenRepeat;

	Pusher();
	virtual ~Pusher();
};

class Button : public Pusher {
public:
	virtual void   Paint(Draw& draw);
	virtual bool   Key(dword key, i32);
	virtual bool   HotKey(dword key);
	virtual void   MouseEnter(Point, dword);
	virtual void   MouseLeave();
	virtual dword  GetAccessKeys() const;
	virtual void   AssignAccessKeys(dword used);
	virtual void   Layout();
	virtual void   GotFocus();
	virtual void   LostFocus();
	virtual i32    OverPaint() const;

public:
	struct Style : ChStyle<Style> {
		Val look[4];
		Color monocolor[4], textcolor[4];
		Point pressoffset;
		i32   focusmargin;
		i32   overpaint;
		Font  font;
		Image ok, cancel, exit;
		bool  transparent;
		bool  focus_use_ok;
	};

protected:
	enum { NORMAL, OK, CANCEL, EXIT };
	const Style *style;
	Image   img;
	bool    monoimg;
	u8    type;

	void  RefreshOK(Ctrl *p);
	const Style *St() const;

public:
	Button&  SetImage(const Image& img);
	Button&  SetMonoImage(const Image& img);

	static const Style& StyleNormal();
	static const Style& StyleOk();
	static const Style& StyleEdge();
	static const Style& StyleLeftEdge();
	static const Style& StyleScroll();
	static const Style& StyleNaked();

	static void  PaintButton(Draw& w, const Rect& r, const Button::Style& st, i32 visualstate, bool focus,
                             const Txt& label, Font font, const Image& img,
                             bool monoimg, i32 accesskey, bool visibaleaccesskeys, bool disabled);

	Button&  SetStyle(const Style& s);
	Button&  AutoStyle();

	Button&  NormalStyle()                        { return SetStyle(StyleNormal()); }
	Button&  EdgeStyle()                          { return SetStyle(StyleEdge()); }
	Button&  LeftEdgeStyle()                      { return SetStyle(StyleLeftEdge()); }
	Button&  ScrollStyle()                        { return SetStyle(StyleScroll()); }
	Button&  NakedStyle()                         { return SetStyle(StyleNaked()); }

	Button&  Ok();
	Button&  Cancel();
	Button&  Exit();
	Button&  Normal()                             { type = NORMAL; Refresh(); return *this; }

	Button();
	virtual ~Button();
};

Color ButtonMonoColor(i32 i);

class Option : public Pusher {
public:
	virtual void   Paint(Draw& draw);
	virtual Size   GetMinSize() const;
	virtual void   SetData(const Val& data);
	virtual Val  GetData() const;
	virtual void   MouseEnter(Point, dword);
	virtual void   MouseLeave();
	virtual void   State(i32);

protected:
	virtual void  RefreshPush();
	virtual void  RefreshFocus();
	virtual void  PerformAction();

protected:
	Image  edge, edged;
	i32    option;
	bool   switchimage:1;
	bool   threestate:1;
	bool   notnull:1;
	bool   blackedge:1;
	bool   showlabel:1;
	bool   box:1;
	bool   autobox:1;
	Color  color;
	
	void   AutoSync();

public:
	Option& Set(i32 b);
	i32     Get() const                           { return option; }

	operator i32() const                          { return option; }
	void operator=(i32 b)                         { Set(b); }
	
	void    EnableBox(bool b);
	void    EnableBox()                           { EnableBox(option); }

	Option& BlackEdge(bool b = true)              { blackedge = b; Refresh(); return *this; }
	bool    IsBlackEdge() const                   { return blackedge; }
	Option& SwitchImage(bool b = true)            { switchimage = b; Refresh(); return *this; }
	bool    IsSwitchImage() const                 { return switchimage; }
	Option& ThreeState(bool b = true)             { threestate = b; notnull = false; return *this; }
	bool    IsThreeState() const                  { return threestate; }
	Option& ShowLabel(bool b = true)              { showlabel = b; Refresh(); return *this; }
	bool    IsShowLabel() const                   { return showlabel; }
	Option& NotNull(bool nn = true)               { notnull = nn; Refresh(); return *this; }
	Option& NoNotNull()                           { return NotNull(false); }
	bool    IsNotNull() const                     { return notnull; }
	Option& SetColor(Color c)                     { color = c; Refresh(); return *this; }
	Option& Box(bool b = true)                    { box = b; return *this; }
	Option& AutoBox(bool b = true)                { Box(autobox = b); return *this; }

	Option();
	virtual ~Option();
};

class OptionBox : public Option {
public:
	OptionBox() { Box(); }
};

class ButtonOption : public Ctrl {
public:
	virtual void  Paint(Draw& w);
	virtual void  LeftDown(Point, dword);
	virtual void  LeftUp(Point, dword);
	virtual void  MouseMove(Point, dword);
	virtual void  MouseEnter(Point, dword);
	virtual void  MouseLeave();
	virtual void  SetData(const Val& v);
	virtual Val GetData() const;
	virtual void  Serialize(Stream& s);
	virtual dword GetAccessKeys() const;
	virtual void  AssignAccessKeys(dword used);

public:
	struct Style : ChStyle<Style> {
		Val look[4];
		Color textcolor[4];
		bool  drawfocus;
	};

private:
	Image        image;
	Image        image1;
	Txt       label;
	const Style *style;
	i32          accesskey;
	bool         option;
	bool         push;

public:
	ButtonOption&  SetImage(const Image& img)                 { image = img; Refresh(); return *this; }
	ButtonOption&  SetImage(const Image& m, const Image& m1)  { image = m; image1 = m1; Refresh(); return *this; }
	void operator=(const Image& img)                          { SetImage(img); }
	
	ButtonOption& SetLabel(const Txt& text);
	Txt GetLabel() const                                   { return label; }
	
	void Set(bool b)                                          { option = b; UpdateRefresh(); }
	bool Get() const                                          { return option; }

	void operator=(bool b)                                    { Set(b); }
	operator bool() const                                     { return Get(); }

	static const Style& StyleDefault();
	static const Style& StyleFlat();
	
	ButtonOption& SetStyle(const Style& s)                    { style = &s; Refresh(); return *this; }
	ButtonOption& AutoStyle()                                 { style = NULL; Refresh(); return *this; }

	ButtonOption();
};

class Switch : public Ctrl {
public:
	virtual void   Paint(Draw& draw);
	virtual void   CancelMode();
	virtual void   MouseMove(Point p, dword keyflags);
	virtual void   LeftDown(Point p, dword keyflags);
	virtual void   LeftUp(Point p, dword keyflags);
	virtual void   MouseLeave();
	virtual bool   Key(dword key, i32 count);
	virtual bool   HotKey(dword key);
	virtual dword  GetAccessKeys() const;
	virtual void   AssignAccessKeys(dword used);
	virtual void   SetData(const Val& data);
	virtual Val  GetData() const;
	virtual void   GotFocus();
	virtual void   LostFocus();

public:
	struct Case  {
		Txt label;
		Val  value;
		Rect16 rect = Rect16(0, 0, 0, 0);
		i32    accesskey = 0;
		i32    gap = 0;
		bool   enabled = true;
	};

private:
	Font         font;
	Val        value;
	i32          pushindex;
	Array<Case>  cs;
	i32          linecy;
	i32          light;
	i32          mincy;
	i32          direction;

	i32   GetIndex() const;
	i32   GetIndex(Point p);
	bool  DoHot(dword key);
	void  Updates();

public:
	enum { GAP_SEPARATOR = 1 << 20 };

	Switch& SetLabel(i32 i, tukk text, i32 gap = 0);
	Switch& SetLabel(tukk text);
	Txt  GetLabel() const                                    { return GetLabel(GetIndex()); }
	Txt  GetLabel(i32 i) const                               { return cs[i].label; }
	Switch& Set(i32 i, const Val& val, tukk text, i32 gap = 0);
	Switch& Set(i32 i, const Val& val);
	Switch& Add(const Val& val, tukk text, i32 gap = 0);
	Switch& Add(tukk text, i32 gap = 0);

	void    EnableCase(i32 i, bool enable = true);
	void    DisableCase(i32 i)                                  { EnableCase(i, false); }

	void    EnableVal(const Val& val, bool enable = true);
	void    DisableVal(const Val& val)                      { EnableVal(val, false); }

	void  Reset()                                               { cs.Clear(); }

	const Array<Case>& GetCases() const                         { return cs; }

	operator i32() const                                        { return GetData(); }
	void operator=(const Val& v)                              { SetData(v); }

	Switch& SetFont(Font f)                                     { font = f; Refresh(); return *this; }
	Font    GetFont() const                                     { return font; }
	Switch& MinCaseHeight(i32 cy)                               { mincy = cy; Refresh(); return *this; }
	Switch& SetAutoDirection()                                  { direction = 0; return *this; }
	Switch& SetHorizontal()                                     { direction = 1; return *this; }
	Switch& SetVertical()                                       { direction = -1; return *this; }

	Switch();
	virtual ~Switch();
};

class DataPusher : public Pusher
{
public:
	virtual void   Paint(Draw& draw);
	virtual Val  GetData() const;
	virtual void   SetData(const Val& value);

private:
	const Convert *convert;
	const Display *display;
	Val          data;

	WTxt        nulltext;
	Color          nullink;
	Font           nullfont;
	
	ActiveEdgeFrame edge;
	
	void  RefreshAll();
	Color GetPaper();
	
protected:
	virtual void   RefreshPush();
	virtual void   RefreshFocus();
	virtual void   PerformAction();
	virtual void   DoAction();

public:
	Event<>        WhenPreAction;

	DataPusher&    SetConvert(const Convert& _convert) { convert = &_convert; Refresh(); return *this; }
	const Convert& GetConvert() const                  { return *convert; }

	DataPusher&    SetDisplay(const Display& _display) { display = &_display; Refresh(); return *this; }
	const Display& GetDisplay() const                  { return *display; }

	void           SetDataAction(const Val& value);

	void           Set(const Val& value);

	DataPusher&    NullText(tukk text = t_("(default)"), Color ink = Brown);
	DataPusher&    NullText(tukk text, Font fnt, Color ink);

	DataPusher();
	DataPusher(const Convert& convert, const Display& display = StdDisplay()); // deprecated
	DataPusher(const Display& display); // deprecated
};

class SpinButtons : public CtrlFrame {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FrameAddSize(Size& sz);
	virtual void FrameAdd(Ctrl& ctrl);
	virtual void FrameRemove();

public:
	struct Style : ChStyle<Style> {
		Button::Style inc;
		Button::Style dec;
		i32           width;
		i32           over;
		bool          onsides;
	};

private:
	bool         visible;
	const Style *style;

public:
	Button inc;
	Button dec;

	void         Show(bool s = true);
	bool         IsVisible() const          { return visible; }

	static const Style& StyleDefault();
	static const Style& StyleOnSides();

	SpinButtons& SetStyle(const Style& s);
	
	SpinButtons& OnSides(bool b = true)     { return SetStyle(b ? StyleOnSides() : StyleDefault()); }
	bool         IsOnSides() const          { return style->onsides; }

	SpinButtons();
	virtual ~SpinButtons();
};

struct VirtualButtons {
	virtual i32                  ButtonCount() const;
	virtual Rect                 ButtonRect(i32 i) const;
	virtual const Button::Style& ButtonStyle(i32 i) const;
	virtual Image                ButtonImage(i32 i) const;
	virtual bool                 ButtonMono(i32 i) const;
	virtual bool                 ButtonEnabled(i32 i) const;

	virtual void ButtonPush(i32 i);
	virtual void ButtonRepeat(i32 i);
	virtual void ButtonAction(i32 i);

	i8    pushi = -1;
	i8    mi = -1;
	bool    buttons_capture = false;

	i32    FindButton(Point p) const;
	
	void   EndPush(Ctrl *ctrl);

	void   ButtonsCancelMode();
	bool   ButtonsMouseEvent(Ctrl *ctrl, i32 event, Point p);
	void   PaintButtons(Draw& w, Ctrl *ctrl);

	i32    ButtonVisualState(Ctrl *ctrl, i32 i);
	void   RefreshButton(Ctrl *ctrl, i32 i);
};
