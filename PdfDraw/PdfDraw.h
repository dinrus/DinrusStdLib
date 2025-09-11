#ifndef _PDF_pdf_h_
#define _PDF_pdf_h_

#include <drx/Draw/Draw.h>
//#include <drx/Core/lib/z/z.h>
#include <drx/Painter/Painter.h>

namespace drx {

INITIALIZE(PdfDraw);

class TTFReader {
	struct TTFStream {
		struct Fail {};

		virtual void Raw(uk ptr, i32 count) = 0;

		TTFStream& operator%(u8& q);
		TTFStream& operator%(i32& q);
		TTFStream& operator%(u32& q);
		TTFStream& operator%(i16& q);
		TTFStream& operator%(u16& q);

		virtual ~TTFStream() {}
	};

	struct TTFStreamIn : TTFStream {
		tukk beg;
		tukk s;
		tukk lim;

		virtual void Raw(uk ptr, i32 count);

		void   Seek(i32 offset);
		i32    Get8();
		i32    Get16();
		i32    Get32();
		Txt    Get(i32 n);

		virtual ~TTFStreamIn() {}
	};

	struct TTFStreamOut : TTFStream {
		Txt out;

		virtual void Raw(uk ptr, i32 count) { out.Cat((tukk )ptr, count); }

		void   Put8(i32 data)       { out.Cat(data); }
		void   Put16(i32 data)      { Put8(HIBYTE(data)); Put8(LOBYTE(data)); }
		void   Put32(i32 data)      { Put16(HIWORD(data)); Put16(LOWORD(data)); }
		void   Put(const Txt& s) { out.Cat(s); }

		operator Txt() const     { return out; }

		virtual ~TTFStreamOut() {}
	};

	Font font;
	Txt current_table;

	struct Table : Moveable<Table> {
		i32 offset;
		i32 length;
	};
	VecMap<Txt, Table> table;
	
	VecMap<wchar, i32> glyph_map;

	struct GlyphInfo : Moveable<GlyphInfo> {
		i32    offset;
		i32    size;
		u16    advanceWidth;
		i16    leftSideBearing;
	};

	Vec<GlyphInfo> glyphinfo;

	void   SetGlyph(wchar ch, word glyph);

	struct Fail {};

	static void Error() { throw Fail(); }

	i32    Peek8(tukk s);
	i32    Peek16(tukk s);
	i32    Peek32(tukk s);
	i32    Peek8(tukk s, i32 i);
	i32    Peek16(tukk s, i32 i);
	i32    Peek32(tukk s, i32 i);
	i32    Read8(tukk &s);
	i32    Read16(tukk &s);
	i32    Read32(tukk &s);
	Txt    Read(tukk &s, i32 n);

	void   Reset();

	tukk   Seek(tukk tab, i32& len);
	tukk   Seek(tukk tab);
	void   Seek(tukk tab, TTFStreamIn& s);
	Txt    GetTable(tukk tab);

	enum {
		ARG_1_AND_2_ARE_WORDS     = (1<<0),
		ARGS_ARE_XY_VALUES        = (1<<1),
		ROUND_XY_TO_GRID          = (1<<2),
		WE_HAVE_A_SCALE           = (1<<3),
		MORE_COMPONENTS           = (1<<5),
		WE_HAVE_AN_X_AND_Y_SCALE  = (1<<6),
		WE_HAVE_A_TWO_BY_TWO      = (1<<7),
		WE_HAVE_INSTRUCTIONS      = (1<<8),
		USE_MY_METRICS            = (1<<9),
	};

	struct NewTable {
		Txt name;
		Txt data;
		dword  checksum;
	};

	static void   Add(Array<NewTable>& t, tukk name, const Txt& data);
	static void   Set(NewTable& m, const Txt& data);
	static Txt CreateHeader(const Array<NewTable>& t);

	static inline i32    Copy8(TTFStreamOut& out, TTFStreamIn& in)
		{ i32 q = in.Get8(); out.Put8(q); return q; }
	static inline i32    Copy16(TTFStreamOut& out, TTFStreamIn& in)
		{ i32 q = in.Get16(); out.Put16(q); return q; }
	static inline i32    Copy32(TTFStreamOut& out, TTFStreamIn& in)
		{ i32 q = in.Get32(); out.Put32(q); return q; }
	static inline Txt Copy(TTFStreamOut& out, TTFStreamIn& in, i32 n)
		{ Txt q = in.Get(n); out.Put(q); return q; }

public:
	struct Head {
		dword version;
		dword fontRevision;
		u32   checkSumAdjustment;
		u32   magicNumber;
		u16   flags;
		u16   unitsPerEm;
		u8    created[8];
		u8    modified[8];
		i16   xMin;
		i16   yMin;
		i16   xMax;
		i16   yMax;
		u16   macStyle;
		u16   lowestRecPPEM;
		i16   fontDirectionHint;
		i16   indexToLocFormat;
		i16   glyphDataFormat;

		void Serialize(TTFStream& s);
	};

	struct Hhea {
		u32  version;
		i16   ascent;
		i16   descent;
		i16   lineGap;
		u16   advanceWidthMax;
		i16   minLeftSideBearing;
		i16   minRightSideBearing;
		i16   xMaxExtent;
		i16   caretSlopeRise;
		i16   caretSlopeRun;
		i16   caretOffset;
		i16   reserved1;
		i16   reserved2;
		i16   reserved3;
		i16   reserved4;
		i16   metricDataFormat;
		u16   numOfLongHorMetrics;

