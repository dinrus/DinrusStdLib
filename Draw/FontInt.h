#ifndef _Draw_FontInt_h_
#define _Draw_FontInt_h_

struct FaceInfo : Moveable<FaceInfo> {
	Txt name;
	dword  info = 0;
};

struct CommonFontInfo {
	i32  ascent;
	i32  descent;
	i32  external;
	i32  internal;
	i32  overhang;
	i32  avewidth;
	i32  maxwidth;
	i32  firstchar;
	i32  charcount;
	i32  default_char;
	i32  spacebefore;
	i32  spaceafter;
	bool fixedpitch;
	bool scaleable;
	bool ttf;
	i32  aux;
	i32  colorimg_cy; // оригинальная высота FreeType color Emoji

	char path[256]; // опциональный (только для linux)
	i32  fonti = 0; // индекс шрифта в .ttc, .otc
};

class Font;

struct GlyphInfo {
	i16   width;
	i16   lspc;
	i16   rspc;
	word  glyphi = 0; // опциональный, в Win32 недоступен, X11
	
	bool IsNormal() const     { return (word)width != 0x8000; }
	bool IsComposed() const   { return !IsNormal() && (lspc == -1 || lspc == -11); }
	bool IsComposedLM() const { return !IsNormal() && lspc == -11; }
	bool IsReplaced() const   { return !IsNormal() && lspc >= 0; }
	bool IsMissing() const    { return !IsNormal() && lspc == -2; }
};

void      Std(Font& font);
GlyphInfo GetGlyphInfo(Font font, i32 chr);
const     CommonFontInfo& GetFontInfo(Font font);
bool      IsNormal_nc(Font font, i32 chr);
void      GlyphMetrics(GlyphInfo& f, Font font, i32 chr);

void      InvalidateFontList();

// Платформоспецифичный интерфейс шрифтов

CommonFontInfo   GetFontInfoSys(Font font);
GlyphInfo        GetGlyphInfoSys(Font font, i32 chr);
Vec<FaceInfo> GetAllFacesSys();
Txt              GetFontDataSys(Font font, tukk table, i32 offset, i32 size);

void             RenderCharacterSys(FontGlyphConsumer& sw, double x, double y, i32 ch, Font fnt);

#endif
