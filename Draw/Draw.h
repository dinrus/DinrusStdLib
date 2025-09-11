#ifndef DRAW_H
#define DRAW_H

#define SYSTEMDRAW 1

#include <drx/Core/Core.h>

#ifdef flagCFONTS
#define CUSTOM_FONTSYS
#endif

namespace drx {

class Drawing;
class Draw;
class Painting;
class SystemDraw;
class ImageDraw;

#include "Image.h"

i32k FONT_V = 40;

struct FontGlyphConsumer {
	virtual void Move(Pointf p) = 0;
	virtual void Line(Pointf p) = 0;
	virtual void Quadratic(Pointf p1, Pointf p2) = 0;
	virtual void Cubic(Pointf p1, Pointf p2, Pointf p3) = 0;
	virtual void Close() = 0;
};

#include "FontInt.h"

class FontInfo;


inline
bool PreferColorEmoji(i32 c)
{ // для этих кодпойнтов предпочитается замена color emoji, даже если глиф есть в шрифте
	return (c >= 0x2600 && c <= 0x27ef) || (c >= 0x1f004 && c <= 0x1f251) || (c >= 0x1f300 && c <= 0x1faf6);
}

class Font : public ValType<Font, FONT_V, Moveable<Font> >{
	union {
		z64 data; //данные
		struct {
			word  face;  //фас
			word  flags; //флаги
			i16 height; //высота
			i16 width; //ширина
		} v;
	};

	enum {
		FONT_BOLD = 0x8000, //полужирный
		FONT_ITALIC = 0x4000, //курсив
		FONT_UNDERLINE = 0x2000, //подчёркнутый
		FONT_STRIKEOUT = 0x1000, //зачёркнутый
		FONT_NON_ANTI_ALIASED = 0x800,//несглаженный
		FONT_TRUE_TYPE_ONLY = 0x400 //только трютайп
	};

	static Font AStdFont;
	static Size StdFontSize;
	static bool std_font_override;

	static void SetStdFont0(Font font);
	static Vec<FaceInfo>& FaceList();
	static void SyncStdFont();
	static void InitStdFont();

	const CommonFontInfo& Fi() const;

	friend void   sInitFonts();
	friend Txt GetFontDataSys(Font font, tukk table, i32 offset, i32 size);

public:
	enum {
		FIXEDPITCH  = 0x0001,
		SCALEABLE   = 0x0002, //масштабируемый
		TTF         = 0x0004, //трютайпный
		SPECIAL     = 0x0010, //особый
		SERIFSTYLE  = 0x0020,
		SCRIPTSTYLE = 0x0040,
		COLORIMG    = 0x0080, // цветной битмап-шрифт freetype (emojis)
	};

	static i32    GetFaceCount();
	static Txt GetFaceName(i32 index);
	static i32    FindFaceNameIndex(const Txt& name);
	static dword  GetFaceInfo(i32 index);
	static void   SetFace(i32 index, const Txt& name, dword info);
	static void   SetFace(i32 index, const Txt& name);

	static void   SetDefaultFont(Font font);
	static void   SetStdFont(Font font);
	static Font   GetStdFont();
	static Size   GetStdFontSize();

	enum {
		STDFONT,
		SERIF,
		SANSSERIF,
		MONOSPACE,
	#ifdef TARGET_WIN32
		SYMBOL, // deprecated
		WINGDINGS, // deprecated
		TAHOMA, // deprecated
	#endif
		OTHER, // deprecated

	// Backward compatibility:
		ROMAN = SERIF,
		ARIAL = SANSSERIF,
		COURIER = MONOSPACE,
		SCREEN_SERIF = SERIF,
		SCREEN_SANS = SANSSERIF,
		SCREEN_FIXED = MONOSPACE,
	};

	i32    GetFace() const          { return v.face; }
	i32    GetHeight() const;
	i32    GetWidth() const         { return v.width; }
	bool   IsBold() const           { return v.flags & FONT_BOLD; }
	bool   IsItalic() const         { return v.flags & FONT_ITALIC; }
	bool   IsUnderline() const      { return v.flags & FONT_UNDERLINE; }
	bool   IsStrikeout() const      { return v.flags & FONT_STRIKEOUT; }
	bool   IsNonAntiAliased() const { return v.flags & FONT_NON_ANTI_ALIASED; } // deprecated
	bool   IsTrueTypeOnly() const   { return v.flags & FONT_TRUE_TYPE_ONLY; } // deprecated
	Txt    GetFaceName() const;
	Txt    GetFaceNameStd() const;
	dword  GetFaceInfo() const;
	z64    AsInt64() const          { return data; }

	void   RealizeStd();

