#include "CtrlCore.h"

#ifdef GUI_WIN

namespace drx {

SystemDraw&  ScreenInfo();

bool ScreenInPaletteMode()
{
	return ScreenInfo().PaletteMode();
}

}

#endif
