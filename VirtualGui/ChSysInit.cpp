#include "Local.h"

#ifdef VIRTUALGUI

NAMESPACE_UPP

void ChSysInit()
{
	ChReset();
}

void ChHostSkin()
{
	ChSysInit();
}

bool IsSystemThemeDark()
{
	return false;
}

END_DRX_NAMESPACE

#endif
