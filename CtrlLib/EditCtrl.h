struct TextArrayOps {
	virtual z64 GetTotal() const = 0;
	virtual i32   GetCharAt(z64 i) const = 0;

	bool   GetWordSelection(z64 c, z64& sell, z64& selh);
	z64    GetNextWord(z64 c);
	z64    GetPrevWord(z64 c);

	virtual ~TextArrayOps() {}
};

class LookFrame : public CtrlFrame {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FramePaint(Draw& w, const Rect& r);
	virtual void FrameAddSize(Size& sz);

private:
	Val (*look)();
	Val lookv;
	Val Get() const { return look ? (*look)() : lookv; }

public:
	void  Set(const Val& v)                  { look = NULL; lookv = v; }
	void  Set(Val (*l)())                    { look = l; }
	LookFrame(Val (*look)()) : look(look)    {}
	LookFrame(const Val& v) : lookv(v) { look = NULL; }
	LookFrame() { look = NULL; }
};

Val EditFieldEdge();
Val ViewEdge();

void ViewEdge_Write(Val);

CtrlFrame& EditFieldFrame();
CtrlFrame& ViewFrame();

class EditField : public Ctrl, private TextArrayOps {
public:
	virtual void  Layout();
	virtual void  Paint(Draw& draw);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  MiddleDown(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword keyflags);
	virtual void  LeftDouble(Point p, dword keyflags);
	virtual void  LeftTriple(Point p, dword keyflags);
	virtual void  LeftDrag(Point p, dword flags);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  RightDown(Point p, dword keyflags);
	virtual void  MouseEnter(Point p, dword keyflags);
	virtual void  MouseLeave();
	virtual Image CursorImage(Point p, dword keyflags);
	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();
	virtual bool  Key(dword key, i32 rep);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual Size  GetMinSize() const;
	virtual void  SetData(const Val& data);
	virtual Val GetData() const;
	virtual void  CancelMode();
	virtual Txt GetSelectionData(const Txt& fmt) const;
	virtual void   State(i32);
	virtual Rect   GetCaret() const;

public:
	struct Style : ChStyle<Style> {
		Color paper;
		Color disabled;
		Color focus;
		Color invalid;
		Color text, textdisabled;
		Color selected, selectedtext;
		Color selected0, selectedtext0; // If does not have focus...
		Val edge[4]; // border (for various active edge states)
		Val coloredge; // border mask for adding color, e.g. round borders with red Error
		bool  activeedge;
		i32   vfm;
	};

	struct Highlight : Moveable<Highlight> {
		Color ink;
		Color paper;
		Color underline;

		bool operator!=(const Highlight& b) const { return ink != b.ink || paper != b.paper || underline != b.underline; }
	};

protected:
	enum {
		ATTR_TEXTCOLOR = Ctrl::ATTR_LAST,
		ATTR_INACTIVE_CONVERT,
		ATTR_CHARFILTER,
		ATTR_NULLICON,
		ATTR_NULLTEXT,
		ATTR_NULLINK,
		ATTR_NULLFONT,
		ATTR_BACKGROUND,
		ATTR_LAST,
	};

	ActiveEdgeFrame edge;

	WTxt    text;
	WTxt    undotext;
	Rect16     dropcaret;

	const Style    *style;
	CharFilter      filter;
	const Convert  *convert;

	Font            font;

	i32             sc;
	i32             cursor, anchor;

	i32             undocursor, undoanchor;

	i32             maxlen;
	i32             autosize;
	i32             fsell, fselh; // used to hold selection after LostFocus for X11 middle mouse copy

	i32             dropcursor;

	u8            charset;

	bool       selclick:1;

	bool       password:1;
	bool       autoformat:1;
	bool       initcaps:1;
	bool       keep_selection:1;
	bool       clickselect:1;
	bool       nobg:1;
	bool       alignright:1;
	bool       errorbg:1;
	bool       showspaces:1;
	bool       no_internal_margin:1;

