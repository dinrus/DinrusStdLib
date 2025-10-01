#ifndef RICHTEXT_H
#define RICHTEXT_H

#include <drx/Draw/Draw.h>
#include <drx/Draw/Draw.h>
#include <X/png/png.h>

namespace drx {

#define IMAGECLASS RichTextImg
#define IMAGEFILE <drx/RichText/RichText.iml>
#include <drx/Draw/iml_header.h>

INITIALIZE(RichImage);

class  PasteClip;
struct RichPara;
class  RichTable;
class  RichTxt;

inline Color VoidColor() { return Color::Special(223); } // Used for mixed/no change in Get/SetCellFormat

struct Zoom {
	i32 m, d;

	i32 operator*(i32 x) const   { return d ? iscale(x, m, d) : 0; }
	i32 operator&(i32 x) const   { i32 q = d ? iscale(x, m, d) : 0; return x > 0 ? max(q, 1) : q; }

	double AsDouble() const      { return (double)m / d; }
	Zoom   Reciprocal() const    { return Zoom(d, m); }

	Zoom()                       { m = d = 1; }
	Zoom(const Nuller&)          { m = d = 0; }
	Zoom(i32 _m, i32 _d)         { m = _m, d = _d; }

	void Serialize(Stream& s)    { s % m % d; }

	bool operator==(Zoom a)      { return m == a.m && d == a.d; }
	bool operator!=(Zoom a)      { return m != a.m || d != a.d; }

	Txt ToTxt() const      { return Txt() << m << "/" << d; }

	friend i32 operator/(i32 x, Zoom z)  { return z.m ? iscale(x, z.d, z.m) : 0; }
};

inline bool IsNull(Zoom z) { return (z.m | z.d) == 0; }

inline i32 operator*(i32 x, Zoom m)
{
	return m * x;
}

inline void operator*=(i32& i, Zoom m)
{
	i = m * i;
}

inline void operator*=(Rect& r, Zoom m)
{
	r.left *= m;
	r.right *= m;
	r.top *= m;
	r.bottom *= m;
}

inline Size operator*(Zoom m, Size sz)
{
	return Size(m * sz.cx, m * sz.cy);
}

inline Size operator/(Size sz, Zoom m)
{
	return Size(sz.cx / m, sz.cy / m);
}

struct PageY : Moveable<PageY, RelOps<PageY> > {
	i32 page;
	i32 y;

	PageY(i32 page, i32 y) : page(page), y(y) {}
	PageY() { page = y = 0; }

#ifdef _DEBUG
	Txt ToTxt() const    { return AsTxt(page) + ":" + AsTxt(y); }
#endif
};

inline bool operator<(PageY a, PageY b)
{
	return a.page < b.page ? true : a.page == b.page ? a.y < b.y : false;
}

inline bool operator==(PageY a, PageY b)
{
	return a.page == b.page && a.y == b.y;
}

inline PageY operator+(PageY a, i32 b)
{
	return PageY(a.page, a.y + b);
}

struct PageRect : public Rect {
	i32  page;

	operator i32() const                 { return page; }
	operator PageY() const               { return PageY(page, top); }
	PageRect& operator=(const Rect& r)   { (Rect&)(*this) = r; page = 0; return *this; }
	PageRect(const Rect& r)              { (Rect&)(*this) = r; page = 0; }
	PageRect()                           { Clear(); page = 0; }
};

struct RichTextLayoutTracer {
	virtual void  Paragraph(const Rect& page, PageY y, const RichPara& para);
	virtual void  EndParagraph(PageY y);
	virtual void  Table(const Rect& page, PageY y, const RichTable& table);
	virtual void  EndTable(PageY y);
	virtual void  TableRow(const Rect& page, PageY y, i32 i, const RichTable& table);
	virtual void  EndTableRow(PageY y);
	virtual void  TableCell(const Rect& page, PageY y, i32 i, i32 j, const RichTable& table, PageY ny);
	virtual void  EndTableCell(PageY y);
};

struct PageDraw {
	virtual Draw& Page(i32 i) = 0;

