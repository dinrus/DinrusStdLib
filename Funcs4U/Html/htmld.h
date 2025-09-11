#ifndef __Html_htmld__
#define __Html_htmld__


namespace drx {

Txt      ToHtmlD(tukk s);

class HtmlTagD;

class HtmlsD : Moveable<HtmlsD> {
	Txt text;

public:
	bool   IsNullInstance() const { return text.IsEmpty(); }

	HtmlsD& Text(tukk s);
	HtmlsD& Quote(tukk text);

	HtmlsD& Color(class Color color);
	HtmlsD& Percent(double d);

	HtmlsD& Color(tukk name, class Color color);
	HtmlsD& Percent(tukk name, double d);

	HtmlsD& Attr(tukk attr);
	HtmlsD& Attr(tukk attr, tukk s);
	HtmlsD& Attr(tukk attr, Txt s);
	HtmlsD& Attr(tukk attr, i32 i);
	HtmlsD& Attr(tukk attr, double d);
	HtmlsD& Attr(tukk attr, Date date);
	HtmlsD& Attr(tukk attr, Val v);

	HtmlsD& Attp(tukk attr, double val_or_percent);
	HtmlsD& Atth(tukk attr, tukk href);

	HtmlsD& Tag(tukk tag);
	HtmlsD& Gat();

	HtmlsD& Cat(Txt s)              { text.Cat(s); return *this; }
	HtmlsD& Cat(tukk s)         { text.Cat(s); return *this; }
	HtmlsD& Cat(char c)                { text.Cat(c); return *this; }
	HtmlsD& Cat(tukk s, i32 n)  { text.Cat(s, n); return *this; }
	HtmlsD& Cat(const HtmlsD& s)       { return Cat(~s); }
	HtmlsD& Cat(const HtmlTagD& tag);

	HtmlsD& Nbsp();
	HtmlsD& Br();
	HtmlsD& Hr();
	HtmlsD& Lf();
	
	Txt operator~() const           { return text; }

	HtmlsD() {}
	HtmlsD(tukk s) : text(s) {}
	HtmlsD(const Txt& s) : text(s) {}
	HtmlsD(const Nuller&) {}
};

class HtmlTagD : Moveable<HtmlTagD> {
protected:
	HtmlsD  tag;
	Txt end;

public:
	HtmlTagD&      Text(tukk s)                        { tag.Text(s); return *this; }
	HtmlTagD&      Quote(tukk s)                       { tag.Quote(s); return *this; }

	HtmlTagD&      Percent(double d)                          { tag.Percent(d); return *this; }

	HtmlTagD&      Color(tukk name, class Color color) { tag.Color(name, color); return *this; }
	HtmlTagD&      Percent(tukk name, double d)        { tag.Percent(name, d); return *this; }

	HtmlTagD&      Attr(tukk attr)                     { tag.Attr(attr); return *this; }
	HtmlTagD&      Attr(tukk attr, tukk v)      { tag.Attr(attr, v); return *this; }
	HtmlTagD&      Attr(tukk attr, Txt v)           { tag.Attr(attr, v); return *this; }
	HtmlTagD&      Attr(tukk attr, i32 v)              { tag.Attr(attr, v); return *this; }
	HtmlTagD&      Attr(tukk attr, double v)           { tag.Attr(attr, v); return *this; }
	HtmlTagD&      Attr(tukk attr, Date v)             { tag.Attr(attr, v); return *this; }
	HtmlTagD&      Attr(tukk attr, const Val& v)     { tag.Attr(attr, v); return *this; }

	HtmlTagD&      Attp(tukk attr, double vp)          { tag.Attp(attr, vp); return *this; }
	HtmlTagD&      Atth(tukk attr, tukk href)   { tag.Atth(attr, href); return *this; }

	HtmlTagD&      Type(tukk s);

	HtmlTagD&      Name(tukk s);
	HtmlTagD&      Name(Txt id);

	HtmlTagD&      Data(tukk v);
	HtmlTagD&      Data(Txt v);
	HtmlTagD&      Data(double d);
	HtmlTagD&      Data(Val v);

	HtmlTagD&      ReadOnly();
	HtmlTagD&      Multiple();
	HtmlTagD&      Selected();

	HtmlTagD&      Align(Alignment align);
	HtmlTagD&      Left();
	HtmlTagD&      Right();
	HtmlTagD&      Center();

	HtmlTagD&      VAlign(Alignment align);
	HtmlTagD&      Top();
	HtmlTagD&      VCenter();
	HtmlTagD&      Bottom();

	HtmlTagD&      HotKey(char c);

	HtmlTagD&      Size(i32 c);
	HtmlTagD&      Size(i32 cx, i32 cy);

