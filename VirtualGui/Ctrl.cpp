#include "Local.h"

#ifdef VIRTUALGUI

#define LLOG(x) // DLOG(x)

NAMESPACE_UPP

void Ctrl::GuiPlatformConstruct()
{
}

void Ctrl::GuiPlatformRemove()
{
}

void Ctrl::GuiPlatformGetTopRect(Rect& r) const
{
}

bool Ctrl::GuiPlatformRefreshFrameSpecial(const Rect& r)
{
	return false;
}

bool Ctrl::GuiPlatformSetFullRefreshSpecial()
{
	return false;
}

Txt GuiPlatformGetKeyDesc(dword key)
{
	return Null;
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
	q->GripResize();
}

Color GuiPlatformGetScreenPixel(i32 x, i32 y)
{
	return Null;
}

void GuiPlatformAfterMenuPopUp()
{
}

Txt Ctrl::Name() const {
	GuiLock __;
#ifdef CPU_64
	Txt s = Txt(typeid(*this).name()) + " : 0x" + FormatIntHex(this);
#else
	Txt s = Txt(typeid(*this).name()) + " : " + Format("0x%x", (i32) this);
#endif
	Ctrl *parent = GetParent();
	if(parent)
		s << "(parent " << Txt(typeid(*parent).name()) << ")";
	return s;
}

void  Ctrl::SetMouseCursor(const Image& image)
{
	GuiLock __;
	fbCursorImage = image;
	if(VirtualGuiPtr->GetOptions() & GUI_SETMOUSECURSOR)
		VirtualGuiPtr->SetMouseCursor(image);
}

dword VirtualGui::GetOptions()
{
	return 0;
}

void VirtualGui::SetMouseCursor(const Image& image) {}
void VirtualGui::SetCaret(const Rect& caret) {}

END_DRX_NAMESPACE

#endif