	RichTextLayoutTracer *tracer;

	PageDraw() { tracer = NULL; }

	virtual ~PageDraw() {}
};

class RichObj;
class Bar;

struct RichObjectType {
	virtual Txt GetTypeName(const Val& v) const = 0;
	virtual Txt GetCreateName() const;
	virtual Val  Read(const Txt& s) const;
	virtual Txt Write(const Val& v) const;
	virtual bool   IsText() const;

	virtual bool   Accept(PasteClip& clip);
	virtual Val  Read(PasteClip& clip);
	virtual Txt GetClipFmts() const;
	virtual Txt GetClip(const Val& data, const Txt& fmt) const;

	virtual Size   GetDefaultSize(const Val& data, Size maxsize, uk context) const;
	virtual Size   GetPhysicalSize(const Val& data, uk context) const;
	virtual Size   GetPixelSize(const Val& data, uk context) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz, uk context) const;
	virtual Image  ToImage(z64 serial_id, const Val& data, Size sz, uk context) const;
	virtual void   Menu(Bar& bar, RichObj& ex, uk context) const;
	virtual void   DefaultAction(RichObj& ex, uk context) const;
	virtual Txt GetLink(const Val& data, Point pt, Size sz, uk context) const;

	Size           StdDefaultSize(const Val& data, Size maxsize, uk context) const;

	RichObjectType();
	virtual ~RichObjectType();

protected:
	virtual Size   GetDefaultSize(const Val& data, Size maxsize) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz) const;
	virtual Image  ToImage(z64 serial_id, const Val& data, Size sz) const;
	virtual void   Menu(Bar& bar, RichObj& ex) const;
	virtual void   DefaultAction(RichObj& ex) const;
	virtual Txt GetLink(const Val& data, Point pt, Size sz) const;
};

class RichObj : Moveable<RichObj> {
	Val                 data;
	i32                   ydelta;
	Size                  size;
	Size                  physical_size;
	Size                  pixel_size;
	bool                  keepratio;
	const RichObjectType *type;
	z64                 serial;
	Txt                type_name;

	static VecMap<Txt, RichObjectType *>& Map();

	void                  NewSerial();
	void                  AdjustPhysicalSize();

public:
	static void   Register(tukk name, RichObjectType *type) init_;
	static i32    GetTypeCount()                 { return Map().GetCount(); }
	static i32    FindType(const Txt& name)   { return Map().Find(name); }
	static RichObjectType& GetType(i32 i)        { return *Map()[i]; }
	static Txt GetTypeName(i32 i)             { return Map().GetKey(i); }

	void   SetSize(i32 cx, i32 cy)               { size = Size(cx, cy); }
	void   SetSize(Size sz)                      { SetSize(sz.cx, sz.cy); }
	Size   GetSize() const                       { return size; }
	void   Paint(Draw& w, Size sz, uk context = NULL) const;
	Image  ToImage(Size sz, uk context = NULL) const;
	Size   GetPhysicalSize() const               { return physical_size; }
	Size   GetPixelSize() const                  { return pixel_size; }
	Size   GetDefaultSize(Size maxsize, uk context = NULL) const { return type ? type->GetDefaultSize(data, maxsize, context) : physical_size; }

	void   Set(RichObjectType *type, const Val& data, Size maxsize = Size(3967, 3967), uk context = NULL);
	bool   Set(const Txt& type_name, const Val& data, Size maxsize = Size(3967, 3967), uk context = NULL);
	void   SetData(const Val& v);

	Txt GetTypeName() const;
	Val  GetData() const                       { return data; }
	Txt GetLink(Point pt, Size sz, uk context = NULL) const { return type ? type->GetLink(data, pt, sz, context) : Txt(); }

	const RichObjectType& GetType() const;