	Font& Face(i32 n)               { v.face = n; return *this; }
	Font& Height(i32 n)             { v.height = n; return *this; }
	Font& Width(i32 n)              { v.width = n; return *this; }
	Font& Bold()                    { v.flags |= FONT_BOLD; return *this; }
	Font& NoBold()                  { v.flags &= ~FONT_BOLD; return *this; }
	Font& Bold(bool b)              { return b ? Bold() : NoBold(); }
	Font& Italic()                  { v.flags |= FONT_ITALIC; return *this; }
	Font& NoItalic()                { v.flags &= ~FONT_ITALIC; return *this; }
	Font& Italic(bool b)            { return b ? Italic() : NoItalic(); }
	Font& Underline()               { v.flags |= FONT_UNDERLINE; return *this; }
	Font& NoUnderline()             { v.flags &= ~FONT_UNDERLINE; return *this; }
	Font& Underline(bool b)         { return b ? Underline() : NoUnderline(); }
	Font& Strikeout()               { v.flags |= FONT_STRIKEOUT; return *this; }
	Font& NoStrikeout()             { v.flags &= ~FONT_STRIKEOUT; return *this; }
	Font& Strikeout(bool b)         { return b ? Strikeout() : NoStrikeout(); }
	Font& NonAntiAliased()          { v.flags |= FONT_NON_ANTI_ALIASED; return *this; }
	Font& NoNonAntiAliased()        { v.flags &= ~FONT_NON_ANTI_ALIASED; return *this; } // deprecated
	Font& NonAntiAliased(bool b)    { return b ? NonAntiAliased() : NoNonAntiAliased(); } // deprecated
	Font& TrueTypeOnly()            { v.flags |= FONT_TRUE_TYPE_ONLY; return *this; } // deprecated
	Font& NoTrueTypeOnly()          { v.flags &= ~FONT_TRUE_TYPE_ONLY; return *this; } // deprecated
	Font& TrueTypeOnly(bool b)      { return b ? TrueTypeOnly() : NoTrueTypeOnly(); } // deprecated

	Font& FaceName(const Txt& name);

	Font  operator()() const        { return *this; }
	Font  operator()(i32 n) const   { return Font(*this).Height(n); }

	i32   GetAscent() const                  { return Fi().ascent; }
	i32   GetDescent() const                 { return Fi().descent; }
	i32   GetCy() const                      { return GetAscent() + GetDescent(); }
	i32   GetExternal() const                { return Fi().external; }
	i32   GetInternal() const                { return Fi().internal; }
	i32   GetLineHeight() const              { return GetCy() + GetExternal(); }
	i32   GetOverhang() const                { return Fi().overhang; }
	i32   GetAveWidth() const                { return Fi().avewidth; }
	i32   GetMaxWidth() const                { return Fi().maxwidth; }
	i32   GetMonoWidth() const               { return max(GetWidth('M'), GetWidth('W')); }
	bool  IsNormal(i32 ch) const;
	bool  IsComposed(i32 ch) const;
	bool  IsReplaced(i32 ch) const;
	bool  IsMissing(i32 ch) const;
	i32   HasChar(i32 ch) const;
	i32   GetWidth(i32 c) const;
	i32   operator[](i32 c) const            { return GetWidth(c); }
	i32   GetLeftSpace(i32 c) const;
	i32   GetRightSpace(i32 c) const;
	bool  IsFixedPitch() const               { return Fi().fixedpitch; }
	bool  IsScaleable() const                { return Fi().scaleable; }
	bool  IsSpecial() const                  { return GetFaceInfo() & SPECIAL; }
	bool  IsTrueType() const                 { return Fi().ttf; }
	bool  IsSerif() const                    { return GetFaceInfo() & SERIFSTYLE; }
	bool  IsScript() const                   { return GetFaceInfo() & SCRIPTSTYLE; }

	Txt GetTextFlags() const;
	void   ParseTextFlags(tukk s);

	Txt GetData(tukk table = NULL, i32 offset = 0, i32 size = INT_MAX) const;

	void   Render(FontGlyphConsumer& sw, double x, double y, i32 ch) const;

	void  Serialize(Stream& s);
	void  Jsonize(JsonIO& jio);
	void  Xmlize(XmlIO& xio);

	bool  operator==(Font f) const  { return v.face == f.v.face && v.flags == f.v.flags &&
	                                        v.width == f.v.width && v.height == f.v.height; }
	bool  operator!=(Font f) const  { return !operator==(f); }

	hash_t GetHashVal() const     { return CombineHash(v.width, v.flags, v.height, v.face); }
	bool   IsNullInstance() const   { return v.face == 0xffff; }
	void   SetNull()                { v.face = 0xffff; v.height = v.width = 0; v.flags = 0; }
	Font()                          { v.height = v.width = 0; v.face = v.flags = 0; }
	Font(i32 face, i32 height)      { v.face = face; v.height = height; v.flags = 0; v.width = 0; }
	Font(const Nuller&)             { SetNull(); }

	operator Val() const          { return RichToVal(*this); }
	Font(const Val& q)            { *this = q.Get<Font>(); }