	bool    FrameIsEdge();
	void    SetEdge(i32 i);
	void    RefreshAll();
	i32     LowChar(i32 c) const { return 0x25af /*c + 0x2400*/; }
	i32     GetCharWidth(i32 c) const { return font[c < 32 ? LowChar(c) : c]; }
	i32     GetTextCx(wtukk text, i32 n, bool password, Font fnt) const;
	void    Paints(Draw& w, i32& x, i32 fcy, wtukk &txt,
		           Color ink, Color paper, i32 n, bool pwd, Font fnt, Color underline, bool showspaces);
	i32     GetTxtCx(wtukk text, i32 n);
	i32     GetCaret(i32 cursor) const;
	i32     GetCursor(i32 posx);
	void    Finish(bool refresh = true);
	void    SaveUndo();
	void    DoAutoFormat();
	i32     GetTy() const;
	void    StdPasteFilter(WTxt&);
	void    SelSource();
	Color   GetPaper();
	i32     GetRightSpace() const;

protected:
	virtual void  HighlightText(Vec<Highlight>& hl);
	virtual z64 GetTotal() const             { return text.GetLength(); }
	virtual i32   GetCharAt(z64 pos) const   { return text[(i32)pos]; }

	// Spin support
	virtual void  PaintSpace(Draw& w);
	virtual i32   GetSpaceLeft() const;
	virtual i32   GetSpaceRight() const;
	virtual void  EditCapture();
	virtual bool  HasEditCapture();

public:
	Event<Bar&>               WhenBar;
	Event<>                   WhenEnter;
	Event<WTxt&>           WhenPasteFilter;
	Event<Vec<Highlight>&> WhenHighlight;
	Event<Color>              WhenPaper; // needed to improve visuals of DropChoice

	static const Style& StyleDefault();
	EditField&  SetStyle(const Style& s);

	static  i32   GetViewHeight(Font font = StdFont());
	static  i32   GetStdHeight(Font font = StdFont());

	i32     Insert(i32 pos, const WTxt& text);
	i32     Insert(i32 pos, const Txt& text)         { return Insert(pos, text.ToWTxt()); }
	i32     Insert(i32 pos, tukk text)           { return Insert(pos, WTxt(text)); }
	void    Remove(i32 pos, i32 n);

	void    Insert(const WTxt& text);
	void    Insert(const Txt& text)                  { return Insert(text.ToWTxt()); }
	void    Insert(tukk text)                    { return Insert(WTxt(text)); }
	void    Insert(i32 chr);

	void    Move(i32 newpos, bool select = false);

	void    SetSelection(i32 l = 0, i32 h = INT_MAX);
	bool    GetSelection(i32& l, i32& h) const;
	bool    IsSelection() const;
	bool    RemoveSelection();
	void    CancelSelection();
	void    Copy();
	void    Undo();
	void    Cut();
	void    Paste();
	void    Erase();
	void    SelectAll();

	void    StdBar(Bar& menu);

	void           SetText(const WTxt& text);
	void           SetText(const Txt& t)  { SetText(t.ToWTxt()); }
	void           SetText(tukk t)    { SetText(WTxt(t)); }
	const WTxt& GetText() const           { return text; }

	operator const WTxt&() const          { return text; } // Deprecated, use ~
	void operator=(const WTxt& s)         { SetText(s); } // Deprecated, use operator<<=
	i32     GetLength() const                { return text.GetLength(); }
	i32     GetChar(i32 i) const             { return text[i]; }

	Rect    GetCaretRect(i32 pos) const;
	Rect    GetCaretRect() const             { return GetCaretRect(cursor); }

	void    Clear();
	void    Reset();

	void    Error(bool error = true)         { if(errorbg != error) { errorbg = error; RefreshAll(); } }