	bool   Read(const Txt& type, const Txt& data, Size sz, uk context = NULL);
	Txt Write() const                         { return type ? type->Write(data) : (Txt)data; }
	bool   IsText() const                        { return type ? type->IsText() : false; }

	void   KeepRatio(bool b)                     { keepratio = b; }
	bool   IsKeepRatio() const                   { return keepratio; }

	void   SetYDelta(i32 yd)                     { ydelta = yd; }
	i32    GetYDelta() const                     { return ydelta; }

	void   Menu(Bar& bar, uk context = NULL)  { if(type) type->Menu(bar, *this, context); }
	void   DefaultAction(uk context = NULL)   { if(type) type->DefaultAction(*this, context); }

	operator bool() const                        { return !IsNull(data); }

	void   Clear();

	z64  GetSerialId() const                   { return serial; }

	void   InitSize(i32 cx, i32 cy, uk context = NULL);

	RichObj();
	RichObj(RichObjectType *type, const Val& data, Size maxsize = Size(3967, 3967));
	RichObj(const Txt& type, const Val& data, Size maxsize = Size(3967, 3967));
};

RichObj CreateDrawingObject(const Drawing& dwg, Size dot_size, Size size);
RichObj CreateDrawingObject(const Drawing& dwg, i32 cx = 0, i32 cy = 0);
RichObj CreatePaintingObject(const Painting& dwg, Size dot_size, Size size);
RichObj CreatePaintingObject(const Painting& dwg, i32 cx = 0, i32 cy = 0);
RichObj CreatePNGObject(const Image& img, Size dot_size, Size size);
RichObj CreatePNGObject(const Image& img, i32 cx = 0, i32 cy = 0);
RichObj CreateRawImageObject(const Txt& s, i32 cx = 0, i32 cy = 0);

enum {
	RICHHOT_LM = -1,
	RICHHOT_RM = -2,
};

struct RichHotPos { // used for resizing table dimensions by mouse
	i32 table;
	i32 column; // can be RICHHOT_LM or RICHHOT_RM or columns index
	i32 delta;
	i32 left, cx;
	i32 textleft, textcx;

	RichHotPos()       { table = 0; column = Null; left = cx = 0; }
};

struct RichValPos : Moveable<RichValPos> {
	PageY   py;
	i32     pos;
	WTxt data;
};

struct PaintInfo {
	Zoom    zoom;
	i32     sell, selh;
	i32     tablesel;
	Rect    cells;
	PageY   top;
	PageY   bottom;
	Color   hyperlink;
	Color   indexentry;
	Color   textcolor; // not Null overrides text color, makes paper transparent, useful for QTFDisplay
	bool    indexentrybg;
	bool    usecache;
	bool    sizetracking;
	Color   showcodes;
	Bits  (*spellingchecker)(const RichPara& para);
	i32     highlightpara;
	Gate<const Txt&> WhenHighlight;
	Color   highlight;
	bool    darktheme;
	void   *context;
	bool    showlabels;
	bool    shrink_oversized_objects;
	void  (*DrawSelection)(Draw& w, i32 x, i32 y, i32 cx, i32 cy);

	Color   ResolveInk(Color ink) const;
	Color   ResolvePaper(Color paper) const;

	PaintInfo();
};

i32 LineZoom(Zoom z, i32 a);

class RichTable;
class RichText;
struct RichStyle;

typedef ArrayMap<Uuid, RichStyle> RichStyles;

struct RichContext {
	const RichText   *text;
	const RichStyles *styles;
	RichText         *header, *footer;
	i32               header_cy, footer_cy; // next page header/footer size
	i32               current_header_cy, current_footer_cy; // current header/footer size
	Rect              page;
	PageY             py;

	void              HeaderFooter(RichText *header, RichText *footer_qtf);
	void              AdjustPage();
	void              Page();
	void              Set(PageY p0, const Rect& first_page, const Rect& next_page, PageY p);

