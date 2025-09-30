#include "CtrlLib.h"

namespace drx {

void CtrlsImageLook(Val *look, i32 i, i32 n)
{
	while(n--)
		*look++ = CtrlsImg::Get(i++);
}

void CtrlsImageLook(Val *look, i32 i, const Image& image, const Color *color, i32 n)
{
	for(i32 q = 0; q < n; q++)
		*look++ = ChLookWith(CtrlsImg::Get(i++), image, *color++);
}

void CtrlsImageLook(Val *look, i32 i, const Image& image, i32 n)
{
	for(i32 q = 0; q < n; q++)
		*look++ = ChLookWith(CtrlsImg::Get(i++), image);
}

Txt DeAmp(tukk s)
{
	Txt out;
	for(; *s; out.Cat(*s++))
		if(*s == '&')
			out.Cat('&');
	return out;
}

Size GetSmartTextSize(tukk text, Font font, i32 cx) {
	if(*text == '\1') {
		Size sz;
		RichText txt = ParseQTF(text + 1);
		txt.ApplyZoom(GetRichTextStdScreenZoom());
		sz.cx = min(cx, txt.GetWidth());
		sz.cy = txt.GetHeight(Zoom(1, 1), sz.cx);
		return sz;
	}
	return GetTLTextSize(ToUnicode(text, CHARSET_DEFAULT), font);
}

i32 GetSmartTextHeight(tukk s, i32 cx, Font font) {
	if(*s == '\1') {
		Size sz;
		RichText txt = ParseQTF(s + 1);
		txt.ApplyZoom(GetRichTextStdScreenZoom());
		return txt.GetHeight(Zoom(1, 1), cx);
	}
	i32 cy = font.Info().GetHeight();
	i32 h = cy;
	while(*s) {
		if(*s == '\n')
			h += cy;
		s++;
	}
	return h;
}

void DrawSmartText(Draw& draw, i32 x, i32 y, i32 cx, tukk text, Font font, Color ink, i32 accesskey, Color qtf_ink) {
	if(*text == '\1') {
		RichText txt = ParseQTF(text + 1, accesskey);
		txt.ApplyZoom(GetRichTextStdScreenZoom());
		PaintInfo pi;
		pi.darktheme = IsDarkTheme();
		pi.textcolor = qtf_ink;
		txt.Paint(draw, x, y, cx, pi);
		return;
	}
	DrawTLText(draw, x, y, cx, ToUnicode(text, CHARSET_DEFAULT), font, ink, accesskey);
}

bool CompareAccessKey(i32 accesskey, dword key)
{
	return accesskey && dword(ToUpper(accesskey & 255) - 'A' + K_ALT_A) == key;
}

i32  ExtractAccessKey(tukk s, Txt& label)
{
	u8 akey = 0;
	i32  pos = 0;
	Txt text;
	bool qtf = *s == '\1';
	while(*s)
		if((*s == '&' && !qtf || *s == '\b') && s[1] && s[1] != '&') {
			akey = ToAscii(ToUpper(s[1]));
			pos = text.GetLength() + 1;
			s++;
		}
		else
			text.Cat(*s++);
	text.Shrink();
	label = text;
	return MAKELONG(akey, pos);
}

i32  ChooseAccessKey(tukk text, dword used)
{
	for(tukk s = text; *s; s++) {
		u8 ac = *s;
		if(ac < 128 && ac >= 'A' && ac <= 'Z' && (Ctrl::AccessKeyBit(ac) & used) == 0)
			return MAKELONG(ac, s - text + 1);
	}
	for(tukk s = text; *s; s++) {
		dword ac = ToUpper(*s);
		if(ac < 128 && ac >= 'A' && ac <= 'Z' && ac != 'I' && ac != 'L' && (Ctrl::AccessKeyBit(ac) & used) == 0)
			return ac;
	}
	for(tukk s = text; *s; s++) {
		dword ac = ToUpper(*s);
		if(ac < 128 && ac >= 'A' && ac <= 'Z' && (Ctrl::AccessKeyBit(ac) & used) == 0)
			return ac;
	}
	return 0;
}

Size DrawLabel::GetSize(i32 txtcx) const
{
	return GetSize(txtcx, limg.GetSize(), lspc, rimg.GetSize(), rspc);
}

Size DrawLabel::GetSize(i32 txtcx, Size sz1, i32 lspc, Size sz2, i32 rspc) const
{
	Size isz(0, 0);
	Size txtsz = *text ? GetSmartTextSize(text, font, nowrap ? INT_MAX/2 : txtcx)
	                   : paintrect.GetStdSize();

	if(!IsNull(lspc)) {
		isz.cx = lspc;
		isz.cy = sz1.cy;
		isz.cx += sz1.cx;
	}

	if(!IsNull(rspc)) {
		isz.cx += rspc;
		if(sz2.cy > isz.cy) isz.cy = sz2.cy;
		isz.cx += sz2.cx;
	}

	isz.cy = max(txtsz.cy, max(sz1.cy, sz2.cy));
	isz.cx += txtsz.cx;

	return isz;
}

Image DisImage(const Image& m)
{
	Image mm = Grayscale(m, 200);
	ImageBuffer ib(mm);
	RGBA *s = ~ib;
	RGBA *e = s + ib.GetLength();
	while(s < e)
		(s++)->a /= 3;
	Premultiply(ib);
	return static_cast<Image>(ib);
}

Image DisabledImage(const Image& img, bool dis)
{
	return dis ? MakeImage(img, GUI_GlobalStyle() == GUISTYLE_CLASSIC ? Etched : DisImage)
	           : img;
}

Color GetLabelTextColor(const Ctrl *ctrl)
{
	if(!IsLabelTextColorMismatch())
		return SColorLabel();
	while(ctrl) {
		if(!ctrl->IsTransparent()) {
			if(dynamic_cast<const TopWindow *>(ctrl) || dynamic_cast<const TabCtrl *>(ctrl) ||
			   dynamic_cast<const ToolBar *>(ctrl) || dynamic_cast<const MenuBar *>(ctrl) ||
			   dynamic_cast<const StaticRect *>(ctrl) || dynamic_cast<const StaticBarArea *>(ctrl))
				break;
			return SColorText();
		}
		ctrl = ctrl->GetParent();
	}
	return SColorLabel();
}

Rect DrawLabel::PaintRect(Ctrl *ctrl, Draw& w, const Rect& r, bool visibleaccesskey) const
{
	i32 lspc = this->lspc;
	i32 rspc = this->rspc;
	Size sz1 = limg.GetSize();
	Size sz2 = rimg.GetSize();
	i32 txtcx = r.GetWidth() - sz1.cx - Nvl(lspc, 0) - sz2.cx - Nvl(rspc, 0);
	bool donowrap = nowrap && *text != '\1';
	Size txtsz = *text ? GetSmartTextSize(text, font, donowrap ? INT_MAX/2 : txtcx) : paintrect.GetStdSize();
	if(txtsz.cx) {
		if(!rimg_never_hide && txtsz.cx + sz1.cx + sz2.cx + Nvl(lspc, 0) + Nvl(rspc, 0) > r.GetWidth()) {
			sz2.cx = 0;
			rspc = 0;
		}
		if(!limg_never_hide && txtsz.cx + sz1.cx + sz2.cx + Nvl(lspc, 0) + Nvl(rspc, 0) > r.GetWidth()) {
			sz1.cx = 0;
			lspc = 0;
		}
	}
	Size isz = GetSize(txtcx, sz1, lspc, sz2, rspc);
	Point p = r.TopLeft(), ip;
	if(align == ALIGN_LEFT)
		p.x = r.left;
	else
	if(align == ALIGN_RIGHT)
		p.x = r.right - isz.cx;
	else
	if(align == ALIGN_CENTER)
		p.x = (r.right + r.left - isz.cx) / 2;
	if(valign == ALIGN_TOP)
		p.y = r.top;
	else
	if(valign == ALIGN_BOTTOM)
		p.y = r.bottom - isz.cy;
	else
	if(valign == ALIGN_CENTER)
		p.y = (r.bottom + r.top - txtsz.cy) / 2;
	Color color = disabled && !IsNull(disabledink) ? disabledink : ink;
	if(IsNull(color))
		color = disabled ? SColorDisabled : GetLabelTextColor(ctrl);
	i32 ix;
	if(IsNull(lspc))
		ix = r.left + push;
	else {
		ix = p.x + push;
		p.x += sz1.cx;
		p.x += lspc;
	}
	i32 iy = push + (r.top + r.bottom - sz1.cy) / 2;

	if(sz1.cx) {
		if(IsNull(lcolor))
			w.DrawImage(ix, iy, DisabledImage(limg, disabled));
		else
			w.DrawImage(ix, iy, limg, lcolor);
	}
	iy = push + (r.top + r.bottom - sz2.cy) / 2;
	ix = (IsNull(rspc) ? r.right - sz2.cx : p.x + txtsz.cx + rspc) + push;
	if(sz2.cx) {
		if(IsNull(rcolor))
			w.DrawImage(ix, iy, DisabledImage(rimg, disabled));
		else
			w.DrawImage(ix, iy, rimg, rcolor);
	}
	paintrect.Paint(w, p.x + push, p.y + push, txtsz.cx, isz.cy, color, Null);
	
	if(*text) {
		if(disabled && *text != '\1')
			DrawSmartText(w, p.x + push + 1, p.y + push + 1,
			              donowrap ? INT_MAX/2 : txtcx, text, font, SColorPaper);
		DrawSmartText(w, p.x + push, p.y + push, donowrap ? INT_MAX/2 : txtcx,
		              text, font, color, visibleaccesskey ? accesskey : 0);
		if(focus)
			DrawFocus(w, p.x - 2, p.y, txtsz.cx + 5, isz.cy);
	}

	return Rect(p, isz);
}

Size DrawLabel::Paint(Ctrl *ctrl, Draw& w, const Rect& r, bool visibleaccesskey) const
{
	return PaintRect(ctrl, w, r, visibleaccesskey).GetSize();
}

Size DrawLabel::Paint(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey) const
{
	return Paint(ctrl, w, RectC(x, y, cx, cy), visibleaccesskey);
}

Rect DrawLabel::PaintRect(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool visibleaccesskey) const
{
	return PaintRect(ctrl, w, RectC(x, y, cx, cy), visibleaccesskey);
}

Size DrawLabel::Paint(Draw& w, const Rect& r, bool visibleaccesskey) const
{
	return Paint(NULL, w, r, visibleaccesskey);
}

Size DrawLabel::Paint(Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool vak) const
{
	return Paint(w, RectC(x, y, cx, cy), vak);
}

Rect DrawLabel::PaintRect(Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool vak) const
{
	return PaintRect(NULL, w, RectC(x, y, cx, cy), vak);
}

void LabelBase::LabelUpdate() {}

DrawLabel LabelBase::Make() const
{
	DrawLabel lx;
	(DrawLabelBasic&)lx = lbl;
	if(ext)
		(DrawLabelExt&)lx = *ext;
	return lx;
}

LabelBase& LabelBase::SetLeftImage(const Image& img, i32 spc, bool never_hide) {
	DrawLabelExt& lbl = Ext();
	lbl.limg = img;
	lbl.lspc = spc;
	lbl.limg_never_hide = never_hide;
	LabelUpdate();
	return *this;
}

LabelBase& LabelBase::SetRightImage(const Image& img, i32 spc, bool never_hide) {
	DrawLabelExt& lbl = Ext();
	lbl.rimg = img;
	lbl.rspc = spc;
	lbl.rimg_never_hide = never_hide;
	LabelUpdate();
	return *this;
}

LabelBase& LabelBase::SetPaintRect(const PaintRect& paintrect) {
	Ext().paintrect = paintrect;
	LabelUpdate();
	return *this;
}


LabelBase& LabelBase::SetText(tukk text) {
	lbl.text = text;
	LabelUpdate();
	return *this;
}

LabelBase& LabelBase::SetFont(Font font) {
	if(lbl.font != font) {
		lbl.font = font;
		LabelUpdate();
	}
	return *this;
}

LabelBase& LabelBase::NoWrap(bool b)
{
	if(lbl.nowrap != b) {
		lbl.nowrap = b;
		LabelUpdate();
	}
	return *this;
}

LabelBase& LabelBase::SetInk(Color ink, Color disabledink) {
	if(lbl.ink != ink || lbl.disabledink != disabledink) {
		lbl.ink = ink;
		lbl.disabledink = disabledink;
		LabelUpdate();
	}
	return *this;
}

LabelBase& LabelBase::SetAlign(i32 align) {
	if(lbl.align != align) {
		lbl.align = align;
		LabelUpdate();
	}
	return *this;
}

LabelBase& LabelBase::SetVAlign(i32 valign) {
	if(lbl.valign != valign) {
		lbl.valign = valign;
		LabelUpdate();
	}
	return *this;
}

Size LabelBase::PaintLabel(Ctrl *ctrl, Draw& w, const Rect& r, bool disabled, bool push, bool focus, bool vak)
{
	DrawLabel lbl1 = Make();
	lbl1.disabled = disabled;
	lbl1.push = push;
	lbl1.focus = focus;
	return lbl1.Paint(ctrl, w, r, vak);
}

Size LabelBase::PaintLabel(Ctrl *ctrl, Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool disabled, bool push, bool focus, bool vak)
{
	return PaintLabel(ctrl, w, RectC(x, y, cx, cy), disabled, push, focus, vak);
}

Size LabelBase::PaintLabel(Draw& w, const Rect& r, bool disabled, bool push, bool focus, bool vak)
{
	return PaintLabel(NULL, w, r, disabled, push, focus, vak);
}


Size LabelBase::PaintLabel(Draw& w, i32 x, i32 y, i32 cx, i32 cy, bool disabled, bool push, bool focus, bool vak)
{
	return PaintLabel(w, RectC(x, y, cx, cy), disabled, push, focus, vak);
}

Size LabelBase::GetLabelSize() const
{
	return Make().GetSize();
}

void LinkToolTipIn__();

LabelBase::~LabelBase()
{
	LinkToolTipIn__();
}

void DrawFocus(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color c) {
	w.Clipoff(x, y, cx, cy);
	for(i32 a = 0; a < cx; a += CtrlImg::focus_h().GetWidth()) {
		w.DrawImage(a, 0, CtrlImg::focus_h(), c);
		w.DrawImage(a, cy - DPI(1), CtrlImg::focus_h(), c);
	}
	for(i32 a = 0; a < cy; a += CtrlImg::focus_v().GetHeight()) {
		w.DrawImage(0, a, CtrlImg::focus_v(), c);
		w.DrawImage(cx - DPI(1), a, CtrlImg::focus_v(), c);
	}
	w.End();
}

void DrawFocus(Draw& w, const Rect& r, Color c) {
	DrawFocus(w, r.left, r.top, r.Width(), r.Height(), c);
}

void DrawHorzDrop(Draw& w, i32 x, i32 y, i32 cx)
{
	w.DrawRect(x, y, cx, 2, SColorHighlight);
	w.DrawRect(x, y - 2, 1, 6, SColorHighlight);
	w.DrawRect(x + cx - 1, y - 2, 1, 6, SColorHighlight);
	w.DrawRect(x + 1, y - 1, 1, 4, SColorHighlight);
	w.DrawRect(x + cx - 2, y - 1, 1, 4, SColorHighlight);
}

void DrawVertDrop(Draw& w, i32 x, i32 y, i32 cy)
{
	w.DrawRect(x, y, 2, cy, SColorHighlight);
	w.DrawRect(x - 2, y, 6, 1, SColorHighlight);
	w.DrawRect(x - 2, y + cy - 1, 6, 1, SColorHighlight);
	w.DrawRect(x - 1, y + 1, 4, 1, SColorHighlight);
	w.DrawRect(x - 1, y + cy - 2, 4, 1, SColorHighlight);
}

Point GetDragScroll(Ctrl *ctrl, Point p, Size max)
{
	if(ctrl->IsReadOnly())
		return Point(0, 0);
	Size sz = ctrl->GetSize();
	Size sd = min(sz / 6, Size(16, 16));
	Point d(0, 0);
	if(p.x < sd.cx)
		d.x = p.x - sd.cx;
	if(p.x > sz.cx - sd.cx)
		d.x = p.x - sz.cx + sd.cx;
	if(p.y < sd.cy)
		d.y = p.y - sd.cy;
	if(p.y > sz.cy - sd.cy)
		d.y = p.y - sz.cy + sd.cy;
	d.x = minmax(d.x, -max.cx, max.cx);
	d.y = minmax(d.y, -max.cy, max.cy);
	return d;
}

Point GetDragScroll(Ctrl *ctrl, Point p, i32 max)
{
	return GetDragScroll(ctrl, p, Size(max, max));
}

Rect LookMargins(const Rect& r, const Val& ch)
{
	Rect m = ChMargins(ch);
	i32 fcy = GetStdFontCy();
	if(m.top + m.bottom + fcy > r.GetHeight())
		m.top = m.bottom = max((r.GetHeight() - fcy) / 2, 0);
	return m;
}

void ActiveEdgeFrame::FrameLayout(Rect& r)
{
	Rect m = LookMargins(r, edge[0]);
	r.left += m.left;
	r.right -= m.right;
	r.top += m.top;
	r.bottom -= m.bottom;
}

void ActiveEdgeFrame::FramePaint(Draw& w, const Rect& r)
{
	i32 i = 0;
	if(ctrl) {
		i = !ctrl->IsEnabled() || ctrl->IsReadOnly() ? CTRL_DISABLED
		    : button ? push : ctrl->HasFocus() ? CTRL_PRESSED
		    : mousein ? CTRL_HOT
		    : CTRL_NORMAL;
	}
	ChPaintEdge(w, r, edge[i]);
	if(!IsNull(coloredge))
		ChPaintEdge(w, r, coloredge, color);
}

void ActiveEdgeFrame::FrameAddSize(Size& sz)
{
	Rect m = ChMargins(edge[0]);
	sz.cx += m.left + m.right;
	sz.cy += m.top + m.bottom;
}

void ActiveEdgeFrame::Set(const Ctrl *ctrl_, const Val *edge_, bool active)
{
	ctrl = active ? ctrl_ : NULL;
	edge = edge_;
}

}