	EditField& Password(bool pwd = true)     { password = pwd; Finish(); return *this; }
	bool       IsPassword() const            { return password; }
	EditField& SetFilter(wchar (*f)( wchar))      { filter = f; return *this; }
	EditField& SetConvert(const Convert& c)  { convert = &c; Refresh(); return *this; }
	EditField& SetInactiveConvert(const Convert& c) { SetVoidPtrAttr(ATTR_INACTIVE_CONVERT, &c); Refresh(); return *this; }
	EditField& AutoFormat(bool b = true)     { autoformat = b; return *this; }
	EditField& NoAutoFormat()                { return AutoFormat(false); }
	bool       IsAutoFormat() const          { return autoformat; }
	EditField& SetCharset(u8 cs)           { charset = cs; return *this; }
	EditField& SetFont(Font _font);
	EditField& SetColor(Color c);
	EditField& SetBackground(Color c);
	EditField& ClickSelect(bool b = true)    { clickselect = b; return *this; }
	bool       IsClickSelect() const         { return clickselect; }
	EditField& InitCaps(bool b = true)       { initcaps = b; return *this; }
	bool       IsInitCaps() const            { return initcaps; }
	EditField& NullText(const Image& icon, tukk text = t_("(дефолт)"), Color ink = Null);
	EditField& NullText(const Image& icon, tukk text, Font fnt, Color ink);
	EditField& NullText(tukk text = t_("(дефолт)"), Color ink = Null);
	EditField& NullText(tukk text, Font fnt, Color ink);
	EditField& MaxChars(i32 mc)              { maxlen = mc; return *this; }
	i32        GetMaxChars() const           { return maxlen; }
	EditField& AutoSize(i32 maxcx = INT_MAX) { autosize = maxcx; Finish(); return *this; }
	EditField& NoBackground(bool b = true)   { nobg = b; Transparent(); Refresh(); return *this; }
	EditField& AlignRight(bool b = true)     { alignright = b; Refresh(); return *this; }
	bool       IsNoBackground() const        { return nobg; }
	bool       IsAlignRight() const          { return alignright; }
	EditField& ShowSpaces(bool b = true)     { showspaces = b; Refresh(); return *this; }
	EditField& NoInternalMargin(bool b = true) { no_internal_margin = b; return *this; }

	CharFilter     GetFilter() const         { return filter; }
	const Convert& GetConvert() const        { return *convert; }
	Font           GetFont() const           { return font; }

	typedef EditField CLASSNAME;

	EditField();
	virtual ~EditField();
};

template <class DataType, class Cv>
class EditVal : public EditField, public Cv {
public:
	EditVal& operator=(const DataType& t)  { EditField::SetData(t); return *this; }  // Deprecated, use operator<<=
	operator DataType() const                { return EditField::GetData(); } // Deprecated, use ~

	EditVal()                              { SetConvert(*this); }
};

template <class DataType, class Cv>
class EditMinMax : public EditVal<DataType, Cv> {
public:
	EditMinMax& operator=(const DataType& t)          { EditField::SetData(t); return *this; }  // Deprecated, use operator<<=

	EditMinMax() {}
	EditMinMax(DataType min, DataType max)            { Cv::MinMax(min, max); }

	EditMinMax& Min(DataType min)                     { Cv::Min(min); Ctrl::Refresh(); return *this; }
	EditMinMax& Max(DataType max)                     { Cv::Max(max); Ctrl::Refresh(); return *this; }
	EditMinMax& MinMax(DataType min, DataType max)    { Min(min); return Max(max); }
	EditMinMax& NotNull(bool nn = true)               { Cv::NotNull(nn); Ctrl::Refresh(); return *this; }
};

template <class DataType, class Base>
class EditMinMaxNotNull : public Base {
public:
	EditMinMaxNotNull& operator=(const DataType& t)   { EditField::SetData(t); return *this; }  // Deprecated, use operator<<=