	RichContext(const RichStyles& styles, const RichText *text);
	RichContext() {}
};

#include "Para.h"

struct RichPos {
	i32              tabtextparti;
	i32              tabtextpartcount;
	i32              tabposintabtext;
	i32              tabtextlen;

	i32              table; // current level table index (unique in text) or zero if not in table
	Size             tabsize;
	Point            cell;

	i32              tablen;
	i32              posintab;

	i32              celllen;
	i32              posincell;

	i32              parai;
	i32              partcount;
	i32              posinpara;
	i32              paralen;

	i32              level;
	i32              parenttab;

	RichPara::Format format;
	i32              chr;
	RichObj       object;
	Id               field;
	Txt           fieldparam;
	RichPara::CharFormat fieldformat;

#ifdef _DEBUG
	Txt ToTxt() const;
#endif

	RichPos();
};

inline bool InSameTxt(const RichPos& a, const RichPos& b)
{
	return a.table == b.table && (a.table == 0 || a.cell == b.cell);
}

struct RichCaret : PageRect {
	i32  lineascent;
	i32  caretascent;
	i32  caretdescent;
	i32  objectcy;
	i32  objectyd;
	i32  line;
	Rect textpage;

	RichCaret()         { lineascent = caretascent = caretdescent = 0; }
};

struct RichStyle {
	RichPara::Format     format;
	Txt                  name;
	Uuid                 next;

	static Uuid              GetDefaultId();
	static const RichStyle&  GetDefault();

	RichStyle()          { next = GetDefaultId(); }
};

const RichStyle& GetStyle(const RichStyles& s, const Uuid& id);
i32   FindStyleWithName(const RichStyles& style, const Txt& name);

class RichText;

struct RichCellPos;

enum {
	QTF_BODY = 1,
	QTF_ALL_STYLES = 2,
	QTF_NOSTYLES = 4,
	QTF_CRLF = 8,
	QTF_NOCHARSET = 16,
	QTF_NOLANG = 32,

	QTF_ALL = 0xffffffff
};

#include "Txt.h"
#include "Table.h"
#include "Text.h"

struct RichCellPos {
	i32               pos;

	i32               textlen;

	Size              tabsize;
	i32               tabpos;
	i32               tablen;
	i32               cellpos;
	i32               celllen;
	i32               level;
	RichCell::Format  format;
	RichTable::Format tableformat;

#ifdef _DEBUG
	Txt ToTxt() const;
#endif
};

Txt DeQtf(tukk s);
Txt DeQtfLf(tukk s);

Txt QtfFormat(Color c);

struct QtfRichObject {
	RichObj obj;

public:
	Txt ToTxt() const;

	QtfRichObject() {}
	QtfRichObject(const RichObj& o);
};

RichText AsRichText(const RichObj& obj);
Txt   AsQTF(const RichObj& obj);

bool ParseQTF(RichText& txt, tukk qtf, i32 accesskey = 0, uk context = NULL);

RichText ParseQTF(tukk qtf, i32 accesskey = 0, uk context = NULL);

RichText AsRichText(wtukk s, const RichPara::Format& f = RichPara::Format());

Txt   AsQTF(const RichText& doc, u8 charset = CHARSET_UTF8,
               dword options = QTF_BODY|QTF_ALL_STYLES|QTF_CRLF);

inline Txt StylesAsQTF(const RichText& doc, u8 charset = CHARSET_UTF8)
{ return AsQTF(doc, charset, QTF_ALL_STYLES|QTF_CRLF); }

inline Txt BodyAsQTF(const RichText& doc, u8 charset = CHARSET_UTF8)
{ return AsQTF(doc, charset, QTF_BODY|QTF_CRLF); }


void SetQTF(One<RichText>& txt, const Txt& qtf);

i32 GetRichTextScreenStdFontHeight();