	static Font FromInt64(z64 q)  { Font fnt; fnt.data = q; return fnt; }

// BW compatibility
	FontInfo Info() const;
};


//BW compatibility
class FontInfo { // Obsolete!
	Font font;
	friend class Font;
public:
	i32    GetAscent() const                  { return font.GetAscent(); }
	i32    GetDescent() const                 { return font.GetDescent(); }
	i32    GetExternal() const                { return font.GetExternal(); }
	i32    GetInternal() const                { return font.GetInternal(); }
	i32    GetHeight() const                  { return font.GetCy(); }
	i32    GetLineHeight() const              { return font.GetLineHeight(); }
	i32    GetOverhang() const                { return font.GetOverhang(); }
	i32    GetAveWidth() const                { return font.GetAveWidth(); }
	i32    GetMaxWidth() const                { return font.GetMaxWidth(); }
	i32    HasChar(i32 c) const               { return font.HasChar(c); }
	i32    GetWidth(i32 c) const              { return font.GetWidth(c); }
	i32    operator[](i32 c) const            { return GetWidth(c); }
	i32    GetLeftSpace(i32 c) const          { return font.GetLeftSpace(c); }
	i32    GetRightSpace(i32 c) const         { return font.GetRightSpace(c); }
	bool   IsFixedPitch() const               { return font.IsFixedPitch(); }
	bool   IsScaleable() const                { return font.IsScaleable(); }
	i32    GetFontHeight() const              { return font.GetHeight(); }
	Font   GetFont() const                    { return font; }
};

struct ComposedGlyph {
	wchar  basic_char;
	Point  mark_pos;
	wchar  mark_char;
	Font   mark_font;
};

bool Compose(Font fnt, i32 chr, ComposedGlyph& cs);

template<>
Txt AsTxt(const Font& f);

inline void SetStdFont(Font font)                   { Font::SetStdFont(font); }
inline Font GetStdFont()                            { return Font::GetStdFont(); }
inline Size GetStdFontSize()                        { return Font::GetStdFontSize(); } // deprecated
inline i32  GetStdFontCy()                          { return GetStdFontSize().cy; }

Font StdFont();

inline Font StdFont(i32 h)                          { return StdFont().Height(h); }

inline Font Serif(i32 n = -32000) { return Font(Font::SCREEN_SERIF, n); }
inline Font SansSerif(i32 n = -32000) { return Font(Font::SCREEN_SANS, n); }
inline Font Monospace(i32 n = -32000) { return Font(Font::SCREEN_FIXED, n); }

inline Font Roman(i32 n = -32000) { return Font(Font::SCREEN_SERIF, n); } // deprecated
inline Font Arial(i32 n = -32000) { return Font(Font::SCREEN_SANS, n); } // deprecated
inline Font Courier(i32 n = -32000) { return Font(Font::SCREEN_FIXED, n); } // deprecated

inline Font ScreenSerif(i32 n = -32000) { return Font(Font::SCREEN_SERIF, n); } // deprecated
inline Font ScreenSans(i32 n = -32000) { return Font(Font::SCREEN_SANS, n); } // deprecated
inline Font ScreenFixed(i32 n = -32000) { return Font(Font::SCREEN_FIXED, n); } // deprecated

#ifdef TARGET_WIN32 // backward comaptibility
inline Font Tahoma(i32 n = -32000) { return Font(Font::TAHOMA, n); }
#endif

Size GetTextSize(wtukk text, Font font, i32 n = -1);
Size GetTextSize(const WTxt& text, Font font);
Size GetTextSize(tukk text, u8 charset, Font font, i32 n = -1);
Size GetTextSize(tukk text, Font font, i32 n = -1);
Size GetTextSize(const Txt& text, Font font);

enum {
	PEN_NULL = -1,
	PEN_SOLID = -2,
	PEN_DASH = -3,
#ifndef TARGET_WINCE
	PEN_DOT = -4,
	PEN_DASHDOT = -5,
	PEN_DASHDOTDOT = -6,
#endif
};

class Image;

Color SBlack();
Color SGray();
Color SLtGray();
Color SWhiteGray();
Color SWhite();
Color SRed();
Color SGreen();
Color SBrown();
Color SBlue();
Color SMagenta();
Color SCyan();
Color SYellow();
Color SLtRed();
Color SLtGreen();
Color SLtYellow();
Color SLtBlue();
Color SLtMagenta();
Color SLtCyan();

Color SColorPaper();
Color SColorText();
Color SColorFace();
Color SColorHighlight();
Color SColorHighlightText();
Color SColorMenu();
Color SColorMenuText();
Color SColorInfo();
Color SColorInfoText();
Color SColorMark();
Color SColorMenuMark();
Color SColorDisabled();
Color SColorLight();
Color SColorLabel();
Color SColorShadow();

Color SColorLtFace();
Color SColorDkShadow();

void SBlack_Write(Color c);
void SGray_Write(Color c);
void SLtGray_Write(Color c);
void SWhiteGray_Write(Color c);
void SWhite_Write(Color c);
void SRed_Write(Color c);
void SGreen_Write(Color c);
void SBrown_Write(Color c);
void SBlue_Write(Color c);
void SMagenta_Write(Color c);
void SCyan_Write(Color c);
void SYellow_Write(Color c);
void SLtRed_Write(Color c);
void SLtGreen_Write(Color c);
void SLtYellow_Write(Color c);
void SLtBlue_Write(Color c);
void SLtMagenta_Write(Color c);
void SLtCyan_Write(Color c);

void SColorPaper_Write(Color c);
void SColorText_Write(Color c);
void SColorHighlight_Write(Color c);
void SColorHighlightText_Write(Color c);
void SColorMenu_Write(Color c);
void SColorMenuText_Write(Color c);
void SColorInfo_Write(Color c);
void SColorInfoText_Write(Color c);
void SColorMark_Write(Color c);
void SColorMenuMark_Write(Color c);
void SColorDisabled_Write(Color c);
void SColorLight_Write(Color c);
void SColorFace_Write(Color c);
void SColorLabel_Write(Color c);
void SColorShadow_Write(Color c);

void SColorLtFace_Write(Color c);
void SColorDkShadow_Write(Color c);

inline Color InvertColor() { return Color::Special(255); } // Special color that with DrawRect actually inverts the rectangle
inline Color DefaultInk() { return Color::Special(254); } // SColorText on screen, Black on other outputs

extern bool dark_theme__;

inline bool  IsDarkTheme()           { return dark_theme__; }

inline Color AdjustIfDark(Color c)   { return IsDarkTheme() ? DarkThemeCached(c) : c; }

Drawing AsDrawing(const Painting& pw);

class Painting : public ValType<Painting, 48, Moveable<Painting> > {
	Txt        cmd;
	ValArray   data;
	Sizef      size;

