#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_COCOA

NAMESPACE_UPP

#define LLOG(x)  // LOG(x)

TopWindow& TopWindow::FullScreen(bool b)
{
	return *this;
}

TopWindow& TopWindow::TopMost(bool b, bool stay_top)
{
	GuiLock __;
	return *this;
}

bool TopWindow::IsTopMost() const
{
	return true;
}

void TopWindow::GuiPlatformConstruct()
{
}
	
END_DRX_NAMESPACE

#endif
