enum {
	CTRL_NORMAL, CTRL_HOT, CTRL_PRESSED, CTRL_DISABLED,
	CTRL_CHECKED, CTRL_HOTCHECKED
};

void CtrlsImageLook(Val *look, i32 i, i32 n = 4);
void CtrlsImageLook(Val *look, i32 i, const Image& image, const Color *color, i32 n = 4);
void CtrlsImageLook(Val *look, i32 i, const Image& image, i32 n = 4);

Txt  DeAmp(tukk s);

Size GetSmartTextSize(tukk text, Font font = StdFont(), i32 cx = INT_MAX);
i32  GetSmartTextHeight(tukk s, i32 cx, Font font = StdFont());
void DrawSmartText(Draw& w, i32 x, i32 y, i32 cx, tukk text,
                   Font font = StdFont(), Color ink = SBlack(), i32 accesskey = 0,
                   Color qtf_ink = Null);

i32   ExtractAccessKey(tukk s, Txt& label);
bool  CompareAccessKey(i32 accesskey, dword key);
i32   ChooseAccessKey(tukk s, dword used);

void DrawFocus(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color c = SColorText());
void DrawFocus(Draw& w, const Rect& r, Color c = SColorText());

void DrawHorzDrop(Draw& w, i32 x, i32 y, i32 cx);
void DrawVertDrop(Draw& w, i32 x, i32 y, i32 cy);

Point GetDragScroll(Ctrl *ctrl, Point p, Size max);
Point GetDragScroll(Ctrl *ctrl, Point p, i32 max = 16);

struct DrawLabelBasic {
	Txt       text;
	Font      font;
	Color     ink;
	Color     disabledink;

	i32       accesskey;

	i32       align:4;
	i32       valign:4;
	
	bool      nowrap:1;
	
	DrawLabelBasic() { align = valign = ALIGN_CENTER; nowrap = false; accesskey = 0; font = StdFont(); }
};

struct DrawLabelExt {
	PaintRect paintrect;

	Image     limg;
	Image     rimg;

	Color     lcolor;
	i32       lspc = 0;
	Color     rcolor;
	i32       rspc = 0;

	bool      limg_never_hide = false;
	bool      rimg_never_hide = false;
};

struct DrawLabel : DrawLabelBasic, DrawLabelExt {
	bool      push = false;
	bool      focus = false;
	bool      disabled = false;

	Size      GetSize(i32 txtcx, Size sz1, i32 lspc, Size sz2, i32 rspc) const;
	Size      GetSize(i32 txtcx = INT_MAX) const;
	Rect      PaintRect(Ctrl *ctrl, Draw& w, const Rect& r, bool visibleaccesskey) const;
	Size      Paint(Ctrl *ctrl, Draw& w, const Rect& r, bool visibleaccesskey = true) const;
	Size      Paint(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey = true) const;
	Rect      PaintRect(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey = true) const;
	Size      Paint(Draw& w, const Rect& r, bool visibleaccesskey = true) const;
	Size      Paint(Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey = true) const;
	Rect      PaintRect(Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey = true) const;
};

Image DisabledImage(const Image& img, bool disabled = true);
Color GetLabelTextColor(const Ctrl *ctrl);

class LabelBase {
protected:
	virtual void  LabelUpdate();

	DrawLabelBasic    lbl;
	One<DrawLabelExt> ext;
	
	DrawLabelExt& Ext()                                      { if(!ext) ext.Create() ; return *ext; }
	DrawLabel     Make() const;

public:
	LabelBase&  SetLeftImage(const Image& bmp1, i32 spc = 0, bool never_hide = false);
	LabelBase&  SetPaintRect(const PaintRect& pr);
	LabelBase&  SetText(tukk text);
	LabelBase&  SetFont(Font font);
	LabelBase&  SetInk(Color color, Color disabledink);
	LabelBase&  SetInk(Color color)                          { return SetInk(color, color); }
	LabelBase&  SetRightImage(const Image& bmp2, i32 spc = 0, bool never_hide = false);
	LabelBase&  SetAlign(i32 align);
	LabelBase&  AlignLeft()                                  { return SetAlign(ALIGN_LEFT); }
	LabelBase&  AlignCenter()                                { return SetAlign(ALIGN_CENTER); }
	LabelBase&  AlignRight()                                 { return SetAlign(ALIGN_RIGHT); }
	LabelBase&  SetVAlign(i32 align);
	LabelBase&  AlignTop()                                   { return SetVAlign(ALIGN_TOP); }
	LabelBase&  AlignVCenter()                               { return SetVAlign(ALIGN_CENTER); }
	LabelBase&  AlignBottom()                                { return SetVAlign(ALIGN_BOTTOM); }
	LabelBase&  SetImage(const Image& bmp, i32 spc = 0, bool never_hide = false)
	{ SetLeftImage(bmp, spc, never_hide); return *this; }
	LabelBase&  NoWrap(bool b = true);

	i32         GetAlign() const                             { return lbl.align; }
	i32         GetVAlign() const                            { return lbl.valign; }
	PaintRect   GetPaintRect() const                         { return ext ? ext->paintrect : PaintRect(); }
	Txt         GetText() const                              { return lbl.text; }
	Font        GetFont() const                              { return lbl.font; }
	Color       GetInk() const                               { return lbl.ink; }

	Size        PaintLabel(Ctrl *ctrl, Draw& w, const Rect& r,
	                       bool disabled = false, bool push = false, bool focus = false, bool vak = true);
	Size        PaintLabel(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy,
	                       bool disabled = false, bool push = false, bool focus = false, bool vak = true);
	Size        PaintLabel(Draw& w, const Rect& r,
	                       bool disabled = false, bool push = false, bool focus = false, bool vak = true);
	Size        PaintLabel(Draw& w, i32 x, i32 y, i32 cx, i32 cy,
	                       bool disabled = false, bool push = false, bool focus = false, bool vak = true);
	Size        GetLabelSize() const;
	
	LabelBase(const LabelBase& src)   { lbl = src.lbl; if(src.ext) ext = clone(src.ext); }
	LabelBase()                       {}

	virtual ~LabelBase();
};

Rect LookMargins(const Rect& r, const Val& ch);

class ActiveEdgeFrame : public CtrlFrame {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FramePaint(Draw& w, const Rect& r);
	virtual void FrameAddSize(Size& sz);

private:
	Val          coloredge;
	const Val*   edge;
	const Ctrl*  ctrl;
	Color        color;
	bool         mousein:1;
	bool         push:1;
	bool         button:1;

public:
	void Set(const Ctrl *ctrl, const Val *edge, bool active);
	void Mouse(bool in)                     { mousein = in; }
	void Push(bool b)                       { button = true; push = b; }
	void SetColor(const Val& ce, Color c) { coloredge = ce; color = c; }

	ActiveEdgeFrame() { edge = NULL; mousein = push = button = false; }
};