	friend class PaintingPainter;
	friend class Painter;

public:
	Sizef   GetSize() const                     { return size; }

	void    Clear()                             { size = Null; data.Clear(); cmd.Clear(); }
	void    Serialize(Stream& s)                { s % cmd % data % size; }
	void    Xmlize(XmlIO& xio)                  { XmlizeBySerialize(xio, *this); }
	void    Jsonize(JsonIO& jio)                { JsonizeBySerialize(jio, *this); }
	bool    IsNullInstance() const              { return cmd.IsEmpty(); }
	void    SetNull()                           { size = Null; }
	bool    operator==(const Painting& b) const { return cmd == b.cmd && data == b.data && size == b.size; }
	hash_t  GetHashVal() const                { return CombineHash(cmd, data); }
	Txt     ToTxt() const                    { return "рисуется " + AsTxt(size); }

	operator Val() const                      { return RichToVal(*this); }
	Painting(const Val& q)                    { *this = q.Get<Painting>(); }

	Painting()                                  { SetNull(); }
	Painting(const Nuller&)                     { SetNull(); }
};

enum {
	MODE_ANTIALIASED = 0,
	MODE_NOAA        = 1,
	MODE_SUBPIXEL    = 2,
};

bool HasPainter();
void PaintImageBuffer(ImageBuffer& ib, const Painting& p, Size sz, Point pos, i32 mode = MODE_ANTIALIASED);
void PaintImageBuffer(ImageBuffer& ib, const Painting& p, i32 mode = MODE_ANTIALIASED);
void PaintImageBuffer(ImageBuffer& ib, const Drawing& p, i32 mode = MODE_ANTIALIASED);

class Draw : NoCopy {
	struct DrawingPos;

public:
	enum {
		DOTS = 0x001,
		PRINTER = 0x004,
		NATIVE = 0x008,
		DATABANDS = 0x010,
		DRAWTEXTLINES = 0x020,
		DRAWING = 0x040,
	};

	virtual dword GetInfo() const = 0;

	virtual Size GetPageSize() const;
	virtual void StartPage();
	virtual void EndPage();

	virtual void BeginOp() = 0;
	virtual void EndOp() = 0;
	virtual void OffsetOp(Point p) = 0;
	virtual bool ClipOp(const Rect& r) = 0;
	virtual bool ClipoffOp(const Rect& r) = 0;
	virtual bool ExcludeClipOp(const Rect& r) = 0;
	virtual bool IntersectClipOp(const Rect& r) = 0;
	virtual bool IsPaintingOp(const Rect& r) const = 0;
	virtual Rect GetPaintRect() const;

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color) = 0;
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, Color color);
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	virtual void DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color) = 0;

	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                i32k *counts, i32 count_count,
	                                i32 width, Color color, Color doxor) = 0;
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                                   i32k *subpolygon_counts, i32 scc,
	                                   i32k *disjunct_polygon_counts, i32 dpcc,
	                                   Color color, i32 width, Color outline,
	                                   zu64 pattern, Color doxor) = 0;
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color) = 0;

	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor) = 0;
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx) = 0;
	virtual void DrawDrawingOp(const Rect& target, const Drawing& w);
	virtual void DrawPaintingOp(const Rect& target, const Painting& w);

	virtual Size GetNativeDpi() const;
	virtual void BeginNative();
	virtual void EndNative();

	virtual i32  GetCloffLevel() const;

	virtual void Escape(const Txt& data);

	virtual ~Draw();

