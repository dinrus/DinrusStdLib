#include "CtrlLib.h"

namespace drx {

i32                  VirtualButtons::ButtonCount() const        { return 0; }
Rect                 VirtualButtons::ButtonRect(i32 i) const    { return Null; }
const Button::Style& VirtualButtons::ButtonStyle(i32 i) const   { return Button::StyleEdge(); }
Image                VirtualButtons::ButtonImage(i32 i) const   { return Null; }
bool                 VirtualButtons::ButtonEnabled(i32 i) const { return true; }
bool                 VirtualButtons::ButtonMono(i32 i) const    { return false; }

void VirtualButtons::ButtonPush(i32 i)   {}
void VirtualButtons::ButtonRepeat(i32 i) {}
void VirtualButtons::ButtonAction(i32 i) {}

i32 VirtualButtons::FindButton(Point p) const
{
	for(i32 i = 0; i < ButtonCount(); i++)
		if(ButtonRect(i).Contains(p))
			return i;
	return -1;
}

i32 VirtualButtons::ButtonVisualState(Ctrl *ctrl, i32 i)
{
	if(ctrl->HasCapture() && !buttons_capture)
		return CTRL_NORMAL;
	if(ButtonEnabled(i)) {
		if(i == pushi)
			return CTRL_PRESSED;
		if(ctrl->HasMouseIn(ButtonRect(i)))
			return CTRL_HOT;
		return CTRL_NORMAL;
	}
	return CTRL_DISABLED;
}

void VirtualButtons::PaintButtons(Draw& w, Ctrl *ctrl)
{
	for(i32 i = 0; i < ButtonCount(); i++) {
		Rect r = ButtonRect(i);
		if(r.GetWidth() > 0 && r.GetHeight() > 0)
			Button::PaintButton(w, ButtonRect(i), ButtonStyle(i), ButtonVisualState(ctrl, i), false,
			                    Txt(), StdFont(), ButtonImage(i),
			                    ButtonMono(i), 0, false, !ButtonEnabled(i));
	}
}

void VirtualButtons::EndPush(Ctrl *ctrl)
{
	i32 i = pushi;
	pushi = -1;
	RefreshButton(ctrl, i);
}

void VirtualButtons::RefreshButton(Ctrl *ctrl, i32 i)
{
	if(i >= 0)
		ctrl->Refresh(ButtonRect(i));
}

void VirtualButtons::ButtonsCancelMode()
{
	pushi = -1;
	mi = -1;
	buttons_capture = false;
}

bool VirtualButtons::ButtonsMouseEvent(Ctrl *ctrl, i32 event, Point p)
{
	if(ctrl->HasCapture() && !buttons_capture)
		return false;
	i32 i = event == Ctrl::MOUSELEAVE ? -1 : FindButton(p);
	if(i != mi) {
		RefreshButton(ctrl, mi);
		RefreshButton(ctrl, mi = i);
	}
	switch(event) {
	case Ctrl::LEFTDOWN:
	case Ctrl::LEFTDOUBLE:
	case Ctrl::LEFTTRIPLE:
		pushi = i;
		if(pushi >= 0) {
			ButtonPush(i);
			ctrl->SetCapture();
			buttons_capture = true;
		}
		else
			EndPush(ctrl);
		RefreshButton(ctrl, pushi);
		break;
	case Ctrl::MOUSEMOVE:
		if(ctrl->HasCapture() && i != pushi) {
			RefreshButton(ctrl, pushi);
			pushi = i;
		}
		break;
	case Ctrl::LEFTREPEAT:
		if(i >= 0)
			ButtonRepeat(i);
		break;
	case Ctrl::LEFTUP:
		i32 ii = pushi;
		if(ii >= 0) {
			EndPush(ctrl);
			ctrl->ReleaseCapture();
			buttons_capture = false;
			ButtonAction(ii);
		}
		break;
	}
	return i >= 0;
}

};