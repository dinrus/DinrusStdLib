#define GUI_WIN
#define GUI_WIN32

namespace drx {

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
	Size  pageSize;
	Size  nativeSize;
	Size  nativeDpi;
	bool  palette:1;
	bool  color16:1;
	bool  is_mono:1;
	i32   native;

	friend class  ImageDraw;
	friend class  FontInfo;
	friend class  Font;

	friend void StaticExitDraw_();

	Point     actual_offset_bak;

	struct Cloff : Moveable<Cloff> {
		Point org;
		HRGN  hrgn;
		Rect  drawingclip;
	};

	Array<Cloff> cloff;
	Rect         drawingclip;

	COLORREF  lastTextColor;
	Color     lastColor;
	HBRUSH    orgBrush;
	HBRUSH    actBrush;
	HPEN      orgPen;
	HPEN      actPen;
	i32       lastPen;
	Color     lastPenColor;

	void   Unselect0();
	void   Cinit();

	void   LoadCaps();
	void   SetPrinterMode();
	void   Reset();
	void   SetOrg();
	friend HPALETTE GetQlibPalette();
	void   DotsMode();

	static void      InitColors();
	
	friend class BackDraw;
	friend class ScreenDraw;
	friend class PrintDraw;

protected:
	dword style;
	HDC   handle, dcMem;
	Point actual_offset;

	SystemDraw();
	void   Init();
	void   InitClip(const Rect& clip);

public:
	static Rect GetVirtualScreenArea();

	static void SetAutoPalette(bool ap);
	static bool AutoPalette();
	bool PaletteMode()                                  { return palette; }

	static void Flush()                                 { GdiFlush(); }

	static Image Win32IconCursor(LPCSTR id, i32 iconsize, bool cursor);
	static HICON IconWin32(const Image& img, bool cursor = false);

	COLORREF GetColor(Color color) const;
	
	Point    GetOffset() const                          { return actual_offset; }

#ifndef TARGET_WINCE
	Point LPtoDP(Point p) const;
	Point DPtoLP(Point p) const;
	Rect  LPtoDP(const Rect& r) const;
	Rect  DPtoLP(const Rect& r) const;
#endif

	void SetColor(Color color);
	void SetDrawPen(i32 width, Color color);

	Size  GetSizeCaps(i32 i, i32 j) const;
	HDC   BeginGdi();
	void  EndGdi();
	HDC   GetHandle()                    { return handle; }
	operator HDC() const                 { return handle; }
	void  Unselect();
	void  Attach(HDC ahandle);
	HDC   Detach();
	
	HDC   GetCompatibleDC()              { return dcMem; }

	SystemDraw(HDC hdc);
	virtual ~SystemDraw();
	
	bool CanSetSurface()                 { return Pixels(); }
};

#ifndef TARGET_WINCE
class WinMetaFile {
	Size size;
	HENHMETAFILE hemf;

	void     Init();

public:
	void         Attach(HENHMETAFILE emf);
	HENHMETAFILE Detach();
	
	void     Set(ukk data, dword len);
	void     Set(const Txt& data)        { Set(~data, data.GetCount()); }
	
	Txt   Get() const;

	operator bool() const                   { return hemf; }
	void     SetSize(const Size& sz)        { size = sz; }
	Size     GetSize() const                { return hemf ? size : Size(0, 0); }

	void     Clear();

	void     Paint(Draw& w, const Rect& r) const;
	void     Paint(Draw& w, i32 x, i32 y, i32 cx, i32 cy) const;

	void     Serialize(Stream& s);

	void     ReadClipboard();
	void     WriteClipboard() const;
	void     Load(tukk file)         { Set(LoadFile(file)); }

	WinMetaFile()                           { Init(); }
	WinMetaFile(HENHMETAFILE hemf);
	WinMetaFile(HENHMETAFILE hemf, Size sz);
	WinMetaFile(tukk file);
	WinMetaFile(uk data, i32 len);
	WinMetaFile(const Txt& data);

	~WinMetaFile()                                { Clear(); }

	HENHMETAFILE GetHEMF() const                  { return hemf; }
};

class WinMetaFileDraw : public SystemDraw {
	Size size;

public:
	bool        Create(HDC hdc, i32 cx, i32 cy, tukk app = NULL, tukk name = NULL, tukk file = NULL);
	bool        Create(i32 cx, i32 cy, tukk app = NULL, tukk name = NULL, tukk file = NULL);
	WinMetaFile Close();

	WinMetaFileDraw() {}
	WinMetaFileDraw(HDC hdc, i32 cx, i32 cy, tukk app = NULL, tukk name = NULL, tukk file = NULL);
	WinMetaFileDraw(i32 cx, i32 cy, tukk app = NULL, tukk name = NULL, tukk file = NULL);
	~WinMetaFileDraw();
};

