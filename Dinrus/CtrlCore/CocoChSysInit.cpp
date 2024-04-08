#include <Dinrus/CtrlCore/CtrlCore.h>

#ifdef GUI_COCOA

#include <Dinrus/CtrlLib/CtrlLib.h>

NAMESPACE_UPP

void ChSysInit()
{
	CtrlImg::Reset();
	CtrlsImg::Reset();
	ChReset();
}

END_UPP_NAMESPACE

#endif