	EditMinMaxNotNull()                               { Base::NotNull(); }
	EditMinMaxNotNull(DataType min, DataType max)     { Base::NotNull(); Base::MinMax(min, max); }

	EditMinMaxNotNull& Min(DataType min)              { Base::Min(min); return *this; }
	EditMinMaxNotNull& Max(DataType max)              { Base::Max(max); return *this; }
	EditMinMaxNotNull& MinMax(DataType min, DataType max) { Base::MinMax(min, max); return *this; }
	EditMinMaxNotNull& NotNull(bool nn = true)        { Base::NotNull(nn); return *this; }
};

typedef EditMinMax<i32, ConvertInt>              EditInt;
typedef EditMinMax<z64, ConvertInt64>          EditInt64;
typedef EditMinMax<double, ConvertDouble>        EditDouble;
typedef EditMinMax<double, ConvertFloat>         EditFloat;
typedef EditMinMax<Date, ConvertDate>            EditDate;
typedef EditMinMax<Time, ConvertTime>            EditTime;
typedef EditMinMaxNotNull<i32, EditInt>          EditIntNotNull;
typedef EditMinMaxNotNull<z64, EditInt64>      EditInt64NotNull;
typedef EditMinMaxNotNull<double, EditFloat>     EditFloatNotNull;
typedef EditMinMaxNotNull<double, EditDouble>    EditDoubleNotNull;
typedef EditMinMaxNotNull<Date, EditDate>        EditDateNotNull;
typedef EditMinMaxNotNull<Time, EditTime>        EditTimeNotNull;

class EditTxt : public EditVal<WTxt, ConvertTxt> {
public:
	operator const WTxt&() const                  { return GetText(); }  // Deprecated, use ~

	EditTxt& operator=(const WTxt& data)       { SetData(data); return *this; } // Deprecated, use operator<<=
	EditTxt& operator=(const Txt& data)        { SetData(data); return *this; } // Deprecated, use operator<<=

	EditTxt() {}
	EditTxt(i32 maxlen)                           { MaxLen(maxlen); }

	EditTxt& MaxLen(i32 maxlen)                   { ConvertTxt::MaxLen(maxlen); Ctrl::Refresh(); return *this; }
	EditTxt& NotNull(bool nn = true)              { ConvertTxt::NotNull(nn); Ctrl::Refresh(); return *this; }
	EditTxt& TrimLeft(bool b = true)              { ConvertTxt::TrimLeft(b); Ctrl::Refresh(); return *this; }
	EditTxt& TrimRight(bool b = true)             { ConvertTxt::TrimRight(b); Ctrl::Refresh(); return *this; }
	EditTxt& TrimBoth(bool b = true)              { ConvertTxt::TrimBoth(b); Ctrl::Refresh(); return *this; }
};

class EditTxtNotNull : public EditTxt {
public:
	EditTxtNotNull& operator=(const WTxt& data) { SetData(data); return *this; } // Deprecated, use operator<<=
	EditTxtNotNull& operator=(const Txt& data)  { SetData(data); return *this; } // Deprecated, use operator<<=

	EditTxtNotNull()                               { NotNull(); }
	EditTxtNotNull(i32 maxlen)                     { NotNull(); MaxLen(maxlen); }
};

template <class IncType> IncType WithSpin_DefaultIncVal() { return 1; }
template <> inline       double  WithSpin_DefaultIncVal() { return 0.1; }

template <class IncType> IncType WithSpin_DefaultStartVal() { return 0; }
template <> inline       Date    WithSpin_DefaultStartVal() { return GetSysDate(); }
template <> inline       Time    WithSpin_DefaultStartVal() { return GetSysTime(); }

template <class DataType, class IncType>
void WithSpin_Add(DataType& value, IncType inc, DataType min, bool roundfrommin)
{
	value += inc;
}

