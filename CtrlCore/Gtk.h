#define GUI_GTK

#define TARGET_X11 // To keep legacy code happy

#define GDKEY(x) GDK_KEY_##x

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace drx {

class SystemDraw : public Draw {
	virtual dword GetInfo() const;

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


private:
	void  RectPath(const Rect& r);
	void  Push();
	void  Pop();
	Vec<Point> offset;
	Vec<Rect>  clip;
	Vec<Rect>  invalid; // for IsPainting checks, if empty, only clip extents is checked

	cairo_t      *cr;
	
	struct TextGlyph : Moveable<TextGlyph> {
		i32 x;
		i32 y;
		i32 index;
	};

	Font              textfont;
	Color             textink;
	i32               textangle;
	Vec<TextGlyph>    textcache;
	
	SystemDraw() {}
	
	friend class ImageDraw;
	friend class BackDraw;
	friend class TopFrameDraw;

	Rect     GetClip() const;
	void     FlushText();
	
	friend void SetSurface(SystemDraw& w, const Rect& dest, const RGBA *pixels, Size srcsz, Point poff);

public:
	void  SetColor(Color c);
	operator cairo_t*()               { return cr; }

	void     PickInvalid(Vec<Rect>&& inv)   { invalid = pick(inv); }
	Point    GetOffset() const;
	bool     CanSetSurface()          { return true; }
	static void Flush()               {} // TODO?

//	SystemDraw(cairo_t *cr, GdkDrawable *dw/* = NULL*/) : cr(cr), drawable(dw) { (void)drawable; invalid = NULL; }
	SystemDraw(cairo_t *cr) : cr(cr) {}
	~SystemDraw();
};

class ImageDraw : public SystemDraw {
	cairo_surface_t *surface;
	Size             isz;
	
	SystemDraw       alpha;
	cairo_surface_t *alpha_surface;
	bool             del;

	void Init(Size sz);
	void FetchStraight(ImageBuffer& b) const;

public:
	Draw& Alpha();

	operator Image() const;

	Image GetStraight() const;

	ImageDraw(Size sz);
	ImageDraw(i32 cx, i32 cy);
	~ImageDraw();

	ImageDraw(cairo_t *cr, Size sz); // Special variant for chameleon skinning
};

class BackDraw : public SystemDraw {
	Size        size;
	Draw       *painting;
	Point       painting_offset;
	
public:
	virtual bool  IsPaintingOp(const Rect& r) const;

public:
	void  Put(SystemDraw& w, i32 x, i32 y);
	void  Put(SystemDraw& w, Point p)                  { Put(w, p.x, p.y); }

	void Create(SystemDraw& w, i32 cx, i32 cy);
	void Create(SystemDraw& w, Size sz)                { Create(w, sz.cx, sz.cy); }
	void Destroy();

	void SetPaintingDraw(Draw& w, Point off)           { painting = &w; painting_offset = off; }

	BackDraw();
	~BackDraw();
};

struct GdkRect : GdkRectangle {
	operator GdkRectangle *() { return this; }
	GdkRect(const Rect& r);
};

class ImageGdk {
	GdkPixbuf *pixbuf;
	Image      img;

	void Free();

public:
	operator GdkPixbuf *()    { return pixbuf; }

	bool Set(const Image& m);
	          
	ImageGdk();
	ImageGdk(const Image& m);
	~ImageGdk();
};

Txt FilesClipFromUrisFree(gchar **uris);
Txt ImageClipFromPixbufUnref(GdkPixbuf *pixbuf);

GdkAtom GAtom(const Txt& id);

#ifdef GDK_WINDOWING_X11
Vec<i32> GetPropertyInts(GdkWindow *w, tukk property);
#endif

#ifndef TARGET_WIN32
#include <drx/CtrlCore/stdids.h>
#endif

#define GUITARGET_KEYCODES_INCLUDE <drx/CtrlCore/GtkKeys.h>

#define GUITARGET_CTRL_TOP_DECLS \
	GtkWidget            *window; \
	GtkIMContext         *im_context; \
	GtkIMContext         *im_context_simple; \
	GtkIMContext         *im_context_multi; \
	z64                   cursor_id; \
	i32                   id; \

#define GUITARGET_CTRL_DECLS_INCLUDE <drx/CtrlCore/GtkCtrl.h>

#define GUITARGET_PASTECLIP_DECLS \
i32 type; \

#define GUITARGET_TOPWINDOW_DECLS_INCLUDE <drx/CtrlCore/GtkTop.h>

#define GUITARGET_NOSCROLL

i32 rmsecs();

}

#define GUITARGET_INCLUDE_AFTER <drx/CtrlCore/GtkAfter.h>

#define HAS_TopFrameDraw
