#include "CtrlLib.h"

namespace drx {

#define LLOG(x) // LOG(x)

void Sb(Button::Style& bs, const Image& img)
{
	bs = Button::StyleNormal();
	ChLookWith(bs.look, img, bs.monocolor);
}

CH_STYLE(ScrollBar, Style, StyleDefault)
{
	arrowsize = ScrollBarArrowSize();
	barsize = FrameButtonWidth();
	thumbmin = 16;
	overthumb = 0;
	through = false;
	CtrlsImageLook(vupper, CtrlsImg::I_SBVU);
	CtrlsImageLook(vthumb, CtrlsImg::I_SBVT, CtrlsImg::SBVI());
	CtrlsImageLook(vlower, CtrlsImg::I_SBVL);
	CtrlsImageLook(hupper, CtrlsImg::I_SBHU);
	CtrlsImageLook(hthumb, CtrlsImg::I_SBHT, CtrlsImg::SBHI());
	CtrlsImageLook(hlower, CtrlsImg::I_SBHL);
	Sb(up, CtrlsImg::UA());
	Sb(up2, CtrlsImg::UA());
	Sb(down, CtrlsImg::DA());
	Sb(down2, CtrlsImg::DA());
	Sb(left, CtrlsImg::LA());
	Sb(left2, CtrlsImg::LA());
	Sb(right, CtrlsImg::RA());
	Sb(right2, CtrlsImg::RA());
	isup2 = isdown2 = isleft2 = isright2 = false;
	thumbwidth = Null;
	bgcolor = SColorPaper();
}

ScrollBar::ScrollBar() {
	minthumb = DPI(16);
	pagepos = pagesize = totalsize = 0;
	linesize = 1;
	autohide = false;
	autodisable = true;
	jump = false;
	track = true;
	horz = false;
	push = light = -1;

	thumbsize = 8;
	thumbpos = 0;
	NoWantFocus();
	style = NULL;
	SetStyle(StyleDefault());
	BackPaint();
	is_active = false;
}

ScrollBar::~ScrollBar() {}

i32 ScrollBar::ButtonCount() const
{
	return BUTTONCOUNT;
}

void ScrollBar::Layout() {
	Set(pagepos);
	Refresh();
}

Rect ScrollBar::ButtonRect(i32 i) const
{
	Size sz = GetSize();
	if(IsHorz()) {
		i32 cc = sz.cx > (3 + style->isleft2 + style->isright2) * style->arrowsize ? style->arrowsize : 0;
		if(i == PREV)
			return RectC(0, 0, cc, sz.cy);
		if(i == NEXT)
			return RectC(sz.cx - cc, 0, cc, sz.cy);
		if(i == PREV2 && style->isleft2)
			return RectC(sz.cx - 2 * cc, 0, cc, sz.cy);
		if(i == NEXT2 && style->isright2)
			return RectC(cc, 0, cc, sz.cy);
	}
	else {
		i32 cc = sz.cy > (3 + style->isup2 + style->isdown2) * style->arrowsize ? style->arrowsize : 0;
		if(i == PREV)
			return RectC(0, 0, sz.cx, cc);
		if(i == NEXT)
			return RectC(0, sz.cy - cc, sz.cx, cc);
		if(i == PREV2 && style->isup2)
			return RectC(0, sz.cy - 2 * cc, sz.cx, cc);
		if(i == NEXT2 && style->isdown2)
			return RectC(0, cc, sz.cx, cc);
	}
	return Null;
}

bool ScrollBar::ButtonEnabled(i32 i) const
{
	return is_active || !autodisable;
}

void ScrollBar::ButtonPush(i32 i)
{
	WhenLeftClick();
	ButtonRepeat(i);
}

void ScrollBar::ButtonRepeat(i32 i)
{
	if(i == PREV)
		PrevLine();
	if(i == NEXT)
		NextLine();
	if(i == PREV2)
		PrevPage();
	if(i == NEXT2)
		NextPage();
}

const Button::Style& ScrollBar::ButtonStyle(i32 i) const
{
	if(IsHorz()) {
		if(i == PREV)
			return style->left;
		if(i == NEXT)
			return style->right;
		if(i == PREV2)
			return style->left2;
		if(i == NEXT2)
			return style->right2;
	}
	else {
		if(i == PREV)
			return style->up;
		if(i == NEXT)
			return style->down;
		if(i == PREV2)
			return style->up2;
		if(i == NEXT2)
			return style->down2;
	}
	return Button::StyleNormal();
}

Rect ScrollBar::Slider(i32& cc) const
{
	Size sz = GetSize();
	Rect r;
	if(IsHorz()) {
		cc = sz.cx > (3 + style->isleft2 + style->isright2) * style->arrowsize ? style->arrowsize : 0;
		r = RectC(cc, 0, sz.cx - 2 * cc, sz.cy);
		if(style->isleft2)
			r.right -= cc;
		if(style->isright2)
			r.left += cc;
	}
	else {
		cc = sz.cy > (3 + style->isup2 + style->isdown2) * style->arrowsize ? style->arrowsize : 0;
		r = RectC(0, cc, sz.cx, sz.cy - 2 * cc);
		if(style->isup2)
			r.bottom -= cc;
		if(style->isdown2)
			r.top += cc;
	}
	return r;
}

Rect ScrollBar::Slider() const
{
	i32 dummy;
	return Slider(dummy);
}

i32& ScrollBar::HV(i32& h, i32& v) const
{
	return IsHorz() ? h : v;
}

i32 ScrollBar::GetHV(i32 h, i32 v) const {
	return IsHorz() ? h : v;
}

Rect ScrollBar::GetPartRect(i32 p) const {
	Rect h = Slider();
	i32 sbo = style->overthumb;
	i32 off = GetHV(h.left, h.top);
	i32 ts = thumbsize;
	if(ts < style->thumbmin)
		ts = 0;
	switch(p) {
	case 0:
		HV(h.right, h.bottom) = thumbpos - sbo + ts / 2 + off;
		break;
	case 1:
		HV(h.left, h.top) = thumbpos + ts / 2 + sbo + off;
		break;
	case 2:
		if(!IsNull(style->thumbwidth))
			h.Deflate((style->barsize - style->thumbwidth) / 2);
		HV(h.left, h.top) = thumbpos - sbo + off;
		HV(h.right, h.bottom) = thumbpos + ts + sbo + off;
		break;
	}
	return h;
}

void ScrollBar::Paint(Draw& w)
{
	w.DrawRect(GetSize(), style->bgcolor);
	i32 cc;
	Size sz = style->through ? GetSize() : Slider(cc).GetSize();
	light = GetMousePart();
	i32 p = push;
	if(!HasCapture() || buttons_capture)
		p = -1;
	const Val *hl[] = { style->hlower, style->hupper, style->hthumb };
	const Val *vl[] = { style->vupper, style->vlower, style->vthumb };

	const Val **l = IsHorz() ? hl : vl;

	if(is_active || !autodisable) {
		for(i32 i = 0; i < 3; i++) {
			Rect pr = GetPartRect(i);
			if(i != 2) {
				w.Clip(pr);
				pr = style->through ? GetSize() : Slider();
			}
			if(i != 2 || thumbsize >= style->thumbmin)
				ChPaint(w, pr, l[i][p == i ? CTRL_PRESSED : light == i && !buttons_capture ? CTRL_HOT : CTRL_NORMAL]);
			if(i != 2)
				w.End();
		}
	}
	else {
		if(style->through)
			ChPaint(w, sz, l[0][CTRL_DISABLED]);
		else
		if(IsHorz())
			ChPaint(w, cc, 0, sz.cx, sz.cy, l[0][CTRL_DISABLED]);
		else
			ChPaint(w, 0, cc, sz.cx, sz.cy, l[0][CTRL_DISABLED]);
	}
	PaintButtons(w, this);
}


i32  ScrollBar::GetMousePart()
{
	i32 q = -1;
	for(i32 i = 2; i >= 0; i--)
		if(HasMouseIn(GetPartRect(i))) {
			q = i;
			break;
		}
	return q;
}

i32  ScrollBar::GetRange() const {
	Size sz = Slider().GetSize();
	return GetHV(sz.cx, sz.cy);
}

void ScrollBar::Bounds() {
	i32 maxsize = GetRange();
	if(thumbsize > maxsize)
		thumbsize = maxsize;
	if(thumbpos + thumbsize > maxsize)
		thumbpos = maxsize - thumbsize;
	if(thumbpos < 0)
		thumbpos = 0;
}

bool ScrollBar::SetThumb(i32 _thumbpos, i32 _thumbsize) {
	i32 ts = thumbsize;
	i32 tp = thumbpos;
	thumbsize = _thumbsize;
	thumbpos = _thumbpos;
	Bounds();
	if(thumbsize != ts || thumbpos != tp) {
		Refresh();
		return true;
	}
	return false;
}

void ScrollBar::Drag(Point p) {
	if(SetThumb(max(0, IsHorz() ? p.x - delta : p.y - delta), thumbsize) && track)
		Position();
}

Image ScrollBar::MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags)
{
	if(ButtonsMouseEvent(this, event, p))
		return Image::Arrow();
	return Ctrl::MouseEvent(event, p, zdelta, keyflags);
}