template <> inline
void WithSpin_Add(i32& value, i32 inc, i32 min, bool roundfrommin) {
	if(roundfrommin)
		value -= min;
	if(inc < 0) {
		inc = -inc;
		value = (value - inc) / inc * inc;
	}
	else
		value = (value + inc) / inc * inc;
	if(roundfrommin)
		value += min;
}

template <> inline
void WithSpin_Add(double& value, double inc, double min, bool roundfrommin) {
	if(roundfrommin)
		value -= min;
	if(inc < 0) {
		inc = -inc;
		value = (ceil(value / inc - inc / 100) - 1) * inc;
	}
	else
		value = (floor(value / inc + inc / 100) + 1) * inc;
	if(roundfrommin)
		value += min;
}

template <class DataType, class Base, class IncType = DataType>
class WithSpin : public Base, private VirtualButtons {
public:
	virtual void  CancelMode();
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual bool  Key(dword key, i32 repcnt);
	virtual Image MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags);

	virtual i32   GetSpaceLeft() const;
	virtual i32   GetSpaceRight() const;
	virtual void  PaintSpace(Draw& w);
	virtual void  EditCapture();
	virtual bool  HasEditCapture();

	virtual i32   ButtonCount() const;
	virtual Rect  ButtonRect(i32 i) const;
	virtual const Button::Style& ButtonStyle(i32 i) const;
	virtual void  ButtonPush(i32 i);
	virtual void  ButtonRepeat(i32 i);

protected:
	void            Inc();
	void            Dec();
	void            Init();

private:
	const SpinButtons::Style *style;
	IncType         inc;
	bool            roundfrommin = false;
	bool            visible = true;
	bool            mousewheel = true;
	bool            keys = true;

	typedef WithSpin CLASSNAME;
public:

	WithSpin&          SetInc(IncType _inc = 1)     { inc = _inc; return *this; }
	DataType           GetInc() const               { return inc; }

	WithSpin&          SetSpinStyle(const SpinButtons::Style& s) { style = &s; return *this; }

	WithSpin&          OnSides(bool b = true);
	bool               IsOnSides() const            { return style->onsides; }

	WithSpin&          ShowSpin(bool b = true)      { visible = b; Base::RefreshLayout(); return *this; }
	bool               IsSpinVisible() const        { return visible; }

	WithSpin&          RoundFromMin(bool b = true)  { roundfrommin = b; return *this; }

	WithSpin&          MouseWheelSpin(bool b = true){ mousewheel = b; return *this; }
	WithSpin&          NoMouseWheelSpin()           { return MouseWheelSpin(false); }

	WithSpin&          KeySpin(bool b = true)       { keys = b; return *this; }
	WithSpin&          NoKeySpin()                  { return KeySpin(false); }

	WithSpin();
	WithSpin(IncType inc); // deprecated
	WithSpin(DataType min, DataType max, IncType inc); // deprecated
	virtual ~WithSpin() {}
};

#include "EditCtrl.hpp"

typedef WithSpin<i32, EditInt>               EditIntSpin;
typedef WithSpin<z64, EditInt64>           EditInt64Spin;
typedef WithSpin<double, EditDouble>         EditDoubleSpin;
typedef WithSpin<double, EditFloat>          EditFloatSpin;
typedef WithSpin<Date, EditDate, i32>        EditDateSpin;
typedef WithSpin<Time, EditTime, i32>        EditTimeSpin;

typedef WithSpin<i32, EditIntNotNull>        EditIntNotNullSpin;
typedef WithSpin<z64, EditInt64NotNull>    EditInt64NotNullSpin;
typedef WithSpin<double, EditDoubleNotNull>  EditDoubleNotNullSpin;
typedef WithSpin<double, EditFloatNotNull>   EditFloatNotNullSpin;
typedef WithSpin<Date, EditDateNotNull, i32> EditDateNotNullSpin;
typedef WithSpin<Time, EditTimeNotNull, i32> EditTimeNotNullSpin;