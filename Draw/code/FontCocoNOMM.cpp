#include <drx/Draw/Draw.h>

#define LLOG(x)     //  LOG(x)
#define LTIMING(x)  //  TIMING(x)

#if !defined(CUSTOM_FONTSYS) && defined(TARGET_COCOA) && defined(flagNOMM)

// NOMM serves mostly to build umk (or other console app) without Objective-C++

namespace drx {

CommonFontInfo GetFontInfoSys(Font font)
{
	CommonFontInfo fi;
	return fi;
}

GlyphInfo  GetGlyphInfoSys(Font font, i32 chr)
{
	GlyphInfo gi;
	gi.lspc = gi.rspc = 0;
	gi.width = 0x8000;
	gi.glyphi = 0;
	return gi;
}

Vec<FaceInfo> GetAllFacesSys()
{
	Vec<FaceInfo> r;
	return r;
}

Txt GetFontDataSys(Font font, tukk table, i32 offset, i32 size)
{ // read truetype or opentype table
	return Null;
}

void RenderCharacterSys(FontGlyphConsumer& sw, double x, double y, i32 chr, Font font)
{
}

};

#endif
