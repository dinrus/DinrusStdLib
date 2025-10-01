#ifndef _IconDes_IconDes_h_
#define _IconDes_IconDes_h_

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/Painter/Painter.h>
#include <drx/RichEdit/RichEdit.h>
#include <X/bmp/bmp.h>

namespace drx {

#define IMAGECLASS IconDesImg
#define IMAGEFILE <drx/IconDes/IconDes.iml>
#include <drx/Draw/iml_header.h>

#define LAYOUTFILE <drx/IconDes/IconDes.lay>
#include <drx/CtrlCore/lay.h>

#define KEYNAMESPACE IconDesKeys
#define KEYGROUPNAME "Icon designer"
#define KEYFILE      <drx/IconDes/IconDes.key>
#include             <drx/CtrlLib/key_header.h>

class AlphaCtrl : public Ctrl {
public:
	virtual void Layout();
	virtual void Paint(Draw& w);
	virtual void LeftDown(Point p, dword keyflags);
	virtual void MouseMove(Point p, dword keyflags);

private:
	Image img;
	Color color;
	i32   alpha;
	bool  mask;
	i32   maskvalue;

	void MakeImage();
	void DoPoint(Point p);

public:
	void SetColor(Color c);
	void Set(i32 a);
	i32  Get() const;
	void Mask(bool m);
	bool IsMask() const                   { return mask; }

	AlphaCtrl();
};

class RGBACtrl : public Ctrl {
public:
	virtual void Paint(Draw& w);
	virtual void Layout();
	virtual void LeftDown(Point p, dword keyflags);
	virtual void MouseMove(Point p, dword keyflags);

private:
	Size  cbox;
	Point cs;
	enum { COLORCOUNT = 216 + 18 + 18 };

	Color         color;
	ColorRampCtrl ramp;
	AlphaCtrl     alpha;
	EditTxt    text;
	FrameRight<Button> setcolor;
	Ptr<Ctrl>     subctrl;

	Color GetColor(i32 i) const;
	void  Ramp();
	void  Alpha();
	void  SyncText();
	i32   DoLayout(Size sz, bool set);

public:
	typedef RGBACtrl CLASSNAME;

	void Set(RGBA c);
	void MaskSet(i32 a);
	RGBA Get() const;
	
	Color GetColor() const;
	i32   GetAlpha() const;

	void Mask(bool b);

	void SubCtrl(Ctrl *c);
	
	i32  GetHeight(i32 cx);

	RGBACtrl();
};

struct IconShow : public Ctrl {
	Image image;
	bool  show_small;
	bool  show_other;

	void Paint(Draw& w);

	IconShow() { BackPaint(); }
};

void   FloodFill(ImageBuffer& img, RGBA color, Point pt, const Rect& rc, i32 tolerance);
void   InterpolateImage(Image& img, const Rect& _rc);
void   MirrorHorz(Image& img, const Rect& rect);
void   MirrorVert(Image& img, const Rect& rect);
Txt PackImlData(const Vec<ImageIml>& image);
Image  DownSample3x(const Image& src);
Image  DownSample2x(const Image& src);

/*
struct IconDraw : ImagePainter {
	IconDraw(Size sz) : ImagePainter(sz, MODE_NOAA) {}
};
*/

struct IconDraw : NilDraw, DDARasterizer {
	RGBA        docolor;
	ImageBuffer image;
	
	virtual void PutHorz(i32 x, i32 y, i32 cx);
	virtual void PutVert(i32 x, i32 y, i32 cy);

	virtual void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	
	operator Image() { return image; }
	
	IconDraw(Size sz) { image.Create(sz); Cy(sz.cy); }
};

class IconDes : public Ctrl {
public:
	virtual void  Layout();
	virtual void  Paint(Draw& w);
	virtual void  LeftDown(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword keyflags);
	virtual void  MouseEnter(Point, dword)                 { SyncStatus(); }
	virtual void  MouseLeave()                             { SyncStatus(); }
	virtual void  LeftUp(Point p, dword keyflags);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual void  RightDown(Point p, dword keyflags);
	virtual bool  Key(dword key, i32 count);
	virtual Image CursorImage(Point p, dword keyflags);

private:
	struct Slot {
		Txt          name;
		Image           image;
		Image           base_image;
		Image           selection;
		Point           pastepos;
		Image           paste_image;
		Txt          undo;
		Txt          redo;
		bool            exp;
		dword           flags = 0;

