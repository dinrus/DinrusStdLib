#ifndef __CtrlLib_DlgColor__
#define __CtrlLib_DlgColor__

class WheelRampCtrl : public Ctrl
{
public:
	WheelRampCtrl(bool ramp);
	~WheelRampCtrl();

	virtual void  Layout();
	virtual void  Paint(Draw& draw);
	virtual void  SetData(const Val& value);
	virtual Val GetData() const                { return color; }

	virtual void  LeftDown(Point pt, dword keyflags);
	virtual void  LeftUp(Point pt, dword keyflags);
	virtual void  LeftDouble(Point pt, dword keyflags);
	virtual void  MouseMove(Point pt, dword keyflags);

	Event<>       WhenLeftDouble;

private:
	void          SetColor(Color color, bool set_norm, bool set_hsv);

	Image         PaintRamp(Size size);
	Image         PaintWheel(Size size);
	void          PaintColumn(Draw& draw);

	i32           ClientToLevel(i32 y) const;
	i32           LevelToClient(i32 l) const;

private:
	bool          ramp;
	Color         color;
	Color         normalized_color;
	i32           h16, s16, v16;
	i32           round_step;
	enum STYLE { S_WHEEL, S_RECT, S_HEXAGON };
	STYLE         style;
	Image         cache;
	i32           cache_level;
	Image         wheel_cache;
	Rect          wheel_rect;
	Rect          column_rect;

	i32           firstclick;

	struct WheelBuff
	{
		i32 arg;
		i32 l;
	};
};

struct ColorWheelCtrl : public WheelRampCtrl {
	ColorWheelCtrl() : WheelRampCtrl(false) {}
};

struct ColorRampCtrl : public WheelRampCtrl {
	ColorRampCtrl() : WheelRampCtrl(true) {}
};

class ColorSelector : public Ctrl
{
public:
	ColorSelector(bool not_null = true);

	ColorSelector& NotNull(bool nn = true)                  { impl->NotNull(nn); return *this; }
	ColorSelector& NoNotNull()                              { return NotNull(false); }
	bool           IsNotNull() const                        { return impl->IsNotNull(); }

	Color          Get() const                              { return impl->Get(); }
	void           Set(Color c)                             { impl->Set(c); }

	Vec<Color>  GetPalette() const                       { return impl->GetPalette(); }
	void           SetPalette(const Vec<Color>& palette) { impl->SetPalette(palette); }

	void           SerializeConfig(Stream& stream)          { impl->SerializeConfig(stream); }

	virtual void   SetData(const Val& color)              { Set(color); }
	virtual Val  GetData() const                          { return Get(); }

	Ctrl&          GetImplCtrl()                            { return impl->GetCtrl(); }

public:
	Event<>        WhenSetColor;

public:
	class Impl
	{
	public:
		virtual ~Impl() {}

		virtual void          Set(Color c) = 0;
		virtual Color         Get() const = 0;
		virtual void          NotNull(bool nn) = 0;
		virtual bool          IsNotNull() const = 0;
		virtual Vec<Color> GetPalette() const = 0;
		virtual void          SetPalette(const Vec<Color>& pal) = 0;
		virtual void          SerializeConfig(Stream& stream) = 0;
		virtual Ctrl&         GetCtrl() = 0;

	protected:
		Impl() {}
	};

protected:
	One<Impl>      impl;
};

class ColorCtrl : public DataPusher
{
public:
	typedef ColorCtrl CLASSNAME;
	ColorCtrl(bool not_null = true);
	virtual ~ColorCtrl();

	ColorCtrl&     NotNull(bool _nn = true) { empty.Show(!_nn); return *this; }
	ColorCtrl&     NoNotNull()              { return NotNull(false); }
	bool           IsNotNull() const        { return !empty.IsVisible(); }

protected:
	virtual void   DoAction();
	void           OnClear()                { SetDataAction(Null); }

protected:
	FrameRight<Button> empty;
};

Color RunDlgSelectColor(Color init_color = Black, bool not_null = true, tukk title = 0, bool *ok = 0);
const Display& StdColorDisplayNull();

class ColorPopUp : public Ctrl {
public:
	virtual  void Paint(Draw& w);
	virtual  void LeftUp(Point p, dword);
	virtual  void LeftDown(Point p, dword);
	virtual  void MouseMove(Point p, dword);
	virtual  void MouseLeave();
	virtual  bool Key(dword key, i32 count);
	virtual  void Layout();

private:
	void PopupDeactivate();

	struct Popup : Ctrl {
		ColorPopUp *color;
		
		virtual void Deactivate() { color->PopupDeactivate(); }
	};

