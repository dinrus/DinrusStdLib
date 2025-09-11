#include "CtrlCore.h"

namespace drx {

#define  TFILE <drx/CtrlCore/CtrlCore.t>
#include <drx/Core/t.h>

static Image sRenderGlyph(i32 cx, i32 x, Font font, i32 chr, i32 py, i32 pcy, Color fg, Color bg)
{
	ImageDraw iw(cx, pcy);
	iw.DrawRect(0, 0, cx, pcy, bg);
	iw.DrawText(x, -py, WTxt(chr, 1), font, fg);
	return iw;
}

void SetRenderGlyph(Image (*f)(i32 cx, i32 x, Font font, i32 chr, i32 py, i32 pcy, Color fg, Color bg));

INITIALIZER(CtrlCore) {
	SetRenderGlyph(sRenderGlyph);
}

}