void ScrollBar::LeftDown(Point p, dword) {
	push = GetMousePart();
	LLOG("ScrollBar::LeftDown(" << p << ")");
	LLOG("MousePos = " << GetMousePos() << ", ScreenView = " << GetScreenView()
	<< ", rel. pos = " << (GetMousePos() - GetScreenView().TopLeft()));
	LLOG("GetWorkArea = " << GetWorkArea());
	LLOG("VisibleScreenView = " << GetVisibleScreenView());
	LLOG("PartRect(0) = " << GetPartRect(0));
	LLOG("PartRect(1) = " << GetPartRect(1));
	LLOG("PartRect(2) = " << GetPartRect(2));
	LLOG("ScrollBar::LeftDown: mousepart = " << (i32)push << ", rect = " << GetPartRect(push)
		<< ", overthumb = " << style->overthumb << ", slider = " << Slider());
	LLOG("thumbpos = " << thumbpos << ", thumbsize = " << thumbsize);
	if(push == 2)
		delta = GetHV(p.x, p.y) - thumbpos;
	else {
		if(jump) {
			delta = thumbsize / 2;
			Drag(p);
		}
		else
			if(push == 0)
				PrevPage();
			else
				NextPage();
	}
	SetCapture();
	buttons_capture = false;
	Refresh();
	WhenLeftClick();
}