		void Serialize(TTFStream& s);
	};

	struct Maxp {
		u32  version;
		u16  numGlyphs;
		u16  maxPoints;
		u16  maxContours;
		u16  maxComponentPoints;
		u16  maxComponentContours;
		u16  maxZones;
		u16  maxTwilightPoints;
		u16  maxStorage;
		u16  maxFunctionDefs;
		u16  maxInstructionDefs;
		u16  maxStackElements;
		u16  maxSizeOfInstructions;
		u16  maxComponentElements;
		u16  maxComponentDepth;

		void Serialize(TTFStream& s);
	};

	struct Post {
		u32   format;
		i32   italicAngle;
		i16   underlinePosition;
		i16   underlineThickness;
		u16   isFixedPitch;
		u16   reserved;
		u32   minMemType42;
		u32   maxMemType42;
		u32   minMemType1;
		u32   maxMemType1;

		void Serialize(TTFStream& s);
	};

	Head   head;
	Hhea   hhea;
	Maxp   maxp;
	Post   post;
	Txt    ps_name;

	i32    GetGlyph(wchar chr)               { return glyph_map.Get(chr, 0); }
	word   GetAdvanceWidth(wchar chr)        { i32 i = glyph_map.Get(chr, 0); return i < glyphinfo.GetCount() ? glyphinfo[GetGlyph(chr)].advanceWidth : 0; }

	Txt Subset(const Vec<wchar>& chars, i32 first = 0, bool os2 = false);
	bool   Open(const Font& fnt, bool symbol = false, bool justcheck = false);

	TTFReader();
};

struct PdfSignatureInfo {
	Txt pkey;
	Txt cert;

	Txt name;
	Txt location;
	Txt reason;
	Txt contact_info;

	Time   time;
	
	PdfSignatureInfo() { time = Null; }
};

class PdfDraw : public Draw {
public:
	virtual dword GetInfo() const;
	virtual Size  GetPageSize() const;

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

	virtual	void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& img, const Rect& src, Color color);
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
	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font,
		                    Color ink, i32 n, i32k *dx);

	virtual void Escape(const Txt& data);

private:
	struct CharPos : Moveable<CharPos>   { word fi, ci; };

	struct OutlineInfo : Moveable<OutlineInfo> {
		bool standard_ttf;
		bool sitalic;
		bool sbold;
	};
	
	struct UrlInfo {
		Rectf  rect;
		Txt url;
	};

	VecMap<Font, OutlineInfo>                outline_info;
	VecMap<Font, Vec<wchar>>              pdffont;
	VecMap<Font, VecMap<wchar, CharPos>>  fontchars;
	Index<zu64>                               patterns;
	VecMap<Tuple2<z64, Rect>, Image>       images;
	Array<Array<UrlInfo>>                       page_url;
	Txt                                      data; // temporary escape data, e.g. JPEG
	Vec<Txt>                              jpeg;
	
	Vec<i32>    offset;
	TxtBuf   out;
	TxtBuf   page;
	Size        pgsz;
	Color       rgcolor;
	Color       RGcolor;
	i32         fontid;
	double      textht;
	double      linewidth;
	i32         margin;
	zu64        patternid;
	bool        pdfa;
	bool        empty;
	Txt         url;
	Vec<Point> offset_stack;
	Point       current_offset;

	double Pt(double dot)               { return 0.12 * dot; }
	Txt Ptf(double dot)              { return FormatF(Pt(dot), 5); }

	i32    Pos()                        { return offset.GetCount() + 1; }
	i32    BeginObj();
	void   EndObj();
	i32    PutStream(const Txt& data, const Txt& keys = Null, bool compress = true);

	void    PutRect(const Rect& rc);
	void    PutrgColor(Color rg, zu64 pattern = 0);
	void    PutRGColor(Color RG);
	void    PutFontHeight(i32 fi, double ht);
	void    PutLineWidth(i32 lw);

	CharPos GetCharPos(Font fnt, wchar chr);
	void    FlushText(i32 dx, i32 fi, i32 height, const Txt& txt);
	static Txt PdfColor(Color c);
	static Txt PdfTxt(tukk s);

	void PushOffset();
	void PopOffset();

	OutlineInfo GetOutlineInfo(Font fnt);

	void Init(i32 pagecx, i32 pagecy, i32 margin, bool pdfa);

	struct RGlyph : Moveable<RGlyph> {
		Txt data;
		Size   sz;
		i32    x;
		i32    color_image = -1;
	};
	
	struct CGlyph : Moveable<CGlyph> {
		Size   sz;
		i32    x;
		i32    image;
	};
	
	VecMap<Tuple<Font, i32>, CGlyph> color_glyph;

	i32    PdfImage(const Image& img, const Rect& src);
	CGlyph ColorGlyph(Font fnt, i32 chr);
	RGlyph RasterGlyph(Font fnt, i32 chr);

public:
	Txt Finish(const PdfSignatureInfo *sign = NULL);
	void   Clear();
	bool   IsEmpty() const                                   { return empty; }
	
	PdfDraw(i32 pagecx, i32 pagecy, bool pdfa = false)       { Init(pagecx, pagecy, 0, pdfa); }
	PdfDraw(Size pgsz = Size(5100, 6600), bool pdfa = false) { Init(pgsz.cx, pgsz.cy, 0, pdfa); }
};

void DrawJPEG(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& jpeg_data);

Txt Pdf(const Array<Drawing>& report, Size pagesize, i32 margin, bool pdfa = false,
           const PdfSignatureInfo *sign = NULL);

}

#endif