enum
{
	ROUNDOFF          = 1 << 20,
	MAX_FONTS         = 10000,
	MAX_DOTS          = 600 * 100,
	MAX_DOT_HEIGHT    = 1200,
	MAX_POINT_HEIGHT  = MAX_DOT_HEIGHT * 3 / 25,
};

inline i32 DotTwips (i32 dots) { return (dots * 12 +  5 * ROUNDOFF +  2) /  5 - ROUNDOFF; }
inline i32 TwipDots (i32 twp)  { return (twp  *  5 + 12 * ROUNDOFF +  6) / 12 - ROUNDOFF; }
inline i32 DotPoints(i32 dots) { return (dots *  3 + 25 * ROUNDOFF + 12) / 25 - ROUNDOFF; }
inline i32 PointDots(i32 pts)  { return (pts  * 25 +  3 * ROUNDOFF +  1) /  3 - ROUNDOFF; }
inline i32 TwipDotSize(i32 twp)  { return IsNull(twp) ? 0 : minmax<i32>(TwipDots(twp), 0, MAX_DOTS); }
inline i32 PointDotHeight(i32 p) { return (minmax<i32>(Nvl(p, 0), 0, MAX_POINT_HEIGHT) * 25 + 5) / 6; }

void  SetRichTextStdScreenZoom(i32 m, i32 d);
Zoom  GetRichTextStdScreenZoom();

const Display& QTFDisplay();
const Display& QTFDisplayVCenter();

class HtmlObjectSaver
{
public:
	virtual ~HtmlObjectSaver() {}

	virtual Txt GetHtml(const RichObj& object)                     { return Null; }
	virtual Txt GetHtml(const RichObj& object, const Txt& link) { return GetHtml(object); }
};

Txt EncodeHtml(const RichText& text, Index<Txt>& css,
                  const VecMap<Txt, Txt>& links,
                  const VecMap<Txt, Txt>& labels,
                  const Txt& path, const Txt& base = Null, Zoom z = Zoom(8, 40),
                  const VecMap<Txt, Txt>& escape = VecMap<Txt, Txt>(),
                  i32 imtolerance = 0);
Txt EncodeHtml(const RichText& text, Index<Txt>& css,
                  const VecMap<Txt, Txt>& links,
                  const VecMap<Txt, Txt>& labels,
                  HtmlObjectSaver& object_saver, Zoom z = Zoom(8, 40),
                  const VecMap<Txt, Txt>& escape = VecMap<Txt, Txt>());
Txt AsCss(Index<Txt>& ss);

Txt MakeHtml(tukk title, const Txt& css, const Txt& body);

inline //BW - no labels
Txt EncodeHtml(const RichText& text, Index<Txt>& css,
                  const VecMap<Txt, Txt>& links,
                  const Txt& path, const Txt& base = Null, Zoom z = Zoom(8, 40)) {
	return EncodeHtml(text, css, links, VecMap<Txt, Txt>(), path, base, z);
}

RichText ParseTrivialHtml(tukk html, Font base_font = Serif(i32(15 * 600 / 96.0)));

struct SimplePageDraw : PageDraw {
	Draw& w;

	virtual Draw& Page(i32);

	SimplePageDraw(Draw& w) : w(w) {}
	virtual ~SimplePageDraw() {}
};

struct PrintPageDraw : PageDraw {
	i32     page;
	Draw&   w;
	NilDraw nw;

	Draw& Page(i32 _page)     { return page == _page ? w : (Draw&)nw; }
	void  SetPage(i32 _page)  { page = _page; }

	PrintPageDraw(Draw& w) : w(w) {}
	virtual ~PrintPageDraw() {}
};

Array<Drawing> RenderPages(const RichText& txt, Size pagesize = Size(3968, 6074));

Txt Pdf(const RichText& txt, Size pagesize = Size(3968, 6074), i32 margin = 200,
           bool pdfa = false, const PdfSignatureInfo *sign = NULL);


}

#endif