// --------------
	Size  GetPixelsPerInch() const;
	Size  GetPageMMs() const;

	bool  Dots() const                                  { return GetInfo() & DOTS; }
	bool  Pixels() const                                { return !Dots(); }
	bool  IsPrinter() const                             { return GetInfo() & PRINTER; }
	bool  IsNative() const                              { return GetInfo() & NATIVE; }

	i32  GetNativeX(i32 x) const;
	i32  GetNativeY(i32 y) const;
	void Native(i32& x, i32& y) const;
	void Native(Point& p) const;
	void Native(Size& sz) const;
	void Native(Rect& r) const;

	void  Begin()                                       { BeginOp(); }
	void  End()                                         { EndOp(); }
	void  Offset(Point p)                               { OffsetOp(p); }
	void  Offset(i32 x, i32 y);
	bool  Clip(const Rect& r)                           { return ClipOp(r); }
	bool  Clip(i32 x, i32 y, i32 cx, i32 cy);
	bool  Clipoff(const Rect& r)                        { return ClipoffOp(r); }
	bool  Clipoff(i32 x, i32 y, i32 cx, i32 cy);
	bool  ExcludeClip(const Rect& r)                    { return ExcludeClipOp(r); }
	bool  ExcludeClip(i32 x, i32 y, i32 cx, i32 cy);
	bool  IntersectClip(const Rect& r)                  { return IntersectClipOp(r); }
	bool  IntersectClip(i32 x, i32 y, i32 cx, i32 cy);
	bool  IsPainting(const Rect& r) const               { return IsPaintingOp(r); }
	bool  IsPainting(i32 x, i32 y, i32 cx, i32 cy) const;

	void DrawRect(i32 x, i32 y, i32 cx, i32 cy, Color color);
	void DrawRect(const Rect& rect, Color color);

	void DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src);
	void DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img);
	void DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	void DrawImage(i32 x, i32 y, i32 cx, i32 cy, const Image& img, Color color);

	void DrawImage(const Rect& r, const Image& img, const Rect& src);
	void DrawImage(const Rect& r, const Image& img);
	void DrawImage(const Rect& r, const Image& img, const Rect& src, Color color);
	void DrawImage(const Rect& r, const Image& img, Color color);

	void DrawImage(i32 x, i32 y, const Image& img, const Rect& src);
	void DrawImage(i32 x, i32 y, const Image& img);
	void DrawImage(i32 x, i32 y, const Image& img, const Rect& src, Color color);
	void DrawImage(i32 x, i32 y, const Image& img, Color color);

	void DrawData(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk type);
	void DrawData(const Rect& r, const Txt& data, tukk type);

	void DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, i32 width = 0, Color color = DefaultInk());
	void DrawLine(Point p1, Point p2, i32 width = 0, Color color = DefaultInk());

	void DrawEllipse(const Rect& r, Color color = DefaultInk(),
	                 i32 pen = Null, Color pencolor = DefaultInk());
	void DrawEllipse(i32 x, i32 y, i32 cx, i32 cy, Color color = DefaultInk(),
		             i32 pen = Null, Color pencolor = DefaultInk());

	void DrawArc(const Rect& rc, Point start, Point end, i32 width = 0, Color color = DefaultInk());

	void DrawPolyPolyline(const Point *vertices, i32 vertex_count,
	                      i32k *counts, i32 count_count,
	                      i32 width = 0, Color color = DefaultInk(), Color doxor = Null);
	void DrawPolyPolyline(const Vec<Point>& vertices, const Vec<i32>& counts,
		                  i32 width = 0, Color color = DefaultInk(), Color doxor = Null);
	void DrawPolyline(const Point *vertices, i32 count,
		              i32 width = 0, Color color = DefaultInk(), Color doxor = Null);
	void DrawPolyline(const Vec<Point>& vertices,
		              i32 width = 0, Color color = DefaultInk(), Color doxor = Null);

	void   DrawPolyPolyPolygon(const Point *vertices, i32 vertex_count,
		                       i32k *subpolygon_counts, i32 subpolygon_count_count,
		                       i32k *disjunct_polygon_counts, i32 disjunct_polygon_count_count,
		                       Color color = DefaultInk(), i32 width = 0, Color outline = Null,
		                       zu64 pattern = 0, Color doxor = Null);
	void   DrawPolyPolyPolygon(const Vec<Point>& vertices,
	                           const Vec<i32>& subpolygon_counts,
	                           const Vec<i32>& disjunct_polygon_counts,
	                           Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolyPolygon(const Point *vertices, i32 vertex_count,
	                       i32k *subpolygon_counts, i32 subpolygon_count_count,
	                       Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolyPolygon(const Vec<Point>& vertices, const Vec<i32>& subpolygon_counts,
	                       Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolygons(const Point *vertices, i32 vertex_count,
	                    i32k *polygon_counts, i32 polygon_count_count,
	                    Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolygons(const Vec<Point>& vertices, const Vec<i32>& polygon_counts,
	                    Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolygon(const Point *vertices, i32 vertex_count,
	                   Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);
	void   DrawPolygon(const Vec<Point>& vertices,
	                   Color color = DefaultInk(), i32 width = 0, Color outline = Null, zu64 pattern = 0, Color doxor = Null);

	void DrawDrawing(const Rect& r, const Drawing& iw) { DrawDrawingOp(r, iw); }
	void DrawDrawing(i32 x, i32 y, i32 cx, i32 cy, const Drawing& iw);
	void DrawDrawing(i32 x, i32 y, const Drawing& iw);

	void DrawPainting(const Rect& r, const Painting& iw) { DrawPaintingOp(r, iw); }
	void DrawPainting(i32 x, i32 y, i32 cx, i32 cy, const Painting& iw);
	void DrawPainting(i32 x, i32 y, const Painting& iw);

	void DrawText(i32 x, i32 y, i32 angle, wtukk text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);
	void DrawText(i32 x, i32 y, wtukk text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);

	void DrawText(i32 x, i32 y, const WTxt& text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32k *dx = NULL);
	void DrawText(i32 x, i32 y, i32 angle, const WTxt& text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32k *dx = NULL);

	void DrawText(i32 x, i32 y, i32 angle, tukk text, u8 charset,
	              Font font = StdFont(), Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);
	void DrawText(i32 x, i32 y, tukk text, u8 charset, Font font = StdFont(),
		          Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);

	void DrawText(i32 x, i32 y, i32 angle, tukk text,
	              Font font = StdFont(), Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);
	void DrawText(i32 x, i32 y, tukk text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32 n = -1, i32k *dx = NULL);

	void DrawText(i32 x, i32 y, const Txt& text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32k *dx = NULL);
	void DrawText(i32 x, i32 y, i32 angle, const Txt& text, Font font = StdFont(),
		          Color ink = DefaultInk(), i32k *dx = NULL);

	static void SinCos(i32 angle, double& sina, double& cosa);

	// deprecated:
	static void SetStdFont(Font font)                   { DRX::SetStdFont(font); }
	static Font GetStdFont()                            { return DRX::GetStdFont(); }
	static Size GetStdFontSize()                        { return DRX::GetStdFontSize(); }
	static i32  GetStdFontCy()                          { return GetStdFontSize().cy; }
	Size   GetPagePixels() const                        { return GetPageSize(); }

protected:
	Color ResolveInk(Color c) const                     { return c == DefaultInk() ? GetDefaultInk() : c; }
	virtual Color GetDefaultInk() const;
};

void DrawImageBandRLE(Draw& w, i32 x, i32 y, const Image& m, i32 minp);

class DataDrawer {
	typedef DataDrawer *(*Factory)();
	template <class T> static DataDrawer *FactoryFn() { return new T; }
	static void AddFormat(tukk id, Factory f);
	static VecMap<Txt, uk >& Map();

public:
	virtual void  Open(const Txt& data, i32 cx, i32 cy) = 0;
	virtual void  Render(ImageBuffer& ib) = 0;
	virtual ~DataDrawer();

	static  One<DataDrawer> Create(const Txt& id);

	template <class T>	static void Register(tukk id)  { AddFormat(id, &DataDrawer::FactoryFn<T>); }
};

class Drawing : public ValType<Drawing, 49, Moveable<Drawing> > {
	Size       size;
	Txt        data;
	ValArray   val;

	friend class DrawingDraw;
	friend class Draw;

public:
	operator bool() const          { return !data.IsEmpty(); }
	Size GetSize() const           { return size; }
	void SetSize(Size sz)          { size = sz; }
	void SetSize(i32 cx, i32 cy)   { size = Size(cx, cy); }

	Size RatioSize(i32 cx, i32 cy) const;
	Size RatioSize(Size sz) const  { return RatioSize(sz.cx, sz.cy); }

	void Clear()                   { data.Clear(); size = Null; }

	void Append(Drawing& dw);

	void Serialize(Stream& s);
	void Xmlize(XmlIO& xio)        { XmlizeBySerialize(xio, *this); }
	void Jsonize(JsonIO& jio)      { JsonizeBySerialize(jio, *this); }

	bool    IsNullInstance() const             { return data.IsEmpty(); }
	void    SetNull()                          { size = Null; data.Clear(); }

	bool    operator==(const Drawing& b) const { return val == b.val && data == b.data && size == b.size; }
	Txt  ToTxt() const                   { return "чертится " + AsTxt(size); }
	hash_t  GetHashVal() const               { return CombineHash(data, val); }

	operator Val() const                     { return RichToVal(*this); }
	Drawing(const Val& src)                  { *this = src.Get<Drawing>(); }

	Drawing()                                  { SetNull(); }
	Drawing(const Nuller&)                     { SetNull(); }
};

class DrawingDraw : public Draw {
public:
	virtual dword GetInfo() const;
	virtual Size  GetPageSize() const;
	virtual Rect  GetPaintRect() const;

	virtual void BeginOp();
	virtual void EndOp();
	virtual void OffsetOp(Point p);
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual bool IsPaintingOp(const Rect& r) const;

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	virtual void DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                i32k *counts, i32 count_count,
	                                i32 width, Color color, Color doxor);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                                   i32k *subpolygon_counts, i32 scc,
	                                   i32k *disjunct_polygon_counts, i32 dpcc,
	                                   Color color, i32 width, Color outline,
	                                   zu64 pattern, Color doxor);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 pen, Color pencolor);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx);

	virtual void DrawDrawingOp(const Rect& target, const Drawing& w);
	virtual void DrawPaintingOp(const Rect& target, const Painting& w);

	virtual void Escape(const Txt& data);

private:
	Size         size;
	bool         dots;
	TxtStream    drawing;
	ValArray     val;

	Stream&      DrawingOp(i32 code);

public:
	void     Create(i32 cx, i32 cy, bool dots = true);
	void     Create(Size sz, bool dots = true);

	Size     GetSize() const                  { return size; }

	Drawing  GetResult();
	operator Drawing()                        { return GetResult(); }

	DrawingDraw();
	DrawingDraw(i32 cx, i32 cy, bool dots = true);
	DrawingDraw(Size sz, bool dots = true);
};