	i32      Get(Point p);
	i32      GetCy();
	void     Setup(Color c);
	void     Finish();
	void     Ramp();
	void     Wheel();
	i32      GetColorCount() const;
	Color    GetColor(i32 i) const;
	void     Select();

	void     DrawFilledFrame(Draw &w, i32 x, i32 y, i32 cx, i32 cy, Color fcol, Color bcol);
	void     DrawFilledFrame(Draw &w, Rect &r, Color fcol, Color bcol);

	i32      colori;
	bool     notnull;
	bool     scolors;
	bool     norampwheel;
	bool     animating;
	bool     hints;
	bool     open;
	bool     withvoid;
	Txt   nulltext;
	Txt   voidtext;
	Color    color;

	ColorRampCtrl  ramp;
	ColorWheelCtrl wheel;
	Button         settext;
	One<Popup>     popup;

	static Color   hint[18];
	
	friend void ColorPopUp_InitHint();

public:
	Event<>  WhenCancel;
	Event<>  WhenSelect;

	static void Hint(Color c);

	typedef ColorPopUp CLASSNAME;

	void     PopUp(Ctrl *owner, Color c = White);
	Color    Get() const;
	
	ColorPopUp& NotNull(bool b = true)               { notnull = b; return *this; }
	ColorPopUp& SColors(bool b = true)               { scolors = b; return *this; }//Deprecated, BUT NEEDED IN DinrusIDE
	ColorPopUp& NullText(tukk s)              { nulltext = s; Refresh(); return *this; }
	ColorPopUp& WithVoid(bool b = true)              { withvoid = b; Refresh(); return *this; }
	ColorPopUp& VoidText(tukk s)              { voidtext = s; Refresh(); return *this; }
	ColorPopUp& NoRampWheel(bool b = true)           { norampwheel = b; return *this; }
	ColorPopUp& Hints(bool b = true)                 { hints = b; return *this; }

	ColorPopUp();
	virtual ~ColorPopUp();
};

class ColorPusher : public Ctrl {
public:
	virtual void  Paint(Draw& w);
	virtual void  LeftDown(Point p, dword);
	virtual bool  Key(dword key, i32);
	virtual void  GotFocus()                  { Refresh(); }
	virtual void  LostFocus()                 { Refresh(); }
	virtual void  SetData(const Val& v);
	virtual Val GetData() const;

protected:
	bool       push;
	bool       withtext;
	bool       withhex;
	bool       track;
	Color      color, saved_color;
	ColorPopUp colors;
	Txt     nulltext;
	Txt     voidtext;

	void AcceptColors();
	void CloseColors();
	void NewColor();
	void Drop();

public:
	typedef ColorPusher CLASSNAME;

	ColorPusher& NullText(tukk s)    { nulltext = s; colors.NullText(s); Refresh(); return *this; }
	ColorPusher& NotNull(bool b = true)     { colors.NotNull(b); return *this; }
	ColorPusher& WithVoid(bool b = true)    { colors.WithVoid(b); return *this; }
	ColorPusher& VoidText(tukk s)    { voidtext = s; colors.VoidText(s); Refresh(); return *this; }
	ColorPusher& SColors(bool b = true)     { colors.SColors(b); return *this; }
	ColorPusher& WithText(bool b = true)    { withtext = b; Refresh(); return *this; }
	ColorPusher& WithHex(bool b = true)     { withhex = b; Refresh(); return *this; }
	ColorPusher& Track(bool b = true)       { track = b; return *this; }
	ColorPusher& NoTrack()                  { return Track(false); }
	ColorPusher& NoRampWheel(bool b = true) { colors.NoRampWheel(b); return *this; }

	ColorPusher();
	virtual ~ColorPusher();
};

class ColorButton : public ColorPusher {
public:
	virtual void  Paint(Draw& w);
	virtual void  MouseEnter(Point p, dword keyflags);
	virtual void  MouseLeave();
	virtual Size  GetMinSize() const;

protected:
	Image      image, nullimage, staticimage;
	const ToolButton::Style *style;

public:
	ColorButton& ColorImage(const Image& img)             { image = img; Refresh(); return *this; }
	ColorButton& NullImage(const Image& img)              { nullimage = img; Refresh(); return *this; }
	ColorButton& StaticImage(const Image& img)            { staticimage = img; Refresh(); return *this; }
	ColorButton& SetStyle(const ToolButton::Style& s)     { style = &s; Refresh(); return *this; }

	ColorButton();
	virtual ~ColorButton();
};

Txt FormatColor(Color c);
Color  ReadColor(CParser& p);
Color  RealizeColor(Color c);

#endif//__TCtrlLib_DlgColor__
