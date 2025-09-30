#define GUI_COCOA

namespace drx {

struct PointCG; // represents CGPoint, used to isolate Cocoa/AppKit includes
struct RectCG; // represents CGRect, used to isolate Cocoa/AppKit includes

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
	Vec<Point> offset;
	Vec<Rect>  clip;

	Color  fill = Null;

	void   Push();
	void   Pop();
	
	uk handle;
	uk nsview;
	
	RectCG MakeRectCG(const Rect& r) const;
	void   ClipCG(const Rect& r);
	Rect   GetClip() const         { return clip.GetCount() ? clip.Top() : Rect(-999999, -999999, 999999, 999999); }
	RectCG Convert(i32 x, i32 y, i32 cx, i32 cy);
	RectCG Convert(const Rect& r);
	PointCG Convert(i32 x, i32 y);

	void  Set(Color c);
	void  SetStroke(Color c);

	void Init(uk cgContext, uk nsview);

	void Stroke(i32 width, Color color, bool fill);
	void DoPath(const Point *pp, const Point *end);

	SystemDraw() {}
	
	friend class ImageDraw;
	friend class BackDraw;
	friend class TopFrameDraw;
	friend struct BackDraw__;

public:
	Point    GetOffset() const       { return offset.GetCount() ? offset.Top() : Point(0, 0); }
	void    *GetCGHandle() const      { return handle; }

	bool     CanSetSurface()          { return false; }
	static void Flush() {}

	SystemDraw(uk cgContext, uk nsview);
	~SystemDraw();
};

 
inline void SetSurface(SystemDraw& w, const Rect& dest, const RGBA *pixels, Size srcsz, Point poff)
{ // TODO: Unless we can do this...
	NEVER();
}

class ImageDraw : public SystemDraw {
	ImageBuffer ib;
	
	One<ImageDraw> alpha;

	void Init(i32 cx, i32 cy);

public:
	Draw& Alpha();

	operator Image() const;

	Image GetStraight() const;

	ImageDraw(Size sz);
	ImageDraw(i32 cx, i32 cy);
	~ImageDraw();
};

struct BackDraw__ : public SystemDraw {
	BackDraw__() : SystemDraw() {}
};

class BackDraw : public BackDraw__ { // Dummy only, as we are running in GlobalBackBuffer mode
	Size        size;
	Draw       *painting;
	Point       painting_offset;
	ImageBuffer ib;
	
public:
	virtual bool  IsPaintingOp(const Rect& r) const;

public:
	void  Put(SystemDraw& w, i32 x, i32 y)             {}
	void  Put(SystemDraw& w, Point p)                  { Put(w, p.x, p.y); }

	void Create(SystemDraw& w, i32 cx, i32 cy)         {}
	void Create(SystemDraw& w, Size sz)                { Create(w, sz.cx, sz.cy); }
	void Destroy()                                     {}

	void SetPaintingDraw(Draw& w, Point off)           { painting = &w; painting_offset = off; }

	Point GetOffset() const                            { return Point(0, 0); }

	BackDraw();
	~BackDraw();
};

Image GetIconForFile(tukk path);

#ifndef TARGET_WIN32
#include <drx/CtrlCore/stdids.h>
#endif

#include "CocoCode.h"

#define GUITARGET_KEYCODES_INCLUDE <drx/CtrlCore/CocoKeys.h>

struct CocoTop;
struct MMCtrl;
struct MMImp;

#define GUITARGET_CTRL_TOP_DECLS \
	CocoTop *coco = NULL; \

#define GUITARGET_CTRL_DECLS_INCLUDE <drx/CtrlCore/CocoCtrl.h>

#define GUITARGET_PASTECLIP_DECLS \
uk nspasteboard; \
friend struct MMImp;

#define GUITARGET_TOPWINDOW_DECLS_INCLUDE <drx/CtrlCore/CocoTop.h>

};

#define GUITARGET_INCLUDE_AFTER <drx/CtrlCore/CocoAfter.h>

#define HAS_TopFrameDraw