void ScrollBar::MouseMove(Point p, dword) {
	if(HasCapture() && push == 2)
		Drag(p);
	else
	if(light != GetMousePart())
		Refresh();
}

void ScrollBar::MouseEnter(Point p, dword)
{
	Refresh();
}

void ScrollBar::MouseLeave()
{
	Refresh();
}

void ScrollBar::LeftUp(Point p, dword) {
	ReleaseCapture();
	if(!track)
		Position();
	Refresh();
	push = -1;
}

void ScrollBar::LeftRepeat(Point p, dword) {
	if(jump || push < 0 || push == 2) return;
	if(push == 0)
		PrevPage();
	else
		NextPage();
	Refresh();
}

void ScrollBar::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	Wheel(zdelta);
}

void ScrollBar::CancelMode() {
	push = light = -1;
	ButtonsCancelMode();
}

i32 ScrollBar::GetSliderPos(i32 pos) const
{
	if(totalsize <= 0)
		return Null;
	else
		return pos * GetRange() / totalsize;
}

bool  ScrollBar::Set(i32 apagepos) {
	i32 op = pagepos;
	pagepos = apagepos;
	if(pagepos > totalsize - pagesize) pagepos = totalsize - pagesize;
	if(pagepos < 0) pagepos = 0;
	i32 slsize = GetRange();
	i32 mint = max(minthumb, style->thumbmin);
	if(totalsize <= 0)
		SetThumb(0, slsize);
	else {
		double thumbsize = slsize * pagesize / (double) totalsize;
		double rest = slsize * pagesize - thumbsize * totalsize;
		double ts, ps;
		if(thumbsize >= slsize || thumbsize < 0) {
			ts = slsize;
			ps = 0;
		}
		else
		if(thumbsize <= mint) {
			ps = ((slsize - mint) * (double)pagepos + rest) / (double) (totalsize - pagesize);
			ts = mint;
		}
		else {
			ps = (slsize * (double)pagepos + rest) / (double) totalsize;
			ts = thumbsize;
		}
		SetThumb(ffloor(ps), fceil(ts));
    }
	if(pagepos != op) {
		Refresh();
		WhenScroll();
		return true;
	}
	return false;
}

