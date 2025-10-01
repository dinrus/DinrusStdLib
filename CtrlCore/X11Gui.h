#define GUI_X11
#define TARGET_X11 // To keep legacy code happy

#define Time    XTime
#define Font    XFont
#define Display XDisplay
#define Picture XPicture

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xinerama.h>

#undef Picture
#undef Time
#undef Font
#undef Display

#undef True
#undef False

#define XFalse 0
#define XTrue  1

namespace drx {

extern XDisplay   *Xdisplay;
extern Visual     *Xvisual;
extern i32         Xscreenno;
extern Window      Xroot;
extern Screen     *Xscreen;
extern Colormap    Xcolormap;
extern i32         Xheight;
extern i32         Xwidth;
extern i32         XheightMM;
extern i32         XwidthMM;
extern i32         Xdepth;
extern dword       Xblack;
extern dword       Xwhite;
extern i32         Xconnection;

extern dword   (*Xgetpixel)(i32 r, i32 g, i32 b);

void          InitX11Draw(tukk dispname = NULL);
void          InitX11Draw(XDisplay *display);

void   XError();
void   XError(tukk s);

inline dword GetXPixel(i32 r, i32 g, i32 b) { return (*Xgetpixel)(r, g, b); }
inline dword GetXPixel(Color color)         { return (*Xgetpixel)(color.GetR(), color.GetG(), color.GetB()); }

enum {
	X11_ROP2_ZERO,
	X11_ROP2_AND,
	X11_ROP2_AND_NOT,
	X11_ROP2_COPY,
	X11_ROP2_NOT_AND,
	X11_ROP2_NOP,
	X11_ROP2_XOR,
	X11_ROP2_OR,
	X11_ROP2_NOT_AND_NOT,
	X11_ROP2_NOT_XOR,
	X11_ROP2_INVERT,
	X11_ROP2_OR_NOT,
	X11_ROP2_NOT_COPY,
	X11_ROP2_NOT_OR,
	X11_ROP2_NOT_OR_NOT,
	X11_ROP2_ONE,
};

void SetClip(GC gc, XftDraw *xftdraw, const Vec<Rect>& cl);

class SystemDraw : public Draw {
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
	virtual void SysDrawImageOp(i32 x, i32 y, const Image& img, const Rect& src, Color color);
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

	virtual Size GetNativeDpi() const;
	virtual void BeginNative();
	virtual void EndNative();

	virtual i32  GetCloffLevel() const;

private:
	Size    pageSize;

	SystemDraw();

	friend class  ImageDraw;
	friend class  FontInfo;
	friend class  Font;
	friend class  TopFrameDraw;

	friend void StaticExitDraw_();
	friend Font StdFont();

	Point        actual_offset;
	Point        actual_offset_bak;
	struct Cloff : Moveable<Cloff> {
		Point org;
		i32 clipi;
		i32 offseti;
	};

	Array<Cloff> cloff;
	Rect         drawingclip;
	Vec< Vec<Rect> > clip;
	Vec<Point> offset;

	Drawable   dw;
	GC         gc;
	XftDraw   *xftdraw;

	i32        foreground;
	i32        linewidth;

	void       Init();
	void       Init(const Vec<Rect>& clip, Point offset = Point(0, 0));
	void       CloneClip();

	friend class BackDraw;
	friend class ScreenDraw;

public:
	static void Flush()                                 { XSync(Xdisplay, false); }
	static Image X11Cursor(i32 c);
	static uk CursorX11(const Image& img);

	Point    GetOffset() const                          { return actual_offset; }

	static XftFont *CreateXftFont(Font f, i32 angle);
	XftDraw *GetXftDraw() const               { return xftdraw; }

	Rect  GetClip() const;

	void  SetForeground(Color color);
	void  SetLineStyle(i32 width);
	void  SetClip();

	Drawable GetDrawable() const              { return dw; }
	GC       GetGC() const                    { return gc; }
	const Vec<Rect>& GetClipList() const   { return clip.Top(); }

	SystemDraw(Drawable dw, GC gc, XftDraw *xftdraw, const Vec<Rect>& clip);

	bool CanSetSurface()                      { return true; }
};

Image X11Cursor(i32 c);
uk CursorX11(const Image& img);

class BackDraw : public SystemDraw {
public:
	virtual bool  IsPaintingOp(const Rect& r) const;

protected:
	Pixmap  pixmap;
	Size    size;
	Draw   *painting;
	Point   painting_offset;

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

class ImageDraw : public SystemDraw {
	Size       size;
	SystemDraw alpha;
	bool       has_alpha;

	void Init();
	Image Get(bool pm) const;

public:
	Draw& Alpha();

	operator Image() const;

	Image GetStraight() const;

	ImageDraw(Size sz);
	ImageDraw(i32 cx, i32 cy);
	~ImageDraw();
};

}

#include "X11/keysym.h"


#define GUITARGET_KEYCODES_INCLUDE "X11Keys.h"


#define GUITARGET_CTRL_TOP_DECLS \
	Window         window;


#define GUITARGET_CTRL_DECLS_INCLUDE "X11Ctrl.h"

#define GUITARGET_TOPWINDOW_DECLS_INCLUDE "X11Top.h"

#define GUITARGET_PASTECLIP_DECLS \
	i32          type;

namespace drx {

Txt XAtomName(Atom atom);
Atom   XAtom(tukk name);

Txt      GetProperty(Window w, Atom property, Atom rtype = AnyPropertyType);
Vec<i32> GetPropertyInts(Window w, Atom property, Atom rtype = AnyPropertyType);
Txt      ReadPropertyData(Window w, Atom property, Atom rtype = AnyPropertyType);

Index<Atom>& _NET_Supported();

#include "stdids.h"

#define GUITARGET_INCLUDE_AFTER "X11GuiA.h"

#define HAS_TopFrameDraw

}
