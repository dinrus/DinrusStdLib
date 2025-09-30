#include "CtrlLib.h"

namespace drx {

#define  TFILE <drx/CtrlLib/CtrlLib.t>
#include <drx/Core/t.h>

void CtrlSetDefaultSkin(void (*_skin)());

extern Size (*extGetSmartTextSize)(tukk text, Font font, i32 cx);
extern void (*extDrawSmartText)(Draw& draw, i32 x, i32 y, i32 cx, tukk text, Font font,
                     Color ink, i32 accesskey, Color qtf_ink);

INITIALIZER(CtrlLib) {

		CtrlSetDefaultSkin(ChDarkSkin);

	extGetSmartTextSize = GetSmartTextSize;

	extDrawSmartText = DrawSmartText;
};

}