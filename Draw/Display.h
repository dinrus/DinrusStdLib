class Display {
public:
	enum {
		CURSOR   = 0x01,
		FOCUS    = 0x02,
		SELECT   = 0x04,
		READONLY = 0x08,
	};

	virtual void PaintBackground(Draw& w, const Rect& r, const Val& q,
	                             Color ink, Color paper, dword style) const;
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
		               Color ink, Color paper, dword style) const;
	virtual Size GetStdSize(const Val& q) const;
	virtual Size RatioSize(const Val& q, i32 cx, i32 cy) const;

	virtual ~Display();
};

struct AttrText : public ValType<AttrText, 151, Moveable<AttrText> > {
	WTxt    text;
	Val     value;
	Font    font;
	Color   ink;
	Color   normalink;
	Color   paper;
	Color   normalpaper;
	i32     align;
	Image   img;
	i32     imgspc;

	AttrText& Set(const Val& v);
	AttrText& operator=(const Val& v)             { Set(v); return *this; }

	AttrText& Text(const Txt& txt)               { text = txt.ToWTxt(); return *this; }
	AttrText& Text(const WTxt& txt)              { text = txt; return *this; }
	AttrText& Text(tukk txt)                 { text = Txt(txt).ToWTxt(); return *this; }

	AttrText& Ink(Color c)                          { ink = c; return *this; }
	AttrText& NormalInk(Color c)                    { normalink = c; return *this; }
	AttrText& Paper(Color c)                        { paper = c; return *this; }
	AttrText& NormalPaper(Color c)                  { normalpaper = c; return *this; }
	AttrText& SetFont(Font f)                       { font = f; return *this; }

	AttrText& Bold(bool b = true)                   { font.Bold(b); return *this; }
	AttrText& Italic(bool b = true)                 { font.Italic(b); return *this; }
	AttrText& Underline(bool b = true)              { font.Underline(b); return *this; }
	AttrText& Strikeout(bool b = true)              { font.Strikeout(b); return *this; }

	AttrText& Align(i32 a)                          { align = a; return *this; }
	AttrText& Left()                                { return Align(ALIGN_LEFT); }
	AttrText& Center()                              { return Align(ALIGN_CENTER); }
	AttrText& Right()                               { return Align(ALIGN_RIGHT); }
	AttrText& SetImage(const Image& m, i32 spc = 4) { img = m; imgspc = spc; return *this; }

	void  Serialize(Stream& s);
	void  Jsonize(JsonIO& jio);
	void  Xmlize(XmlIO& xio);

	bool  operator==(const AttrText& f) const;
	bool  operator!=(const AttrText& f) const       { return !operator==(f); }

	hash_t GetHashVal() const                     { return drx::GetHashVal(value); }
	bool   IsNullInstance() const                   { return IsNull(text); }
	void   SetNull()                                { Init(); img = Null; text = Null; }

	Txt      ToTxt() const                       { return AsTxt(value); }
	i32      Compare(const AttrText& x) const       { return value.Compare(x.value); }
	i32      PolyCompare(const Val& v) const      { return value.Compare(v); }

	operator Val() const;
	AttrText(const Val& v);
	AttrText()                                      { Init(); }

private:
	void Init();
};

const Display& StdDisplay();
const Display& StdCenterDisplay();
const Display& StdRightDisplay();

const Display& ColorDisplay();
const Display& SizeTextDisplay();
const Display& ImageDisplay();
const Display& FittedImageDisplay();
const Display& CenteredImageDisplay();
const Display& CenteredHighlightImageDisplay();
const Display& DrawingDisplay();

class ColorDisplayNull : public Display {
public:
	ColorDisplayNull(Txt nulltext = Null) : nulltext(nulltext) {}
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
		               Color ink, Color paper, dword style) const;
private:
	Txt nulltext;
};

class DisplayWithIcon : public Display {
	const Display *display;
	Image icon;
	i32   lspc;
public:
	virtual void PaintBackground(Draw& w, const Rect& r, const Val& q,
	                             Color ink, Color paper, dword style) const;
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
		               Color ink, Color paper, dword style) const;
	virtual Size GetStdSize(const Val& q) const;

	void SetIcon(const Image& img, i32 spc = 4)             { icon = img; lspc = spc; }
	void SetDisplay(const Display& d)                       { display = &d; }
	void Set(const Display& d, const Image& m, i32 spc = 4) { SetIcon(m, spc); SetDisplay(d); }

	DisplayWithIcon();
};

class PaintRect : Moveable<PaintRect> {
protected:
	Val          value;
	const Display *display;

public:
	void     Paint(Draw& w, const Rect& r,
	               Color ink = SColorText, Color paper = SColorPaper, dword style = 0) const;
	void     Paint(Draw& w, i32 x, i32 y, i32 cx, i32 cy,
		           Color ink = SColorText, Color paper = SColorPaper, dword style = 0) const;
	Size     GetStdSize() const;
	Size     RatioSize(i32 cx, i32 cy) const;
	Size     RatioSize(Size sz) const              { return RatioSize(sz.cx, sz.cy); }

	void     SetDisplay(const Display& d)          { display = &d; }
	void     SetVal(const Val& v)              { value = v; }
	void     Set(const Display& d, const Val& v) { display = &d; value = v; }
	void     Clear()                               { display = NULL; }

	const Val&   GetVal() const                { return value; }
	const Display& GetDisplay() const              { return *display; }

	operator bool() const                          { return display; }

	PaintRect();
	PaintRect(const Display& display);
	PaintRect(const Display& display, const Val& val);
};