void ScrollBar::Set(i32 _pagepos, i32 _pagesize, i32 _totalsize) {
	pagesize = _pagesize;
	totalsize = _totalsize;
	
	bool active = totalsize > pagesize && pagesize > 0;
	if(active != is_active) {
		Refresh();
		is_active = active;
	}
	if(autohide && is_active != IsShown()) {
		Show(is_active);
		WhenVisibility();
	}
	Set(_pagepos);
}

void ScrollBar::SetPage(i32 _pagesize) {
	Set(pagepos, _pagesize, totalsize);
}

void ScrollBar::SetTotal(i32 _totalsize) {
	Set(pagepos, pagesize, _totalsize);
}

void ScrollBar::Position() {
	i32 slsize = GetRange();
	i32 mint = max(minthumb, style->thumbmin);
	if(slsize < mint || totalsize <= pagesize)
		pagepos = 0;
	else
	if(thumbpos == slsize - thumbsize)
		pagepos = totalsize - pagesize;
	else
	if(thumbsize == mint)
		pagepos = iscale(thumbpos, (totalsize - pagesize), (slsize - mint));
	else
		pagepos = iscale(thumbpos, totalsize, slsize);
	Action();
	WhenScroll();
}

void ScrollBar::Uset(i32 a) {
	if(Set(a))
		Action();
}

void ScrollBar::PrevLine() {
	Uset(pagepos - linesize);
}

void ScrollBar::NextLine() {
	Uset(pagepos + linesize);
}

void ScrollBar::PrevPage() {
	Uset(pagepos - max(pagesize - linesize, 1));
}

void ScrollBar::NextPage() {
	Uset(pagepos + max(pagesize - linesize, 1));
}

void ScrollBar::Wheel(i32 zdelta, i32 lines) {
	Uset(pagepos - lines * linesize * zdelta / 120);
}

void ScrollBar::Wheel(i32 zdelta) {
	Wheel(zdelta, GUI_WheelScrollLines());
}

bool ScrollBar::VertKey(dword key, bool homeend) {
	if(!IsVisible() || !IsEnabled() || GetRect().IsEmpty())
		return false;
	switch(key) {
	case K_PAGEUP:
		PrevPage();
		break;
	case K_PAGEDOWN:
		NextPage();
		break;
	case K_UP:
		PrevLine();
		break;
	case K_DOWN:
		NextLine();
		break;
	case K_HOME:
		if(!homeend) break;
	case K_CTRL_HOME:
	case K_CTRL_PAGEUP:
		Begin();
		break;
	case K_END:
		if(!homeend) break;
	case K_CTRL_END:
	case K_CTRL_PAGEDOWN:
		End();
		break;
	default:
		return false;
	}
	return true;
}

void ScrollBar::Begin()
{
	Uset(0);
}

void ScrollBar::End()
{
	Uset(max(0, totalsize - pagesize));
}

bool ScrollBar::HorzKey(dword key) {
	if(!IsVisible() || !IsEnabled() || GetRect().IsEmpty())
		return false;
	switch(key) {
	case K_CTRL_LEFT:
		PrevPage();
		break;
	case K_CTRL_RIGHT:
		NextPage();
		break;
	case K_LEFT:
		PrevLine();
		break;
	case K_RIGHT:
		NextLine();
		break;
	case K_HOME:
		Begin();
		break;
	case K_END:
		End();
		break;
	default:
		return false;
	}
	return true;
}

bool ScrollBar::ScrollInto(i32 pos, i32 _linesize) {
	i32 new_pos = pagepos;
	if(pos > new_pos + pagesize - _linesize)
		new_pos = pos - pagesize + _linesize;
	if(pos < new_pos)
		new_pos = pos;
	return Set(new_pos);
}

Size ScrollBar::GetStdSize() const {
	i32 a = HeaderCtrl::GetStdHeight();
	return Size(a, a);
}

void ScrollBar::FrameLayout(Rect& r)
{
	(IsHorz() ? LayoutFrameBottom : LayoutFrameRight)(r, this, ScrollBarSize());
}

void ScrollBar::FrameAddSize(Size& sz)
{
	(IsHorz() ? sz.cy : sz.cx) += ScrollBarSize();
}