void DrawWMF(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& wmf);
void DrawWMF(Draw& w, i32 x, i32 y, const Txt& wmf);
Drawing LoadWMF(tukk path, i32 cx, i32 cy);
Drawing LoadWMF(tukk path);

Txt  AsWMF(const Drawing& iw);

#endif

class ScreenDraw : public SystemDraw {
public:
	ScreenDraw(bool ic = false);
	~ScreenDraw();
};

#ifndef TARGET_WINCE
class PrintDraw : public SystemDraw {
public:
	virtual void StartPage();
	virtual void EndPage();

private:
	bool aborted;
	
	void  InitPrinter();
public:
	PrintDraw(HDC hdc, tukk jobname);
	~PrintDraw();
};
#endif

inline bool     BitBlt(HDC ddc, Point d, HDC sdc, const Rect& s, dword rop = SRCCOPY)
{ return BitBlt(ddc, d.x, d.y, s.Width(), s.Height(), sdc, s.left, s.top, rop); }

inline bool     StretchBlt(HDC ddc, const Rect& r, HDC sdc, const Rect& s, dword rop = SRCCOPY)
{ return StretchBlt(ddc, r.left, r.top, r.Width(), r.Height(), sdc, s.left, s.top, s.Width(), s.Height(), rop); }

inline bool     PatBlt(HDC dc, const Rect& r, dword rop = PATCOPY)
{ return PatBlt(dc, r.left, r.top, r.Width(), r.Height(), rop); }

inline void     MoveTo(HDC hdc, Point pt)                         { MoveToEx(hdc, pt.x, pt.y, 0); }
inline void     LineTo(HDC hdc, Point pt)                         { LineTo(hdc, pt.x, pt.y); }

inline void     DrawLine(HDC hdc, Point p, Point q)               { MoveTo(hdc, p); LineTo(hdc, q); }
inline void     DrawLine(HDC hdc, i32 px, i32 py, i32 qx, i32 qy) { MoveToEx(hdc, px, py, 0); LineTo(hdc, qx, qy); }

#ifndef TARGET_WINCE
inline void     DrawArc(HDC hdc, const Rect& rc, Point p, Point q){ Arc(hdc, rc.left, rc.top, rc.right, rc.bottom, p.x, p.y, q.x, q.y); }
#endif
inline void     DrawCircle(HDC hdc, i32 x, i32 y, i32 radius)     { Ellipse(hdc, x - radius, y - radius, x + radius + 1, y + radius + 1); }
inline void     DrawCircle(HDC hdc, Point centre, i32 radius)     { DrawCircle(hdc, centre.x, centre.y, radius); }
inline void     DrawEllipse(HDC hdc, const Rect& rc)              { Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom); }

inline void     DrawRect(HDC hdc, const Rect& rc)                 { Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); }

HDC      ScreenHDC();
HPALETTE GetQlibPalette();

Image Win32Icon(LPCSTR id, i32 iconsize = 0);
Image Win32Icon(i32 id, i32 iconsize = 0);
Image Win32Cursor(LPCSTR id);
Image Win32Cursor(i32 id);
Image Win32DllIcon(tukk dll, i32 ii, bool large);

class BackDraw : public SystemDraw {
public:
	virtual bool  IsPaintingOp(const Rect& r) const;

protected:
	HBITMAP hbmpold;
	HBITMAP hbmp;

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
	Size    size;

	struct  Section {
		HDC     dc;
		HBITMAP hbmp, hbmpOld;
		RGBA   *pixels;

		void Init(i32 cx, i32 cy);
		~Section();
	};

	Section     rgb;
	Section     a;
	SystemDraw  alpha;


	bool    has_alpha;

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

#define GUITARGET_KEYCODES_INCLUDE "Win32Keys.h"


#define GUITARGET_CTRL_TOP_DECLS \
	HWND           hwnd; \
	UDropTarget   *dndtgt; \


#define GUITARGET_CTRL_DECLS_INCLUDE "Win32Ctrl.h"


#define GUITARGET_PASTECLIP_DECLS \
	UDropTarget *dt; \

#define GUITARGET_TOPWINDOW_DECLS_INCLUDE "Win32Top.h"

namespace drx {

inline hash_t GetHashVal(const HWND& hwnd)
{
	return (hash_t)(intptr_t)hwnd;
}
}

#ifdef TARGET_WIN32
#ifndef TARGET_WINCE

#include <shellapi.h>

#endif
#endif

#define GUITARGET_INCLUDE_AFTER "Win32GuiA.h"

#define HAS_TopFrameDraw