class NilDraw : public Draw {
public:
	virtual dword GetInfo() const;
	virtual Size  GetPageSize() const;
	virtual void BeginOp();
	virtual void EndOp();
	virtual void OffsetOp(Point p);
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual bool IsPaintingOp(const Rect& r) const;
	virtual Rect GetPaintRect() const;

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	virtual void DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                i32k *counts, i32 count_count,
	                                i32 width, Color color, Color doxor);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                                   i32k *subpolygon_counts, i32 scc,
	                                   i32k *disjunct_polygon_counts, i32 dpcc,
	                                   Color color, i32 width, Color outline,
	                                   zu64 pattern, Color doxor);
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx);
	virtual void DrawDrawingOp(const Rect& target, const Drawing& w);
	virtual void DrawPaintingOp(const Rect& target, const Painting& w);
};

struct DrawProxy : Draw {
	Draw *ptr;

	void SetTarget(Draw *w) { ptr = w; }

	virtual dword GetInfo() const;

	virtual Size GetPageSize() const;
	virtual void StartPage();
	virtual void EndPage();

	virtual void BeginOp();
	virtual void EndOp();
	virtual void OffsetOp(Point p);
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual bool IsPaintingOp(const Rect& r) const;
	virtual Rect GetPaintRect() const;

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, Color color);
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	virtual void DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);

	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                i32k *counts, i32 count_count,
	                                i32 width, Color color, Color doxor);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                                   i32k *subpolygon_counts, i32 scc,
	                                   i32k *disjunct_polygon_counts, i32 dpcc,
	                                   Color color, i32 width, Color outline,
	                                   zu64 pattern, Color doxor);
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);

	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx);
	virtual void DrawDrawingOp(const Rect& target, const Drawing& w);
	virtual void DrawPaintingOp(const Rect& target, const Painting& w);

	virtual Size GetNativeDpi() const;
	virtual void BeginNative();
	virtual void EndNative();

	virtual i32  GetCloffLevel() const;

	virtual void Escape(const Txt& data);
};