		Slot();
	};
	
	enum {
		PASTE_TRANSPARENT, PASTE_OPAQUE, PASTE_BACK
	};

	Array<Slot>  slot;
	Slot         dummy;

	i32          magnify;
	i32          pen;
	Point        startpoint;
	Rect         m1refresh;
	void        (IconDes::*tool)(Point p, dword flags);
	bool         doselection = false;
	bool         selectrect = false;
	i32          paste_mode;
	bool         show_other = false;
	bool         show_small = false;

	ScrollBars   sb;
	Scroller     scroller;
	ToolBar      toolbar;

	SplitterFrame  leftpane;
	SplitterFrame  bottompane;

	ParentCtrl     imgs;
	ArrayCtrl      ilist;
	EditTxt     search;

	RGBACtrl       rgbactrl;
	IconShow       iconshow;
	Image          cursor_image;
	Image          fill_cursor, fill_cursor2, fill_cursor3, antifill_cursor;
	bool           single_mode;
	ParentCtrl     single;
	Label          info;
	Button         resize;

	Array<Slot>    removed;

	FrameRight<Label> status;

	i32 syncinglist = 0;
	
	Rect           rect;

	struct TextDlg : WithIconDesTextLayout<TopWindow> {
		typedef TextDlg CLASSNAME;
		
		Font GetFont();
		
		TextDlg();
	};
	
	TextDlg        textdlg;

	void  PenSet(Point p, dword flags);

	void  LineTool(Point p, dword f);
	void  FreehandTool(Point p, dword f);

	void  EllipseTool0(Point p, dword flags, Color inner);
	void  EllipseTool(Point p, dword f);
	void  EmptyEllipseTool(Point p, dword f);

	void  RectTool0(Point p, dword f, bool empty);
	void  RectTool(Point p, dword f);
	void  EmptyRectTool(Point p, dword f);

	void  HotSpotTool(Point p, dword f);

	void  DoFill(i32 tolerance);
	void  FillTool(Point p, dword flags);
	void  Fill2Tool(Point p, dword flags);
	void  Fill3Tool(Point p, dword flags);
	void  AntiFillTool(Point p, dword flags);

	void  Text();
	void  PasteText();
	void  CloseText();

	bool         IsCurrent()            { return !IsNull(ilist.GetKey()); }
	Slot&        Current();

	Image&       CurrentImage();
	void         SetCurrentImage(ImageBuffer& ib);
	RGBA         CurrentColor();
	Size         GetImageSize();
	bool         InImage(i32 x, i32 y);
	bool         InImage(Point p)       { return InImage(p.x, p.y); }


	bool  IsHotSpot() const             { return tool == &IconDes::HotSpotTool; }

	Image MakeIconDesCursor(const Image& arrow, const Image& cmask);

	void  SyncShow();

	void  RefreshPixel(Point p, i32 cx = 1, i32 cy = 1);
	void  RefreshPixel(i32 x, i32 y, i32 cx = 1, i32 cy = 1);
	Point GetPos(Point p);
	void  Set(Point p, RGBA rgba, dword flags);
	void  ApplyDraw(IconDraw& iw, dword flags);
	void  ApplyImage(Image m, dword flags, bool alpha = false);

	void  SyncImage();
	void  Reset();

	bool  IsPasting()                   { return IsCurrent() && !IsNull(Current().pastepos); }
	void  MakePaste();
	void  Paste(const Image& img);
	void  FinishPaste();

	void  SetSelect(i32 a);
	Rect  SelectionRect();
	void  Select();
	void  InvertSelect();
	void  CancelSelect();
	void  SelectRect();
	Image Copy(const Rect& r);
	void  Delete();
	void  Move();
	void  MaskSelection();

	void  SetSb();
	void  Scroll();

	void  SetTool(void (IconDes::*tool)(Point p, dword flags));

