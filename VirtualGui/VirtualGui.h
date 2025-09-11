#ifdef TARGET_POSIX
#include <drx/CtrlCore/stdids.h>
#endif

namespace drx {

#define IMAGECLASS FBImg
#define IMAGEFILE <drx/VirtualGui/FB.iml>
#include <drx/Draw/iml_header.h>

class SystemDraw : public DrawProxy {
public:
	bool    CanSetSurface()                         { return false; }
	static void Flush()                             {}
};

enum KM {
	KM_NONE  = 0x00,

	KM_LSHIFT= 0x01,
	KM_RSHIFT= 0x02,
	KM_LCTRL = 0x04,
	KM_RCTRL = 0x08,
	KM_LALT  = 0x10,
	KM_RALT  = 0x20,

	KM_CAPS  = 0x40,
	KM_NUM   = 0x80,

	KM_CTRL = KM_LCTRL | KM_RCTRL,
	KM_SHIFT = KM_LSHIFT | KM_RSHIFT,
	KM_ALT = KM_LALT | KM_RALT,
};

enum GUI_OPTIONS {
	GUI_SETMOUSECURSOR = 0x01,
	GUI_SETCARET       = 0x02,
};

struct VirtualGui {
	virtual dword       GetOptions();
	virtual Size        GetSize() = 0;
	virtual dword       GetMouseButtons() = 0;
	virtual dword       GetModKeys() = 0;
	virtual bool        IsMouseIn() = 0;
	virtual bool        ProcessEvent(bool *quit) = 0;
	virtual void        WaitEvent(i32 ms) = 0;
	virtual void        WakeUpGuiThread() = 0;
	virtual void        SetMouseCursor(const Image& image);
	virtual void        SetCaret(const Rect& caret);
	virtual void        Quit() = 0;
	virtual bool        IsWaitingEvent() = 0;
	virtual SystemDraw& BeginDraw() = 0;
	virtual void        CommitDraw() = 0;
};

void RunVirtualGui(VirtualGui& gui, Event<> app_main);

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

class ImageDraw : public SImageDraw { // using software renderer
public:
	ImageDraw(Size sz) : SImageDraw(sz) {}
	ImageDraw(i32 cx, i32 cy) : SImageDraw(cx, cy) {}
};

void DrawDragRect(SystemDraw& w, const Rect& rect1, const Rect& rect2, const Rect& clip, i32 n,
                  Color color, zu64 pattern);

class TopWindowFrame;

#define GUITARGET_CTRL_TOP_DECLS   Ctrl *owner_window;

#define GUITARGET_CTRL_DECLS_INCLUDE <drx/VirtualGui/Ctrl.h>

#define GUITARGET_PASTECLIP_DECLS \
	bool dnd; \
	friend struct DnDLoop; \

#define GUITARGET_TOPWINDOW_DECLS_INCLUDE <drx/VirtualGui/Top.h>

class PrinterJob { // Dummy only...
	NilDraw             nil_;
	Vec<i32>         pages;

public:
	Draw&               GetDraw()                       { return nil_; }
	operator            Draw&()                         { return GetDraw(); }
	const Vec<i32>&  GetPages() const                { return pages; }
	i32                 operator[](i32 i) const         { return 0; }
	i32                 GetPageCount() const            { return 0; }

	bool                Execute()                       { return false; }

	PrinterJob& Landscape(bool b = true)                { return *this; }
	PrinterJob& MinMaxPage(i32 minpage, i32 maxpage)    { return *this; }
	PrinterJob& PageCount(i32 n)                        { return *this; }
	PrinterJob& CurrentPage(i32 currentpage)            { return *this; }
	PrinterJob& Name(tukk _name)                 { return *this; }

	PrinterJob(tukk name = NULL)                 {}
	~PrinterJob()                                       {}
};

}

#define GUITARGET_INCLUDE_AFTER <drx/VirtualGui/After.h>