	HtmlTagD&      Color(class Color color);
	HtmlTagD&      BorderColor(class Color color);
	HtmlTagD&      BgColor(class Color color);

	HtmlTagD&      Face(tukk s);
	HtmlTagD&      Border(i32 i);
	HtmlTagD&      CellSpacing(i32 i);
	HtmlTagD&      CellPadding(i32 i);

	HtmlTagD&      Width(double d);
	HtmlTagD&      Height(double d);

	HtmlTagD&      RowSpan(i32 i);
	HtmlTagD&      ColSpan(i32 i);

	HtmlTagD&      Class(tukk s);
	HtmlTagD&      DataFld(tukk s);
	HtmlTagD&      DataSrc(tukk s);
	HtmlTagD&      Href(tukk s);
	HtmlTagD&      HtmlId(tukk s);
	HtmlTagD&      Lang(tukk s);
	HtmlTagD&      Language(tukk s);
	HtmlTagD&      JavaScript();
	HtmlTagD&      VbScript();
	HtmlTagD&      Methods(tukk s);
	HtmlTagD&      Rel(tukk s);
	HtmlTagD&      Rev(tukk s);
	HtmlTagD&      Style(tukk s);
	HtmlTagD&      TabIndex(i32 i);
	HtmlTagD&      Target(tukk s);
	HtmlTagD&      TargetBlank();
	HtmlTagD&      TargetParent();
	HtmlTagD&      TargetSelf();
	HtmlTagD&      TargetTop();
	HtmlTagD&      Title(tukk s);
	HtmlTagD&      Urn(tukk s);
	HtmlTagD&      Alt(tukk s);
	HtmlTagD&      Coords(tukk s);
	HtmlTagD&      NoHref();
	HtmlTagD&      Shape(tukk s);
	HtmlTagD&      ShapeCirc();
	HtmlTagD&      ShapePoly();
	HtmlTagD&      ShapeRect();
	HtmlTagD&      Balance(double v);
	HtmlTagD&      Loop(i32 i);
	HtmlTagD&      Volume(double v);
	HtmlTagD&      Src(tukk s);
	HtmlTagD&      Alink(class Color c);
	HtmlTagD&      Vlink(class Color c);
	HtmlTagD&      Link(class Color c);
	HtmlTagD&      Background(tukk s);

	HtmlTagD&      OnClick(tukk js);

	HtmlsD         GetTag() const;
	HtmlsD         GetEndTag() const;

	void          Combine(const HtmlTagD& tag);

	HtmlsD         ApplyTo(Txt s) const;
	HtmlsD         ApplyTo(tukk s) const;

	HtmlTagD&      PairTag(tukk s);
	HtmlTagD&      SingleTag(tukk s);

	HtmlTagD       operator()() const                         { return *this; }
	HtmlsD         operator~() const                          { return ApplyTo(Null); }
	operator       HtmlsD() const                             { return ApplyTo(Null); }
	Txt         ToTxt() const                           { return ~ApplyTo(Null); }

	const HtmlsD&  Tag() const                                { return tag; }
	HtmlsD&        Tag()                                      { return tag; }

	const Txt& End() const                                { return end; }
	Txt&       End()                                      { return end; }

	bool          IsEmpty() const                            { return IsNull(tag); }