Size ScrollBar::GetViewSize() const {
	if(IsChild() && InFrame()) {
		Size sz = GetParent()->GetSize();
		if(IsShown())
			(IsVert() ? sz.cx : sz.cy) += ScrollBarSize();
		return sz;
	}
	return Size(0, 0);
}

Size ScrollBar::GetReducedViewSize() const {
	if(IsChild() && InFrame()) {
		Size sz = GetParent()->GetSize();
		if(!IsShown())
			(IsVert() ? sz.cx : sz.cy) -= ScrollBarSize();
		return sz;
	}
	return Size(0, 0);
}

ScrollBar& ScrollBar::AutoHide(bool b) {
	autohide = b;
	if(b)
		SetTotal(totalsize);
	else
		Show();
	WhenVisibility();
	return *this;
}

ScrollBar& ScrollBar::AutoDisable(bool b) {
	autodisable = b;
	Refresh();
	return *this;
}

ScrollBar& ScrollBar::SetStyle(const Style& s)
{
	if(style != &s) {
		style = &s;
		RefreshLayout();
		Refresh();
	}
	return *this;
}

Image SizeGrip::CursorImage(Point p, dword)
{
	if(GuiPlatformHasSizeGrip()) {
		TopWindow *q = dynamic_cast<TopWindow *>(GetTopCtrl());
		if(q && !q->IsMaximized() && q->IsSizeable())
			return Image::SizeBottomRight();
	}
	return Image::Arrow();
}

CH_IMAGE(SizeGripImg, CtrlsImg::SizeGrip());

void SizeGrip::Paint(Draw& w)
{
	Size sz = GetSize();
	if(!IsTransparent())
	    w.DrawRect(sz, SColorFace);
	if(GuiPlatformHasSizeGrip()) {
		TopWindow *q = dynamic_cast<TopWindow *>(GetTopCtrl());
		if(q && !q->IsMaximized() && q->IsSizeable()) {
			Size isz = CtrlsImg::SizeGrip().GetSize();
			w.DrawImage(sz.cx - isz.cx, sz.cy - isz.cy, CtrlsImg::SizeGrip());
		}
    }
}

SizeGrip::SizeGrip()
{
	Transparent();
	RightPos(0, DPI(12)).BottomPos(0, DPI(12));
	NoWantFocus();
	Width(DPI(14));
}

SizeGrip::~SizeGrip() {}

void SizeGrip::LeftDown(Point p, dword flags)
{
	TopWindow *q = dynamic_cast<TopWindow *>(GetTopCtrl());
	if(q && !q->IsMaximized() && q->IsSizeable())
		GuiPlatformGripResize(q);
}

void ScrollBars::Scroll() {
	WhenScroll();
}

bool ScrollBars::Key(dword key) {
	return x.HorzKey(key) || y.VertKey(key);
}

void ScrollBars::Set(Point pos, Size page, Size total) {
	x.Set(pos.x, page.cx, total.cx);
	y.Set(pos.y, page.cy, total.cy);
}

bool ScrollBars::Set(i32 _x, i32 _y) {
	return x.Set(_x) + y.Set(_y);
}

bool ScrollBars::Set(Point pos) {
	return Set(pos.x, pos.y);
}

void ScrollBars::SetPage(i32 cx, i32 cy) {
	x.SetPage(cx);
	y.SetPage(cy);
}

void ScrollBars::SetPage(Size page) {
	SetPage(page.cx, page.cy);
}

void ScrollBars::SetTotal(i32 cx, i32 cy) {
	x.SetTotal(cx);
	y.SetTotal(cy);
}

void ScrollBars::SetTotal(Size total) {
	SetTotal(total.cx, total.cy);
}

bool ScrollBars::ScrollInto(Point pos, Size linesize) {
	return x.ScrollInto(pos.x, linesize.cx) + y.ScrollInto(pos.y, linesize.cy);
}

bool ScrollBars::ScrollInto(Point pos) {
	return x.ScrollInto(pos.x) + y.ScrollInto(pos.y);
}

ScrollBars& ScrollBars::SetLine(i32 linex, i32 liney) {
	x.SetLine(linex);
	y.SetLine(liney);
	return *this;
}

ScrollBars& ScrollBars::Track(bool b) {
	x.Track(b);
	y.Track(b);
	return *this;
}

