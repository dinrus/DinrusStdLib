#include "CtrlCore.h"

#ifdef GUI_X11

namespace drx {

#define LTIMING(x)
#define LLOG(x)    // DLOG(x)

i32    gtk_antialias = -1;
i32    gtk_hinting = -1;
Txt gtk_hintstyle;
Txt gtk_rgba;

XftFont *CreateXftFont(Font font, i32 angle)
{
	LTIMING("CreateXftFont");
	XftFont *xftfont;
	double sina, cosa;
	i32 hg = abs(font.GetHeight());
	if(hg == 0) hg = max(GetStdFontCy(), 10);
	Txt face = font.GetFaceName();
	FcPattern *p = FcPatternCreate();
	FcPatternAddTxt(p, FC_FAMILY, (FcChar8*)~face);
	FcPatternAddInteger(p, FC_SLANT, font.IsItalic() ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
	FcPatternAddInteger(p, FC_PIXEL_SIZE, hg);
	FcPatternAddInteger(p, FC_WEIGHT, font.IsBold() ? FC_WEIGHT_BOLD : FC_WEIGHT_NORMAL);
	FcPatternAddBool(p, FC_MINSPACE, 1);
	if(angle) {
		FcMatrix mx;
		Draw::SinCos(angle, sina, cosa);
		mx.xx = cosa;
		mx.xy = -sina;
		mx.yx = sina;
		mx.yy = cosa;
		FcPatternAddMatrix(p, FC_MATRIX, &mx);
	}
	FcResult result;
	FcPattern *m = XftFontMatch(Xdisplay, Xscreenno, p, &result);
	if(font.IsNonAntiAliased() || gtk_antialias >= 0) {
		FcPatternDel(m, FC_ANTIALIAS);
		FcPatternAddBool(m, FC_ANTIALIAS,
		                 font.IsNonAntiAliased() ? FcFalse : gtk_antialias ? FcTrue : FcFalse);
	}
	if(gtk_hinting >= 0) {
		FcPatternDel(m, FC_HINTING);
		FcPatternAddBool(m, FC_HINTING, gtk_hinting);
	}
	tukk hs[] = { "hintnone", "hintslight", "hintmedium", "hintfull" };
	for(i32 i = 0; i < 4; i++)
		if(gtk_hintstyle == hs[i]) {
			FcPatternDel(m, FC_HINT_STYLE);
			FcPatternAddInteger(m, FC_HINT_STYLE, i);
		}
	tukk rgba[] = { "_", "rgb", "bgr", "vrgb", "vbgr" };
	for(i32 i = 0; i < __countof(rgba); i++)
		if(gtk_rgba == rgba[i]) {
			FcPatternDel(m, FC_RGBA);
			FcPatternAddInteger(m, FC_RGBA, i);
		}
	xftfont = XftFontOpenPattern(Xdisplay, m);
	FcPatternDestroy(p);
	return xftfont;
}

#define FONTCACHE 37

struct XftEntry {
	Font     font;
	i32      angle;
	XftFont *xftfont;
};

XftFont *GetXftFont(XftEntry *cache, Font fnt, i32 angle)
{
	XftEntry be;
	be = cache[0];
	for(i32 i = 0; i < FONTCACHE; i++) {
		XftEntry e = cache[i];
		if(i)
			cache[i] = be;
		if(e.font == fnt && e.angle == angle) {
			if(i)
				cache[0] = e;
			return e.xftfont;
		}
		be = e;
	}
	if(be.xftfont) {
		XftFontClose(Xdisplay, be.xftfont);
	}
	be.font = fnt;
	be.angle = angle;
	be.xftfont = CreateXftFont(fnt, angle);
	cache[0] = be;
	return be.xftfont;
}

XftFont *GetXftMetricFont(Font fnt, i32 angle)
{ // Using different global cache for metrics to avoid MT locking issues
	static XftEntry cache[FONTCACHE];
	ONCELOCK {
		for(i32 i = 0; i < FONTCACHE; i++)
			cache[i].font.Height(-30000);
	}
	Std(fnt);
	return GetXftFont(cache, fnt, angle);
}

CommonFontInfo XftGetFontInfoSys(Font font)
{
	CommonFontInfo fi;
	Txt path;
	XftFont *xftfont = GetXftMetricFont(font, 0);
	if(xftfont) {
		fi.ascent = (i16)xftfont->ascent;
		fi.descent = (i16)xftfont->descent;
		fi.external = 0;
		fi.internal = 0;
		fi.overhang = 0;
		fi.maxwidth = (i16)xftfont->max_advance_width;
		fi.avewidth = fi.maxwidth;
		fi.default_char = '?';
		fi.fixedpitch = font.GetFaceInfo() & Font::FIXEDPITCH;

		tuk fn = NULL;
		XftPatternGetTxt(xftfont->pattern, XFT_FILE, 0, &fn);
		if(fn && strlen(fn) < 250)
			strcpy(fi.path, fn);
	}
	return fi;
}

GlyphInfo XftGetGlyphInfoSys(Font font, i32 chr)
{
	wchar h = chr;
	XGlyphInfo info;
	XftTextExtents32(Xdisplay, GetXftMetricFont(font, 0), &h, 1, &info);
	GlyphInfo gi;
	gi.width = info.xOff;
	gi.lspc = -info.x;
	gi.rspc = info.xOff - info.width + info.x;
	return gi;
}

INITBLOCK {
	// it is probably not quite required as Xft is based on FC, but to be sure to have 
	// consistent metrics hook Xft metrics into Draw/FontFc.cpp

	extern CommonFontInfo (*GetFontInfoSysXft)(Font font);
	extern GlyphInfo (*GetGlyphInfoSysXft)(Font font, i32 chr);

	GetFontInfoSysXft = XftGetFontInfoSys;
	GetGlyphInfoSysXft = XftGetGlyphInfoSys;
}

void SystemDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
                            Color ink, i32 n, i32k *dx) {
	GuiLock __;
	LTIMING("DrawText");
	LLOG("DrawText " << ToUtf8(WTxt(text, n)) << " color:" << ink << " font:" << font);
	//TODO - X11 seems to crash when displaying too long strings (?)
	i32 ox = x + actual_offset.x;
	i32 oy = y + actual_offset.y;
	SetForeground(ink);
	XftColor c;
	c.color.red = ink.GetR() << 8;
	c.color.green = ink.GetG() << 8;
	c.color.blue = ink.GetB() << 8;
	c.color.alpha = 0xffff;
	c.pixel = GetXPixel(ink.GetR(), ink.GetG(), ink.GetB());

	static XftEntry cache[FONTCACHE];
	ONCELOCK {
		for(i32 i = 0; i < FONTCACHE; i++)
			cache[i].font.Height(-30000);
	}
	font.RealizeStd();
	XftFont *xftfont = GetXftFont(cache, font, angle);
	
	Size offset = Point(0, 0);
	double sina = 0, cosa = 1;
	i32 ascent = font.Info().GetAscent();
	if(angle) {
		SinCos(angle, sina, cosa);
		offset.cx = fround(ascent * sina);
		offset.cy = fround(ascent * cosa);
	}
	i32 hg = abs(font.GetHeight());
	if(hg == 0) hg = 10;
	i32 underline_thickness = max(hg / 20, 1);
	i32 underline_position = max(hg / 15, i32(font.Info().GetDescent() > 0));
	if(angle) {
		i32 xpos = 0;
		for(i32 i = 0; i < n; i++) {
			wchar h = text[i];
			XftDrawTxt16(xftdraw, &c, xftfont,
			                i32(ox + xpos * cosa + offset.cx),
			                i32(oy - xpos * sina + offset.cy),
			                (FcChar16 *)&h, 1);
			xpos += dx ? dx[i] : font[text[i]];
		}
		if(font.IsUnderline() || font.IsStrikeout()) {
			x += offset.cx;
			y += offset.cy;
			if(font.IsUnderline())
				DrawLine(
					i32(x + underline_position * sina),
					i32(y + underline_position * cosa),
					i32(x + xpos * cosa + underline_position * sina),
					i32(y - xpos * sina + underline_position * cosa),
					underline_thickness,
					ink
				);
			if(font.IsStrikeout()) {
				i32 p = 2 * ascent / 3;
				DrawLine(
					i32(x + p * sina),
					i32(y + p * cosa),
					i32(x + xpos * cosa + p * sina),
					i32(y - xpos * sina + p * cosa),
					underline_thickness,
					ink
				);
			}
		}
	}
	else {
	//	if(dx) {
			i32 xpos = ox;
			Buffer<XftCharSpec> ch(n);
			for(i32 i = 0; i < n; i++) {
				ch[i].ucs4 = text[i];
				ch[i].x = xpos;
				ch[i].y = oy + ascent;
				xpos += dx ? dx[i] : font[text[i]];
			}
			XftDrawCharSpec(xftdraw, &c, xftfont, ch, n);
	//	}
	//	else
	//		XftDrawTxt16(xftdraw, &c, xftfont, ox, oy + ascent,
	//		                (FcChar16 *)text, n);
		LLOG("XftColor: r=" << c.color.red << ", g=" << c.color.green << ", b=" << c.color.blue
			 << ", alpha=" << c.color.alpha << ", pixel=" << FormatIntHex(c.pixel));
		if(font.IsUnderline() || font.IsStrikeout()) {
			i32 cx;
			if(dx && n > 0) {
				cx = 0;
				Sum(cx, dx, dx + n - 1);
				cx += font[text[n - 1]];
			}
			else
				cx = GetTextSize(text, font, n).cx;
			if(font.IsUnderline())
				DrawRect(x, y + ascent + underline_position, cx, underline_thickness, ink);
			if(font.IsStrikeout())
				DrawRect(x, y + 2 * ascent / 3, cx, underline_thickness, ink);
		}
	}
}

}

#endif
