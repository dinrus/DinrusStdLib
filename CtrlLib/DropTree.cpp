#include "CtrlLib.h"

namespace drx {

CH_VALUE(TreeDropEdge, ChBorder(BlackBorder()));

CtrlFrame& TreeDropFrame()
{
	static LookFrame m(TreeDropEdge);
	return m;
}

PopUpTree::PopUpTree() {
	SetFrame(TreeDropFrame());
	Accel();
	MouseMoveCursor();
	NoPopUpEx();
	SetDropLines(16);
	open = autosize = false;
	showpos = Null;
	WhenOpen = WhenClose = OTVET(OpenClose);
}

PopUpTree::~PopUpTree() {}

void PopUpTree::CancelMode() {
	if(open) {
		DoClose();
		WhenCancel();
	}
	TreeCtrl::CancelMode();
}

void PopUpTree::DoClose() {
	open = false;
	Ctrl::Close();
}

void PopUpTree::Deactivate() {
	if(open) {
		DoClose();
		IgnoreMouseClick();
		WhenCancel();
	}
}

void PopUpTree::Select() {
	if(IsCursor() && !GetData().IsVoid()) {
		DoClose();
		WhenSelect();
	}
}

bool PopUpTree::Key(dword key, i32 n) {
	switch(key) {
	case K_ENTER:
	case K_ALT_DOWN:
		DoClose();
		WhenSelect();
		return true;
	case K_ESCAPE:
		DoClose();
		WhenCancel();
		return true;
	}
	return TreeCtrl::Key(key, n);
}

void PopUpTree::PopUp(Ctrl *owner, i32 x, i32 top, i32 bottom, i32 width) {
	DoClose();
	Rect area = Ctrl::GetWorkArea();
	i32 mh = min(maxheight, area.bottom - bottom);
	i32 h = AddFrameSize(width, maxheight).cy;
	showpos.x = x;
	showpos.y = bottom;
	showwidth = width;
	up = false;
	if(showpos.y + h > area.bottom) {
		up = true;
		showpos.y = top;
	}
	open = false;

	i32 ht = AddFrameSize(width, min(mh, autosize ? GetTreeSize().cy : INT_MAX)).cy;
	Rect rt = RectC(showpos.x, showpos.y - (up ? ht : 0), showwidth, ht);
	if(GUI_PopUpEffect()) {
		bool vis = sb.x.IsShown();
		bool ah = sb.x.IsAutoHide();
		sb.AutoHide(false);
		sb.Hide();
		sPaintRedirectCtrl pb;
		pb.ctrl = this;
		if(up) {
			SetRect(Rect(rt.left, rt.bottom - 1, rt.right, rt.bottom));
			Ctrl::Add(pb.TopPos(0, rt.Height()).LeftPos(0, rt.Width()));
		}
		else {
			SetRect(Rect(rt.left, rt.top, rt.right, rt.top + 1));
			Ctrl::Add(pb.BottomPos(0, rt.Height()).LeftPos(0, rt.Width()));
		}
		CenterCursor();
		Ctrl::PopUp(owner, true, true, GUI_DropShadows());
		SetFocus();
		Ctrl::ProcessEvents();
		Animate(*this, rt, GUIEFFECT_SLIDE);
		Ctrl::Remove();
		sb.Show(vis);
		sb.AutoHide(ah);
		pb.Remove();
		open = true;
	}

	CenterCursor();
	SetRect(rt);
	if(!open)
		Ctrl::PopUp(owner, true, true, GUI_DropShadows());
	SetFocus();
	open = true;
}

void PopUpTree::OpenClose(i32)
{
	if(autosize) {
		SyncTree();
		Rect area = Ctrl::GetWorkArea();
		i32 mh = min(maxheight, area.bottom - showpos.y);
		i32 ht = AddFrameSize(showwidth, min(mh, GetTreeSize().cy)).cy;
		SetRect(RectC(showpos.x, showpos.y - (up ? ht : 0), showwidth, ht));
	}
}

void PopUpTree::PopUp(Ctrl *owner, i32 width)
{
	Rect r = owner->GetScreenRect();
	r.right = r.left + width;
	PopUp(owner, r.left, r.top, r.bottom, width);
	if(IsNull(showpos))
		showpos = r.TopLeft();
	OpenClose(0);
}

void PopUpTree::PopUp(Ctrl *owner)
{
	Rect r = owner->GetScreenRect();
	PopUp(owner, r.left, r.top, r.bottom, r.Width());
}

void DropTree::Sync() {
	Image icon;
	if(tree.IsCursor())
		icon = tree.GetNode(tree.GetCursor()).image;
	icond.Set(valuedisplay ? *valuedisplay : tree.GetDisplay(tree.GetCursor()), icon);
	MultiButton::SetDisplay(icond);
	Set(tree.GetVal());
}

bool DropTree::Key(dword k, i32) {
	if(IsReadOnly()) return false;
	if(k == K_ALT_DOWN) {
		Drop();
		return true;
	}
	return false;
}

void DropTree::Drop() {
	if(IsReadOnly()) return;
	if(dropfocus)
		SetFocus();
	WhenDrop();
	tree.PopUp(this, GetRect().GetWidth());
}

void DropTree::Select() {
	if(dropfocus)
		SetFocus();
	Sync();
	UpdateAction();
}

void DropTree::Cancel() {
	if(dropfocus)
		SetFocus();
	Sync();
}

void DropTree::Clear() {
	tree.Clear();
	tree <<= Null;
	Sync();
	Update();
}

void DropTree::SetData(const Val& data)
{
	if(tree.Get() != data) {
		tree <<= data;
		Update();
		Sync();
	}
}

Val DropTree::GetData() const
{
	return notnull && IsNull(tree.Get()) ? NotNullError() : tree.Get();
}

DropTree& DropTree::ValDisplay(const Display& d)
{
	valuedisplay = &d;
	Sync();
	return *this;
}

DropTree::DropTree()
{
	valuedisplay = NULL;
	dropfocus = false;
	notnull = false;
	AddButton().Main().WhenPush = OTVET(Drop);
	NoInitFocus();
	tree.WhenSelect = OTVET(Select);
	tree.WhenCancel = OTVET(Cancel);
	dropwidth = 0;
}

};