ScrollBars& ScrollBars::Jump(bool b) {
	x.Jump(b);
	y.Jump(b);
	return *this;
}

ScrollBars& ScrollBars::AutoHide(bool b) {
	x.AutoHide(b);
	y.AutoHide(b);
	return *this;
}

ScrollBars& ScrollBars::AutoDisable(bool b) {
	x.AutoDisable(b);
	y.AutoDisable(b);
	return *this;
}

ScrollBars& ScrollBars::Box(Ctrl& _box) {
	box->Remove();
	box = &_box;
	if(x.GetParent() && x.GetParent() != box->GetParent())
		x.GetParent()->Add(*box);
	return *this;
}

void ScrollBars::FrameAdd(Ctrl& p) {
	p.Add(x);
	p.Add(y);
	if(box->GetParent() != &p)
		p.Add(*box);
}

void ScrollBars::FrameRemove() {
	x.Remove();
	y.Remove();
	box->Remove();
}

void ScrollBars::FramePaint(Draw& w, const Rect& r) {
	if(x.IsShown() && y.IsShown() && !box) {
		i32 h = ScrollBarSize();
		w.DrawRect(r.right - h, r.bottom - h, h, h, SColorFace);
	}
}

void ScrollBars::FrameLayout(Rect& r) {
	i32 h = ScrollBarSize();
	i32 by = 0;
	i32 bx = x.IsShown() && y.IsShown() ? h : 0;
	if(box_type == 1)
		by = bx;
	if(box_type == 2)
		by = h;
	i32 dx = x.IsShown() * h;
	i32 dy = y.IsShown() * h;
	y.SetFrameRect(r.right - dy, r.top, dy, r.Height() - by);
	x.SetFrameRect(r.left, r.bottom - dx, r.Width() - bx, dx);
	if(box)
		box->SetFrameRect(r.right - by, r.bottom - by, by, by);
	r.right -= dy;
	r.bottom -= dx;
}

void ScrollBars::FrameAddSize(Size& sz) {
	i32 h = ScrollBarSize();
	sz.cy += x.IsShown() * h;
	sz.cx += y.IsShown() * h;
}

Size ScrollBars::GetViewSize() const {
	return Size(y.GetViewSize().cx, x.GetViewSize().cy);
}

Size ScrollBars::GetReducedViewSize() const {
	return Size(y.GetReducedViewSize().cx, x.GetReducedViewSize().cy);
}

ScrollBars& ScrollBars::NormalBox()
{
	box_type = 1;
	y.RefreshParentLayout();
	return *this;
}

ScrollBars& ScrollBars::NoBox()
{
	box_type = 0;
	y.RefreshParentLayout();
	return *this;
}

ScrollBars& ScrollBars::FixedBox()
{
	box_type = 2;
	y.RefreshParentLayout();
	return *this;
}

ScrollBars& ScrollBars::WithSizeGrip()
{
	box_bg.Color(SColorFace());
	the_box.Add(box_bg.SizePos());
	the_box.Add(grip);
	return *this;
}

ScrollBars::ScrollBars() {
	box = &the_box;
//	the_box.NoTransparent();
	x.WhenScroll = y.WhenScroll = callback(this, &ScrollBars::Scroll);
	x.WhenLeftClick = y.WhenLeftClick = Proxy(WhenLeftClick);
	x.AutoHide();
	y.AutoHide();
	box_type = 1;
}

ScrollBars::~ScrollBars() {}

void Scroller::Scroll(Ctrl& p, const Rect& rc, Point newpos, Size cellsize)
{
	if(!IsNull(psb) && !p.IsTransparent()) {
		Point d = psb - newpos;
		p.ScrollView(rc, d.x * cellsize.cx, d.y * cellsize.cy);
	}
	else
		p.Refresh();
	psb = newpos;
}

void Scroller::Scroll(Ctrl& p, const Rect& rc, i32 newposy, i32 linesize)
{
	Scroll(p, rc, Point(0, newposy), Size(0, linesize));
}

void Scroller::Scroll(Ctrl& p, Point newpos)
{
	Scroll(p, p.GetSize(), newpos);
}

void Scroller::Scroll(Ctrl& p, i32 newposy)
{
	Scroll(p, p.GetSize(), newposy);
}

}
