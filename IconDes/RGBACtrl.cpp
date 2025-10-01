#include "IconDes.h"

namespace drx {

Color s_stdcolor[] = {
	Black,
	Red,
	Green,
	Brown,
	Blue,
	Magenta,
	Cyan,
	Gray,
	LtGray,
	LtRed,
	LtGreen,
	LtYellow,
	LtBlue,
	LtMagenta,
	LtCyan,
	Yellow,
	WhiteGray,
	White,
};

Color RGBACtrl::GetColor(i32 i) const
{
	if(i < 18)
		return s_stdcolor[i];
	i -= 18;
	if(i < 18)
		return GrayColor(255 * (i + 1) / 20);
	i -= 18;
	i32 q = i % 18;
	i /= 18;
	Color c = Color(255 * (q < 9 ? q + 1 : 18 - q) / 9,
	                255 * (i < 6 ? i + 1 : 12 - i) / 6,
	                q < 9 ? i < 6 ? 0 : 200 : i < 6 ? 150 : 255);
	return c == White() || c == Yellow() ? Color(c.GetR(), c.GetG() - 1, c.GetB()) : c;
}

void RGBACtrl::Layout()
{
	DoLayout(GetSize(), true);
}

i32 RGBACtrl::DoLayout(Size sz, bool set)
{
	i32 ah = IconDesImg::Alpha().GetHeight() + DPI(20);
	i32 th = text.GetStdHeight(text.GetFont());
	i32 eh = DPI(4 - 20) - ah - th - DPI(4);
	i32 rh = min(sz.cx - (sz.cx >> 3), sz.cy - eh);
	if(set)
		ramp.BottomPos(0, rh).HSizePos();
	i32 y = sz.cy - rh - DPI(4) - ah;
	if(set)
		alpha.TopPos(y, ah).HSizePos(1, 0);
	y -= DPI(4) + th;
	if(set)
		text.TopPos(y, th).HSizePos(1, 0);
	Size cbx;
	cbx.cx = (sz.cx - 3) / 18;
	cbx.cy = minmax(cbx.cx, 4, Zy(16));
	i32 ch = 14 * cbx.cy;
	y -= ch + DPI(4);
	if(set) {
		cbox = cbx;
		cs.x = (sz.cx - 18 * cbox.cx) / 2 + 1;
		cs.y = y;
	}
	if(subctrl) {
		if(set)
			subctrl->TopPos(0, y - DPI(4)).HSizePos(1, 0);
		y -= DPI(4);
	}
	return y;
}

i32 RGBACtrl::GetHeight(i32 cx)
{
	return 100000 - DoLayout(Size(cx, 100000), false);
}

void RGBACtrl::Paint(Draw& w)
{
	w.DrawRect(GetSize(), SColorFace);
	if(alpha.IsMask())
		return;
	for(i32 x = 0; x <= 18; x++)
		w.DrawRect(x * cbox.cx + cs.x, cs.y, 1, cbox.cy * 14, SColorText());
	i32 i = 0;
	i32 my = cs.y + 1;
	w.DrawRect(cs.x, cs.y + 14 * cbox.cy, cbox.cx * 18 + 1, 1, SColorText());
	Point pp = Null;
	for(i32 y = 0; y < 14; y++) {
		w.DrawRect(cs.x, my - 1, cbox.cx * 18 + 1, 1, SColorText());
		i32 mx = cs.x + 1;
		for(i32 x = 0; x < 18; x++) {
			Color c = GetColor(i++);
			w.DrawRect(mx, my, cbox.cx - 1, cbox.cy - 1, c);
			if(c == color)
				pp = Point(mx, my);
			mx += cbox.cx;
		}
		my += cbox.cy;
	}
	if(!IsNull(pp)) {
		Size isz = CtrlImg::wheel_cursor().GetSize();
		pp = pp + (cbox - isz) / 2;
		w.DrawImage(pp.x, pp.y, CtrlImg::wheel_cursor(),
		            Grayscale(color) < 120 ? White() : Black());
	}
}

void RGBACtrl::LeftDown(Point p, dword keyflags)
{
	i32 x = (p.x - cs.x) / cbox.cx;
	i32 y = (p.y - cs.y) / cbox.cy;
	if(x >= 0 && x < 18 && y >= 0 && y < 18) {
		color = GetColor(x + 18 * y);
		ramp <<= color;
		alpha.Set(255);
		alpha.SetColor(color);
		UpdateActionRefresh();
		SyncText();
	}
}

void RGBACtrl::MouseMove(Point p, dword keyflags)
{
	if(keyflags & K_MOUSELEFT)
		LeftDown(p, keyflags);
}

void RGBACtrl::SyncText()
{
	RGBA x = Get();
	text <<= Format("RGBA(%3d, %3d, %3d, %3d)", (i32)x.r, (i32)x.g, (i32)x.b, (i32)x.a);
}

void RGBACtrl::Ramp()
{
	color = ~ramp;
	alpha.SetColor(color);
	UpdateActionRefresh();
	SyncText();
}

void RGBACtrl::Alpha()
{
	SyncText();
	UpdateAction();
}

void RGBACtrl::MaskSet(i32 a)
{
	alpha.Set(a);
	UpdateRefresh();
	SyncText();
}

void RGBACtrl::Set(RGBA c)
{
	Unmultiply(&c, &c, 1);
	color = StraightColor(c);
	ramp <<= color;
	alpha.SetColor(color);
	MaskSet(c.a);
}

RGBA RGBACtrl::Get() const
{
	RGBA c = color;
	c.a = alpha.Get();
	if(alpha.IsMask())
		c.r = c.g = c.b = c.a;
	Premultiply(&c, &c, 1);
	return c;
}

Color RGBACtrl::GetColor() const
{
	return color;
}

i32 RGBACtrl::GetAlpha() const
{
	return alpha.Get();
}

void RGBACtrl::Mask(bool b)
{
	alpha.Mask(b);
	ramp.Show(!b);
	Refresh();
}

void RGBACtrl::SubCtrl(Ctrl *c)
{
	if(subctrl)
		subctrl->Remove();
	subctrl = c;
	if(c)
		Add(*c);
	Layout();
}

RGBACtrl::RGBACtrl()
{
	color = Black();
	BackPaint();
	Add(ramp);
	ramp <<= OTVET(Ramp);
	Add(alpha);
	alpha <<= OTVET(Alpha);
	text.SetFont(CourierZ(11));
	text.NoWantFocus();
	text.SetReadOnly();
	text.SetFrame(BlackFrame());
	text.AddFrame(setcolor);
	setcolor.Width(CtrlImg::color_edit().GetSize().cx + DPI(8));
	setcolor.SetImage(CtrlImg::color_edit());
	setcolor << [=, this] {
		Txt text;
		if(!IsNull(color) && color != VoidColor())
			text = ColorToHtml(color);
		EditText(text, "Установить Цвет", "Значение цвета");
		Color c = ColorFromText(text);
		if(IsNull(c))
			return;
		Set(c);
		UpdateActionRefresh();
	};
	Add(text);
	SyncText();
}

}
