#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_COCOA

#include <drx/CtrlLib/CtrlLib.h>

NAMESPACE_UPP

void ChSysInit()
{
	CtrlImg::Reset();
	CtrlsImg::Reset();
	ChReset();
}

END_DRX_NAMESPACE

#endif