	explicit HtmlTagD(tukk s);
	HtmlTagD() {}
};

HtmlTagD HtmlSingleTagD(tukk s);
HtmlTagD HtmlInputD(tukk type, tukk name = 0);
HtmlTagD HtmlEditD(Txt name);
HtmlTagD HtmlEditD(Txt name, i32 size, i32 maxlength = 256,
                 tukk defaultVal = 0, bool password = false);

HtmlTagD HtmlHiddenD(Txt name, Val val);
HtmlTagD HtmlHiddenD(tukk name, Val val);
HtmlTagD HtmlHiddenD(Val val);

HtmlTagD HtmlSubmitD(tukk text);

HtmlTagD HtmlSelectD(Txt name);
HtmlsD   HtmlOptionD(Val val, tukk text, bool selected = false);

HtmlTagD HtmlTableD();
HtmlTagD HtmlRowD();
HtmlTagD HtmlCellD();

HtmlTagD HtmlHeaderD(tukk title, Txt css = Null, tukk other = NULL);

//HtmlTagD HtmlFontD(Font font);

HtmlTagD HtmlLinkD(tukk link);

HtmlsD   HtmlBlockD(HtmlsD html, double width,
				              double left, double top = 0, double right = 0, double bottom = 0,
				              Alignment align = ALIGN_LEFT);

inline HtmlsD& operator<<(HtmlsD& s, const HtmlTagD& tag) { s.Cat(tag); return s; }
inline HtmlsD& operator<<(HtmlsD& s, const HtmlsD& v)     { s.Cat(v); return s; }
inline HtmlsD& operator<<(HtmlsD& s, Txt v)           { s.Cat(v); return s; }
inline HtmlsD& operator<<(HtmlsD& s, tukk v)      { s.Cat(v); return s; }

inline HtmlsD& operator+=(HtmlsD& s, const HtmlTagD& tag) { s.Cat(tag); return s; }

HtmlsD operator+(const HtmlTagD& tag1, const HtmlTagD& tag2);
HtmlsD operator+(const Txt& s, const HtmlTagD& tag);
HtmlsD operator+(const HtmlTagD& tag, const Txt& s);
HtmlsD operator+(tukk s, const HtmlTagD& tag);
HtmlsD operator+(const HtmlTagD& tag, tukk s);

HtmlsD operator+(const HtmlsD& s, const HtmlTagD& tag);
HtmlsD operator+(const HtmlTagD& tag, const HtmlsD& s);
HtmlsD operator+(const HtmlsD& a, tukk b);
HtmlsD operator+(tukk a, const HtmlsD& b);
HtmlsD operator+(const HtmlsD& a, const Txt& b);
HtmlsD operator+(const Txt& a, const HtmlsD& b);
HtmlsD operator+(const HtmlsD& a, const HtmlsD& b);

HtmlTagD operator/(const HtmlTagD& t1, const HtmlTagD& t2);
HtmlTagD& operator/=(HtmlTagD& tag, const HtmlTagD& s);

inline HtmlsD operator/(const HtmlTagD& tag, const HtmlsD& s)   { return tag.ApplyTo(~s); }
inline HtmlsD operator/(const HtmlTagD& tag, Txt s)          { return tag.ApplyTo(s); }
inline HtmlsD operator/(const HtmlTagD& tag, tukk s)     { return tag.ApplyTo(s); }

//////////////////////////////////////////////////////////////////////
// from htmlutil

HtmlTagD        operator % (const HtmlTagD& t1, const HtmlTagD& t2);
HtmlsD          operator % (const HtmlTagD& tag, tukk s);
inline HtmlsD   operator % (const HtmlTagD& tag, Txt s)     { return tag % ~s; }

//////////////////////////////////////////////////////////////////////

inline HtmlTagD HtmlBoldD()    { return HtmlTagD("B"); }
inline HtmlTagD HtmlItalicD()  { return HtmlTagD("I"); }
inline HtmlTagD HtmlBigD()     { return HtmlTagD("BIG"); }

//////////////////////////////////////////////////////////////////////

HtmlTagD        HtmlFontColorD(Color c);
HtmlTagD        HtmlFontSizeD(i32 s);

//////////////////////////////////////////////////////////////////////

inline HtmlsD   ToHtmlD(Txt s)   { return ToHtmlD(~s); }
HtmlTagD        HtmlLinkD(tukk link, tukk target);
HtmlTagD        HtmlImgD(Txt src, Txt alt = "");

HtmlsD          HtmlTextAreaD(Txt id, Size size, tukk defaultVal = 0);

HtmlTagD        HtmlButtonD(Txt id, tukk text);
HtmlTagD        HtmlWarnD(Color color = LtRed);
HtmlTagD        HtmlPackedTableD();
HtmlTagD        HtmlFormD(Txt action, bool multipart = false, Txt method = "POST");

HtmlsD          HtmlCheckD(Txt name, bool on);
HtmlsD          HtmlUploadD(Txt name);

HtmlTagD        HtmlTCellD();
HtmlTagD        HtmlRowCellD();
HtmlTagD        HtmlRowTCellD();

HtmlsD          HtmlMenuD(bool active, tukk text, tukk url, i32 wd = 0);
HtmlTagD        HtmlDialogD(i32 width = -100);
HtmlsD          HtmlHeadD(Txt title, tukk charset = "windows-1250");
HtmlsD          HtmlPageD(Txt title, const HtmlsD& body, Color bgcolor = White, tukk charset = "windows-1250");
HtmlsD          HtmlTitlePageD(Txt title, const HtmlsD& body, Color bgcolor = White, tukk charset = "windows-1250");

HtmlTagD HtmlLineD();
HtmlTagD HtmlParaD();
HtmlTagD HtmlCourierD(i32 size);
HtmlTagD HtmlArialD(i32 size);
HtmlTagD HtmlRomanD(i32 size);
HtmlTagD HtmlCourierD();
HtmlTagD HtmlArialD();
HtmlTagD HtmlRomanD();
HtmlTagD HtmlFntSizeD(i32 size);

}

#endif