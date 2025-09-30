#include "CtrlCore.h"

#ifdef GUI_WIN

#define LLOG(x)  // DLOG(x)

namespace drx {

void Ctrl::GuiPlatformConstruct()
{
	activex = false;
	isdhctrl = false;
}

void Ctrl::GuiPlatformDestruct()
{
}

void Ctrl::GuiPlatformRemove()
{
}

void Ctrl::GuiPlatformGetTopRect(Rect& r) const
{
	if(activex)
		r = GetWndScreenRect();
}

bool Ctrl::GuiPlatformRefreshFrameSpecial(const Rect& r)
{
	if(isdhctrl) {
		InvalidateRect(((DHCtrl *)this)->GetHWND(), r, false);
		return true;
	}
	return false;
}

bool Ctrl::GuiPlatformSetFullRefreshSpecial()
{
	return isdhctrl;
}

void Ctrl::GuiPlatformSelection(PasteClip&)
{
}

void GuiPlatformAdjustDragImage(ImageBuffer&)
{
}

bool GuiPlatformHasSizeGrip()
{
	return true;
}

void GuiPlatformGripResize(TopWindow *q)
{
	LLOG("GuiPlatformGripResize " << Name(q));
	HWND hwnd = q->GetHWND();
	Point p = GetMousePos() - q->GetRect().TopLeft();
	if(hwnd) {
		::SendMessage(hwnd, WM_SYSCOMMAND, 0xf008, MAKELONG(p.x, p.y));
		::SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELONG(p.x, p.y));
	}
}

Color GuiPlatformGetScreenPixel(i32 x, i32 y)
{
	HDC sdc = GetWindowDC(0);
	Color c = Color::FromCR(GetPixel(sdc, x, y));
	ReleaseDC(0, sdc);
	return c;
}

void GuiPlatformAfterMenuPopUp()
{
}

Txt Ctrl::Name() const {
	GuiLock __;
	Txt s = Name0();
	if(!IsChild())
		s << Format(" (hwnd 0x%x)", (i32)(intptr_t) GetHWND());
	return s;
}

}

#endif