class ImageAnyDraw : public Draw {
	Draw *draw;

	void Init(Size sz);

public:
	virtual dword GetInfo() const;
	virtual Size  GetPageSize() const;
	virtual void BeginOp();
	virtual void EndOp();
	virtual void OffsetOp(Point p);
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual bool IsPaintingOp(const Rect& r) const;
	virtual Rect GetPaintRect() const;

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
	virtual void DrawDataOp(i32 x, i32 y, i32 cx, i32 cy, const Txt& data, tukk id);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                                i32k *counts, i32 count_count,
	                                i32 width, Color color, Color doxor);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	                                   i32k *subpolygon_counts, i32 scc,
	                                   i32k *disjunct_polygon_counts, i32 dpcc,
	                                   Color color, i32 width, Color outline,
	                                   zu64 pattern, Color doxor);
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx);
	virtual void DrawDrawingOp(const Rect& target, const Drawing& w);
	virtual void DrawPaintingOp(const Rect& target, const Painting& w);

public:
	static bool IsAvailable();

	operator Image() const;

	ImageAnyDraw(Size sz);
	ImageAnyDraw(i32 cx, i32 cy);

	~ImageAnyDraw();
};

void         AddNotEmpty(Vec<Rect>& result, i32 left, i32 right, i32 top, i32 bottom);
bool         Subtract(const Rect& r, const Rect& sub, Vec<Rect>& result);
bool         Subtract(const Vec<Rect>& rr, const Rect& sub, Vec<Rect>& result);
Vec<Rect> Subtract(const Vec<Rect>& rr, const Rect& sub, bool& changed);
Vec<Rect> Intersect(const Vec<Rect>& b, const Rect& a, bool& changed);

void Subtract(Vec<Rect>& rr, const Rect& sub);
void Union(Vec<Rect>& rr, const Rect& add);

