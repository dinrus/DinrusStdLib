#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_COCOA

#define LLOG(x) // DLOG(x)

NAMESPACE_UPP

void Ctrl::GuiPlatformConstruct()
{
}

void Ctrl::GuiPlatformDestruct()
{
}

void Ctrl::GuiPlatformRemove()
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
}

Color GuiPlatformGetScreenPixel(i32 x, i32 y)
{
	return Black;
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
	if(IsChild()) {
		Ctrl *parent = GetParent();
		s << "(parent " << Txt(typeid(*parent).name()) << ")";
	}
	return s;
}

void Ctrl::InstallPanicBox()
{
}

END_DRX_NAMESPACE

#endif
