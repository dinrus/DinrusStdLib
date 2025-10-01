#include "CtrlLib.h"
#include "CtrlLib.h"

namespace drx {

CtrlFrame& DropFrame();

void PopUpList::Clear()
{
	items.Clear();
	lineinfo.Clear();
	linedisplay.Clear();
	cursor = -1;
	if(popup)
		popup->ac.Clear();
}

void PopUpList::Remove(i32 i)
{
	items.Remove(i);
	if(i < lineinfo.GetCount())
		lineinfo.Remove(i);
	if(i < linedisplay.GetCount())
		linedisplay.Remove(i);
	if(popup)
		popup->ac.Remove(i);
	if(cursor > i)
		cursor--;
}

void PopUpList::Insert(i32 i, const Val& v)
{
	items.Insert(i, v);
	if(i < lineinfo.GetCount())
		lineinfo.Insert(i, 0x7fff);
	if(i < linedisplay.GetCount())
		linedisplay.Insert(i, nullptr);
	if(popup)
		popup->ac.Insert(i, Vec<Val>() << v);
	if(cursor >= i)
		cursor++;
}

void PopUpList::SetCount(i32 n)
{
	items.SetCount(n);
	if(popup)
		popup->ac.SetCount(n);
}

void PopUpList::AddSeparator()
{
	word& x = lineinfo.At(items.GetCount(), 0x7fff);
	x |= 0x8000;
	items.Add(Null);
	if(popup)
		popup->ac.AddSeparator();
}

void PopUpList::Add(const Val& v)
{
	items.Add(v);
	if(popup)
		popup->ac.Add(v);
}

void PopUpList::SetLineCy(i32 ii, i32 cy)
{
	ASSERT(cy >= 0 && cy < 32000);
	word& x = lineinfo.At(ii, 0x7fff);
	x = (x & 0x8000) | cy;
	if(popup)
		popup->ac.SetLineCy(ii, cy);
}

i32 PopUpList::GetLineCy(i32 ii) const
{
	if(ii >= 0 && ii < lineinfo.GetCount()) {
		word h = lineinfo[ii] & 0x7fff;
		if(h != 0x7fff)
			return h;
	}
	return linecy;
}

bool PopUpList::IsLineEnabled(i32 ii) const
{
	return !(ii >= 0 && ii < lineinfo.GetCount() && ((lineinfo[ii] & 0x8000) || (lineinfo[ii] & 0x7fff) == 0));
}

void PopUpList::Set(i32 i, const Val& v)
{
	items.At(i) = v;
	if(popup)
		popup->ac.Set(i, 0, v);
}

i32 PopUpList::Find(const Val& v) const
{
	return FindIndex(items, v);
}

void PopUpList::SetScrollBarStyle(const ScrollBar::Style& s)
{
	sb_style = &s;
	if(popup)
		popup->ac.SetScrollBarStyle(*sb_style);
}

void PopUpList::SetLineCy(i32 cy)
{
	linecy = cy;
	if(popup)
		popup->ac.SetLineCy(linecy);
}

void PopUpList::SetDisplay(const Display& d)
{
	display = &d;
	if(popup)
		popup->ac.ColumnAt(0).SetDisplay(d);
}

void PopUpList::SetDisplay(i32 i, const Display& d)
{
	linedisplay.At(i, nullptr) = &d;
	if(popup)
		popup->ac.SetDisplay(i, 0, d);
}

const Display& PopUpList::GetDisplay(i32 i) const
{
	if(i >= 0 && i < linedisplay.GetCount() && linedisplay[i])
		return *linedisplay[i];
	return *display;
}

void PopUpList::SetConvert(const Convert& c)
{
	convert = &c;
	if(popup)
		popup->ac.ColumnAt(0).SetConvert(c);
}

void PopUpList::SetCursor(i32 i)
{
	cursor = i;
	if(popup)
		popup->ac.SetCursor(i);
}

i32 PopUpList::GetCursor() const
{
	return popup ? popup->ac.GetCursor() : cursor;
}

bool PopUpList::Key(i32 c)
{
	i32 q = GetCursor();
	q = q >= 0 ? q + 1 : 0;
	c = ToUpperAscii(c);
	for(i32 i = 0; i < GetCount(); i++) {
		i32 ii = (q + i) % GetCount();
		if(ToUpperAscii(*StdFormat(items[ii]).ToWTxt()) == c) {
			SetCursor(ii);
			return true;
		}
	}
	return false;
}

void PopUpList::PopupCancelMode() {
	if(popup && !inpopup)
		DoCancel();
}

void PopUpList::DoClose() {
	if(!inpopup && popup) {
		popup->closing = true; // prevent infinite recursion
		cursor = popup->ac.GetCursor();
		if(permanent) {
			popup->Close();
			popup->closing = false;
		}
		else
			popup.Clear();
	}
}

void PopUpList::PopupDeactivate() {
	if(popup) {
		Ctrl::IgnoreMouseClick();
		DoCancel();
	}
}

void PopUpList::PopupArrayCtrl::LeftUp(Point p, dword keyflags)
{
	ArrayCtrl::LeftUp(p, keyflags);
	list->DoSelect();
}

bool PopUpList::PopupArrayCtrl::Key(dword key, i32 n)
{
	switch(key) {
	case K_ENTER:
	case K_ALT_DOWN:
		list->DoSelect();
		return true;
	case K_ESCAPE:
		list->DoCancel();
		return true;
	case K_UP:
		if(!IsCursor()) {
			SetCursor(GetCount() - 1);
			return true;
		}
		break;
	case K_DOWN:
		if(!IsCursor()) {
			SetCursor(0);
			return true;
		}
		break;
	}
	return list->Key(key) || ArrayCtrl::Key(key, n);
}

PopUpList::Popup::Popup(PopUpList *list)
:	list(list)
{
	ac.list = list;
	ac.SetFrame(DropFrame());
	auto& col = ac.AddColumn().Accel();
	if(list->convert)
		col.SetConvert(*list->convert);
	col.SetDisplay(*list->display);
	ac.NoHeader();
	ac.HeaderTab(0).SetMargin(0);
	ac.MouseMoveCursor();
	ac.NoGrid();
	ac.AutoHideSb();
	ac.SetLineCy(Draw::GetStdFontCy());
	for(i32 i = 0; i < list->items.GetCount(); i++) {
		Val v = list->items[i];
		word w = i < list->lineinfo.GetCount() ? list->lineinfo[i] : 0x7fff;
		if(w & 0x8000)
			ac.AddSeparator();
		else
			ac.Add(v);
		w &= 0x7fff;
		if(w != 0x7fff)
			ac.SetLineCy(i, w);
	}
	ac.SetCursor(list->GetCursor());
	ac.CenterCursor();
}

ArrayCtrl& PopUpList::Permanent()
{
	if(!popup)
		popup.Create(this);
	permanent = true;
	return popup->ac;
}

void PopUpList::PopUp(Ctrl *owner, i32 x, i32 top, i32 bottom, i32 width) {
	if(inpopup)
		return;
	DoClose();
	inpopup++;
	if(!popup)
		popup.Create(this);
	i32 h = popup->ac.AddFrameSize(width, min(droplines * popup->ac.GetLineCy(), popup->ac.GetTotalCy())).cy;
	Rect rt = RectC(x, bottom, width, h);
	Rect area = Ctrl::GetWorkArea(Point(x, top));
	bool up = false;
	if(rt.bottom > area.bottom) {
		up = true;
		rt.top = top - h;
		rt.bottom = rt.top + h;
	}
	if(up) {
		popup->SetRect(Rect(rt.left, rt.bottom - 1, rt.right, rt.bottom));
		popup->Add(popup->ac.TopPos(0, rt.Height()).LeftPos(0, rt.Width()));
	}
	else {
		popup->SetRect(Rect(rt.left, rt.top, rt.right, rt.top + 1));
		popup->Add(popup->ac.BottomPos(0, rt.Height()).LeftPos(0, rt.Width()));
	}
	if(GUI_PopUpEffect()) {
		popup->ac.CenterCursor();
		popup->PopUp(owner, true, true, GUI_DropShadows());
		popup->ac.SetFocus();
		Ctrl::ProcessEvents();
		Animate(*popup, rt, GUIEFFECT_SLIDE);
	}
	if(popup && !popup->IsOpen()) {
		popup->SetRect(rt);
		if(!popup->IsOpen())
			popup->PopUp(owner, true, true, GUI_DropShadows());
		popup->ac.CenterCursor();
		popup->ac.SetFocus();
	}
	inpopup--;
}

void PopUpList::PopUp(Ctrl *owner, i32 width)
{
	Rect r = owner->GetScreenRect();
	if(width)
		r.right = r.left + width;
	PopUp(owner, r.left, r.top, r.bottom, r.Width());
}

void PopUpList::PopUp(Ctrl *owner)
{
	Rect r = owner->GetScreenRect();
	PopUp(owner, r.left, r.top, r.bottom, r.Width());
}

void PopUpList::DoSelect()
{
	DoClose();
	WhenSelect();
}

void PopUpList::DoCancel()
{
	DoClose();
	WhenCancel();
}

PopUpList::PopUpList() {
	droplines = 16;
	inpopup = 0;
	permanent = false;
	linecy = Draw::GetStdFontCy();
	display = &StdDisplay();
	convert = NULL;
}

PopUpList::~PopUpList() {}

}