Vec<Rect> Intersection(const Vec<Rect>& b, const Rect& a);

void AddRefreshRect(Vec<Rect>& invalid, const Rect& _r, double ctolerance = 2);

void DrawDragFrame(Draw& w, const Rect& r, i32 n, i32k *pattern, Color color, i32 animation);
void DrawDragFrame(Draw& w, const Rect& r, i32 n, i32 pattern, Color color, i32 animation);

void DrawRect(Draw& w, const Rect& rect, const Image& img, bool ralgn = false); //??? TODO
void DrawRect(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Image& img, bool ra = false);

void DrawTiles(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Image& img);
void DrawTiles(Draw& w, const Rect& rect, const Image& img);

void DrawFatFrame(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color color, i32 n);
void DrawFatFrame(Draw& w, const Rect& r, Color color, i32 n);

void DrawFrame(Draw& w, i32 x, i32 y, i32 cx, i32 cy,
			   Color leftcolor, Color topcolor, Color rightcolor, Color bottomcolor);
void DrawFrame(Draw& w, const Rect& r,
			   Color leftcolor, Color topcolor, Color rightcolor, Color bottomcolor);
void DrawFrame(Draw& w, i32 x, i32 y, i32 cx, i32 cy,
			   Color topleftcolor, Color bottomrightcolor);
void DrawFrame(Draw& w, const Rect& r,
			   Color topleftcolor, Color bottomrightcolor);
void DrawFrame(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color color);
void DrawFrame(Draw& w, const Rect& r, Color color);

void DrawBorder(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const ColorF *colors_ltrd); //TODO
void DrawBorder(Draw& w, const Rect& r, const ColorF *colors_ltrd);

const ColorF *BlackBorder();
const ColorF *WhiteBorder();
const ColorF *ButtonPushBorder();
const ColorF *EdgeButtonBorder();
const ColorF *DefButtonBorder();
const ColorF *ButtonBorder();
const ColorF *InsetBorder();
const ColorF *OutsetBorder();
const ColorF *ThinOutsetBorder();
const ColorF *ThinInsetBorder();

void DrawBorder(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const ColorF *(*colors_ltrd)());
void DrawBorder(Draw& w, const Rect& r, const ColorF *(*colors_ltrd)());

void DrawRectMinusRect(Draw& w, const Rect& rect, const Rect& inner, Color color);

void DrawHighlightImage(Draw& w, i32 x, i32 y, const Image& img, bool highlight = true,
                        bool enabled = true, Color maskcolor = SColorPaper);

Color GradientColor(Color fc, Color tc, i32 i, i32 n);

void DrawTextEllipsis(Draw& w, i32 x, i32 y, i32 cx, tukk text, tukk ellipsis,
				      Font font = StdFont(), Color ink = SColorText(), i32 n = -1);
void DrawTextEllipsis(Draw& w, i32 x, i32 y, i32 cx, wtukk text, tukk ellipsis,
				      Font font = StdFont(), Color ink = SColorText(), i32 n = -1);

Size GetTLTextSize(wtukk text, Font font = StdFont());
i32  GetTLTextHeight(wtukk s, Font font = StdFont());
void DrawTLText(Draw& draw, i32 x, i32 y, i32 cx, wtukk text, Font font = StdFont(),
                Color ink = SColorText(), i32 accesskey = 0);

enum {
	BUTTON_NORMAL, BUTTON_OK, BUTTON_HIGHLIGHT, BUTTON_PUSH, BUTTON_DISABLED, BUTTON_CHECKED,
	BUTTON_VERTICAL = 0x100,
	BUTTON_EDGE = 0x200,
	BUTTON_TOOL = 0x400,
	BUTTON_SCROLL = 0x800,
};

void DrawXPButton(Draw& w, Rect r, i32 type);

struct PdfSignatureInfo;
typedef Txt (*DrawingToPdfFnType)(const Array<Drawing>& report, Size pagesize, i32 margin,
                                     bool pdfa, const PdfSignatureInfo *sign);
typedef void (*PdfDrawJPEGFnType)(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& jpeg_data);

void SetDrawingToPdfFn(DrawingToPdfFnType Pdf, PdfDrawJPEGFnType Jpeg);
DrawingToPdfFnType GetDrawingToPdfFn();
PdfDrawJPEGFnType GetPdfDrawJPEGFn();

typedef bool (*IsJPGFnType)(StreamRaster *s);
void SetIsJPGFn(IsJPGFnType isjpg);
IsJPGFnType GetIsJPGFn();

#include "Display.h"
#include "Cham.h"
#include "DDARasterizer.h"
#include "SDraw.h"

enum {
	CMAP_GLYPHS = 1,
	CMAP_ALLOW_SYMBOL = 2,
};

bool ReadCmap(tukk ptr, i32 count, Event<i32, i32, i32> range, dword flags = 0);
bool ReadCmap(Font font, Event<i32, i32, i32> range, dword flags = 0);
bool GetPanoseNumber(Font font, u8 *panose);

}

#endif