	void  SetMagnify(i32 mag);
	void  ZoomIn();
	void  ZoomOut();

	void  SetPen(i32 pen);

	void  MainToolBar(Bar& bar);
	void  SetBar();

	void  SaveUndo();
	void  Undo();
	void  Redo();

	void  DoPaste();
	void  DoCopy();
	void  DoCut();

	void  ColorChanged();

	void  MaskFill(ImageBuffer& ib, RGBA color, i32 mask);
	void  SetColor0(RGBA color);
	void  SetColor();
	void  DoDelete();
	void  Interpolate();
	bool  BeginTransform();
	void  MirrorX();
	void  MirrorY();
	void  SymmX();
	void  SymmY();
	void  FreeRotate();
	void  Rotate();
	void  SmoothRescale();
	void  KeyMove(i32 dx, i32 dy);

	void  BeginResize();
	void  ResizeUp();
	void  ResizeDown();
	void  ResizeUp2();
	void  ResizeDown2();
	void  Upscale();

	void  PlaceDlg(TopWindow& dlg);
	Image ImageStart();
	void  ImageSet(const Image& m);
	void  BlurSharpen();
	void  Colorize();
	void  Chroma();
	void  Contrast();
	void  Alpha();
	void  Colors();
	void  Smoothen();

	void  Search();
	void  GoTo(i32 q);
	void  SyncList();
	void  ListCursor();
	void  PrepareImageDlg(WithImageLayout<TopWindow>& dlg);
	void  PrepareImageSizeDlg(WithImageSizeLayout<TopWindow>& dlg);
	void  SyncDlg(WithImageLayout<TopWindow>& dlg);
	dword GetFlags(WithImageLayout<TopWindow>& dlg);
	Slot& ImageInsert(i32 ii, const Txt& name, const Image& m, bool exp = false);
	Slot& ImageInsert(const Txt& name, const Image& m, bool exp = false);
	void  Slice();
	void  InsertImage();
	void  InsertRemoved(i32 ii);
	void  EditImageSize();
	void  EditImage();
	void  RemoveImage();
	void  Duplicate();
	void  InsertPaste();
	void  InsertFile();
	void  ExportPngs();
	void  InsertIml();
	void  MoveSlot(i32 d);
	void  ChangeSlot(i32 d);
	void  DnDInsert(i32 line, PasteClip& d);
	void  Drag();

	static FileSel& ImgFile();
	static Txt FormatImageName(const Slot& c);

public:
	virtual void ToolEx(Bar& bar);
	virtual void ListMenuEx(Bar& bar);

	void  ListMenu(Bar& bar);
	void  EditBar(Bar& bar);
	void  SettingBar(Bar& bar);
	void  SelectBar(Bar& bar);
	void  ImageBar(Bar& bar);
	void  DrawBar(Bar& bar);

	struct EditPosSlot : Moveable<EditPosSlot> {
		Txt          undo;
		Txt          redo;
		Txt          selection;
		bool            supersampling;
	};

	struct EditPos {
		Vec<EditPosSlot> slot;
		i32                 cursor, sc;
	};

	void   Clear();
	Slot&  AddImage(const Txt& name, const Image& image, bool exp);
	i32    GetCount() const;
	Image  GetImage(i32 ii) const;
	Txt GetName(i32 ii) const;
	bool   GetExport(i32 ii) const;
	dword  GetFlags(i32 ii) const;
	bool   FindName(const Txt& name);

	Txt GetCurrentName() const;
	
	void    SyncStatus();

	EditPos GetEditPos();
	void    SetEditPos(const EditPos& o);

	void    SerializeSettings(Stream& s);
	
	void    SingleMode();
	bool    IsSingleMode() const                  { return single_mode; }

	typedef IconDes CLASSNAME;

	IconDes();
};

struct ImlImage : ImageIml {
	Txt name;
	bool   exp;
};

bool   LoadIml(const Txt& data, Array<ImlImage>& img, i32& format);
Txt SaveIml(const Array<ImlImage>& iml, i32 format, const Txt& eol = "\r\n");

void SetRes(Image& m, i32 resolution);

}

#endif
