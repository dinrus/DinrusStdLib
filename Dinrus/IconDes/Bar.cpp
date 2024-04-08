#include "IconDes.h"

namespace Upp {

#define KEYNAMESPACE IconDesKeys
#define KEYGROUPNAME "Дизайнер иконок"
#define KEYFILE      <Dinrus/IconDes/IconDes.key>
#include             <Dinrus/CtrlLib/key_source.h>

void IconDes::SetPen(int _pen)
{
	pen = _pen;
	SetBar();
}

bool IconDes::Key(dword key, int count)
{
	switch(key) {
	case K_SHIFT_LEFT:  KeyMove(-1, 0); return true;
	case K_SHIFT_RIGHT: KeyMove(1, 0); return true;
	case K_SHIFT_UP:    KeyMove(0, -1); return true;
	case K_SHIFT_DOWN:  KeyMove(0, 1); return true;
	case K_PAGEUP:      ChangeSlot(-1); return true;
	case K_PAGEDOWN:    ChangeSlot(1); return true;
	case K_CTRL_F:      search.SetFocus(); return true;
	}
	return false;
}

void IconDes::SetMagnify(int mag)
{
	if( !IsCurrent() )
		return;

	magnify = minmax(mag, 1, 27);

	sb = Point(0, 0);
	SetSb();
	Refresh();

	SetBar();
}


void IconDes::ZoomIn()
{
	SetMagnify(max(magnify, 1) + 2);
}

void IconDes::ZoomOut()
{
	SetMagnify(max(magnify, 1) - 2);
}

void IconDes::DoPaste()
{
	if(!IsCurrent())
		return;
	Image m = ReadClipboardImage();
	if(m)
		Paste(m);
}

void IconDes::DoCopy()
{
	if(!IsCurrent())
		return;
	WriteClipboardImage(IsPasting() ? Current().paste_image : Copy(SelectionRect()));
}

void IconDes::DoCut()
{
	if(!IsCurrent())
		return;
	DoCopy();
	if(IsPasting()) {
		Current().paste_image.Clear();
		MakePaste();
	}
	else
		Delete();
}

void IconDes::ToolEx(Bar& bar) {}

void IconDes::EditBar(Bar& bar)
{
	Slot *c = IsCurrent() ? &Current() : NULL;
	bar.Add(c, "Вырезать", CtrlImg::cut(), THISBACK(DoCut)).Key(K_DELETE).Key(K_CTRL_X);
	bar.Add(c, "Копировать", CtrlImg::copy(), THISBACK(DoCopy)).Key(K_CTRL_C);
	bar.Add(c, "Вставить", CtrlImg::paste(), THISBACK(DoPaste)).Key(K_CTRL_V);
	bar.Separator();
	bar.Add(c && c->undo.GetCount(), "Отменить", CtrlImg::undo(), THISBACK(Undo))
	   .Key(K_CTRL_Z)
	   .Repeat();
	bar.Add(c && c->redo.GetCount(), "Восстановить", CtrlImg::redo(), THISBACK(Redo))
	   .Key(K_SHIFT_CTRL_Z)
	   .Repeat();
}

void IconDes::SettingBar(Bar& bar)
{
	using namespace IconDesKeys;
	Slot *c = IsCurrent() ? &Current() : NULL;
	bar.Add(c, AK_ZOOM_IN, IconDesImg::ZoomMinus(), THISBACK(ZoomOut))
		.Enable(magnify > 1);
	bar.Add(c, AK_ZOOM_OUT,  IconDesImg::ZoomPlus(), THISBACK(ZoomIn))
		.Enable(magnify < 27);
	bar.Add(AK_PASTE_MODE, IconDesImg::PasteOpaque(),
	        [=,this] { paste_mode = paste_mode == PASTE_OPAQUE ? PASTE_TRANSPARENT : PASTE_OPAQUE; MakePaste(); SetBar(); })
	   .Check(paste_mode == PASTE_OPAQUE);
	bar.Add(AK_PASTE_BACK, IconDesImg::PasteBack(),
	        [=, this] { paste_mode = paste_mode == PASTE_BACK ? PASTE_TRANSPARENT : PASTE_BACK; MakePaste(); SetBar(); })
	   .Check(paste_mode == PASTE_BACK);
}

void IconDes::SelectBar(Bar& bar)
{
	using namespace IconDesKeys;
	Slot *c = IsCurrent() ? &Current() : NULL;
	bar.Add(c, AK_SELECT, IconDesImg::Select(), THISBACK(Select))
	   .Check(doselection);
	bar.Add(c, AK_INVERT_SEL, IconDesImg::InvertSelect(), THISBACK(InvertSelect));
	bar.Add(c, AK_CANCEL_SEL, IconDesImg::CancelSelect(), THISBACK(CancelSelect));
	bar.Add(c, AK_SELECT_MOVE, IconDesImg::SelectRect(), THISBACK(SelectRect))
	   .Check(selectrect);
	bar.Add(c, AK_MOVE, IconDesImg::Move(), THISBACK(Move))
	   .Check(IsPasting());
}

void IconDes::ImageBar(Bar& bar)
{
	using namespace IconDesKeys;
	Slot *c = IsCurrent() ? &Current() : NULL;
	bar.Add(c, AK_SETCOLOR, IconDesImg::SetColor(), THISBACK(SetColor));
	bar.Add(c, AK_EMPTY, IconDesImg::Delete(), THISBACK(DoDelete));
	bar.Add(c, AK_INTERPOLATE, IconDesImg::Interpolate(), THISBACK(Interpolate));
	bar.Add(c, AK_HMIRROR, IconDesImg::MirrorX(), THISBACK(MirrorX));
	bar.Add(c, AK_VMIRROR, IconDesImg::MirrorY(), THISBACK(MirrorY));
	bar.Add(c, AK_HSYM, IconDesImg::SymmX(), THISBACK(SymmX));
	bar.Add(c, AK_VSYM, IconDesImg::SymmY(), THISBACK(SymmY));
	bar.Add(c, AK_ROTATE, IconDesImg::Rotate(), THISBACK(Rotate));
	bar.Add(c, AK_FREE_ROTATE, IconDesImg::FreeRotate(), THISBACK(FreeRotate));
	bar.Add(c, AK_RESCALE, IconDesImg::Rescale(), THISBACK(SmoothRescale));
	bar.Add(c, AK_BLUR, IconDesImg::BlurSharpen(), THISBACK(BlurSharpen));
	bar.Add(c, AK_COLORIZE, IconDesImg::Colorize(), THISBACK(Colorize));
	bar.Add(c, AK_CHROMA, IconDesImg::Chroma(), THISBACK(Chroma));
	bar.Add(c, AK_CONTRAST, IconDesImg::Contrast(), THISBACK(Contrast));
	bar.Add(c, AK_ALPHA, IconDesImg::AlphaI(), THISBACK(Alpha));
	bar.Add(c, AK_COLORS, IconDesImg::Colors(), THISBACK(Colors));
	bar.Add(c, AK_SMOOTHEN, IconDesImg::Smoothen(), THISBACK(Smoothen));
}

void IconDes::DrawBar(Bar& bar)
{
	using namespace IconDesKeys;
	bool notpasting = !IsPasting();
	bar.Add(AK_FREEHAND, IconDesImg::FreeHand(), THISBACK1(SetTool, &IconDes::FreehandTool))
	   .Check(tool == &IconDes::FreehandTool && notpasting);
	bar.Add(AK_LINES, IconDesImg::Lines(), THISBACK1(SetTool, &IconDes::LineTool))
	   .Check(tool == &IconDes::LineTool && notpasting);
	bar.Add(AK_ELLIPSES, IconDesImg::Circles(), THISBACK1(SetTool, &IconDes::EllipseTool))
	   .Check(tool == &IconDes::EllipseTool && notpasting);
	bar.Add(AK_EMPTY_ELLIPSES, IconDesImg::EmptyCircles(), THISBACK1(SetTool, &IconDes::EmptyEllipseTool))
	   .Check(tool == &IconDes::EmptyEllipseTool && notpasting);
	bar.Add(AK_RECTANGLES, IconDesImg::Rects(), THISBACK1(SetTool, &IconDes::RectTool))
	   .Check(tool == &IconDes::RectTool && notpasting);
	bar.Add(AK_EMPTY_RECTANGLES, IconDesImg::EmptyRects(), THISBACK1(SetTool, &IconDes::EmptyRectTool))
	   .Check(tool == &IconDes::EmptyRectTool && notpasting && !selectrect);
	bar.Add(AK_HOTSPOTS, IconDesImg::HotSpot(), THISBACK1(SetTool, &IconDes::HotSpotTool))
	   .Check(tool == &IconDes::HotSpotTool);
	bar.Add(AK_TEXT, IconDesImg::Text(), THISBACK(Text))
	   .Check(textdlg.IsOpen());
	bar.Add("Заполнить", fill_cursor, [=, this] { SetTool(&IconDes::FillTool); })
	   .Check(tool == &IconDes::FillTool && notpasting)
	   .Tip("Заполнить (Shift+Click)");
	bar.Add("Заполнить с малой толерантностью", fill_cursor2, [=, this] { SetTool(&IconDes::Fill2Tool); })
	   .Check(tool == &IconDes::Fill2Tool && notpasting)
	   .Tip("Заполнить с малой толерантностью (Ctrl+Click)");
	bar.Add("Заполнить с большой толерантностью", fill_cursor3, [=, this] { SetTool(&IconDes::Fill3Tool); })
	   .Check(tool == &IconDes::Fill3Tool && notpasting)
	   .Tip("Заполнить с большой толерантностью (Alt+Click)");
	bar.Add("Антизаполнение", antifill_cursor, [=, this] { SetTool(&IconDes::AntiFillTool); })
	   .Check(tool == &IconDes::AntiFillTool && notpasting)
	   .Tip("Антизаполнение (Shift+Ctrl+Click)");
	bar.Separator();
	for(int i = 1; i <= 6; i++)
		bar.Add("Перо " + AsString(i), IconDesImg::Get(IconDesImg::I_Pen1 + i - 1), THISBACK1(SetPen, i))
		   .Check(pen == i)
		   .Key(K_1 + i - 1);
	bar.Separator();
	Slot *c = IsCurrent() ? &Current() : NULL;
	bar.Add(c && c->image.GetLength() < 256 * 256, "Смарт-увеличение 2x",
	        IconDesImg::Upscale(), THISBACK(Upscale))
	   .Key(AK_RESIZEUP2);
	bar.Add(c && c->image.GetLength() < 256 * 256, "Увеличить 2x",
	        IconDesImg::ResizeUp2(), THISBACK(ResizeUp2))
	   .Key(AK_RESIZEUP2);
	bar.Add(c, "Суперсэмпл 2x", IconDesImg::ResizeDown2(), THISBACK(ResizeDown2))
	   .Key(AK_RESIZEDOWN2);
	bar.Add(c && c->image.GetLength() < 256 * 256, "Увеличить 3x",
	        IconDesImg::ResizeUp(), THISBACK(ResizeUp))
       .Key(AK_RESIZEUP3);
	bar.Add(c, "Суперсэмпл 3x", IconDesImg::ResizeDown(), THISBACK(ResizeDown))
	   .Key(AK_RESIZEDOWN3);
	bar.Add("Показать синтетику UHD/Dark", IconDesImg::ShowOther(),
	        [=, this] { show_other = !show_other; show_small = false; SyncShow(); SetBar(); })
	   .Check(show_other);
	bar.Add("Показать уменьшенным", IconDesImg::ShowSmall(),
	        [=, this] { show_small = !show_small; show_other = false; SyncShow(); SetBar(); })
	   .Check(show_small);
	bar.Separator();
	bar.Add(c, AK_SLICE, IconDesImg::Slice(), THISBACK(Slice));
}

void IconDes::MainToolBar(Bar& bar)
{
	EditBar(bar);
	bar.Separator();
	SelectBar(bar);
	bar.Separator();
	ImageBar(bar);
	bar.Separator();
	bar.Add(status, INT_MAX, GetStdFontCy());
	bar.Break();
	DrawBar(bar);
	ToolEx(bar);
	bar.Separator();
	SettingBar(bar);
}

void IconDes::SetBar()
{
	toolbar.Set(THISBACK(MainToolBar));
	SetSb();
	SyncStatus();
}

struct CachedIconImage : public Display {
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
	                   Color ink, Color paper, dword style) const
	{
		w.DrawRect(r, paper);
		Image m = q;
		if(IsNull(m))
			return;
		Size rsz = r.GetSize();
		Size isz = m.GetSize();
		if(isz.cx > 200 || isz.cy > 200)
			m = IconDesImg::LargeImage();
		else
		if(2 * isz.cx <= rsz.cx && 2 * isz.cy <= rsz.cy) {
			int n = min(rsz.cx / isz.cx, rsz.cy / isz.cy);
			m = Magnify(m, n, n); // TODO: Cached!
		}
		else
		if(isz.cx > r.GetWidth() || isz.cy > r.GetHeight())
			m = CachedRescale(m, GetFitSize(m.GetSize(), r.GetSize()));
		Point p = r.CenterPos(m.GetSize());
		w.DrawImage(p.x, p.y, m);
	}
	virtual Size GetStdSize(const Value& q) const
	{
		Image m = q;
		if(IsNull(m))
			return Size(0, 0);
		Size isz = m.GetSize();
		return isz.cx < 200 && isz.cy < 200 ? isz : IconDesImg::LargeImage().GetSize();
	}
};

void IconDes::SerializeSettings(Stream& s)
{
	void (IconDes::*toollist[])(Point p, dword flags) = {
		&IconDes::LineTool,
		&IconDes::FreehandTool,
		&IconDes::EllipseTool,
		&IconDes::EmptyEllipseTool,
		&IconDes::RectTool,
		&IconDes::EmptyRectTool,
		&IconDes::HotSpotTool,
	};

	int version = 5;
	s / version;
	s / magnify;
	s % leftpane % bottompane;
	int i;
	for(i = 0; i < __countof(toollist); i++)
		if(toollist[i] == tool)
			break;
	s % i;
	if(i >= 0 && i < __countof(toollist))
		tool = toollist[i];
	if(version >= 1)
		s % pen;
	SetSb();
	Refresh();
	SetBar();
	if(version >= 2)
		s % ImgFile();
	if(version >= 3) {
		bool b = false;
		s % b % show_small;
	}
	if(version >= 4)
		s % paste_mode;
	if(version >= 5)
		s % show_other;
}

void IconDes::SyncStatus()
{
	Point p = GetPos(GetMousePos() - GetScreenView().TopLeft());
	Size sz = IsCurrent() ? Current().image.GetSize() : Size(0, 0);
	String s;
	if(Rect(sz).Contains(p))
		s << "(" << p.x << ", " << p.y << ") : (" << sz.cx - p.x - 1 << ", " << sz.cy - p.y - 1 << ")";
	if(!IsNull(rect) && (doselection || IsPasting()))
		MergeWith(s, ", ", AsString(rect));
	status.SetLabel(s);
}

IconDes::IconDes()
{
	sb.WhenScroll = THISBACK(Scroll);

	paste_mode = PASTE_TRANSPARENT;
	doselection = false;

	tool = &IconDes::FreehandTool;

	AddFrame(leftpane);
	AddFrame(toolbar);
	AddFrame(bottompane);
	AddFrame(sb);
	AddFrame(ViewFrame());

	leftpane.Left(rgbactrl, 256);
	rgbactrl.SubCtrl(&imgs);

	rgbactrl <<= THISBACK(ColorChanged);

	search.NullText("Поиск (Ctrl+F)");
	search <<= THISBACK(Search);
	search.SetFilter(CharFilterToUpper);

	int cy = EditString::GetStdHeight();
	imgs.Add(search.HSizePos().TopPos(0, cy));
	imgs.Add(ilist.HSizePos().VSizePos(cy, 0));

	ilist.AddKey();
	ilist.AddColumn("", 4);
	ilist.AddColumn("").SetDisplay(Single<CachedIconImage>());
	ilist.NoHeader().NoVertGrid();
	ilist.SetLineCy(max(GetStdFontCy(), DPI(16)));
	ilist.WhenBar = THISBACK(ListMenu);
	ilist.WhenCursor = THISBACK(ListCursor);
	ilist.WhenLeftDouble = THISBACK(EditImage);
	ilist.NoWantFocus();

	ilist.WhenDrag = THISBACK(Drag);
	ilist.WhenDropInsert = THISBACK(DnDInsert);

	search <<= THISBACK(Search);
	search.SetFilter(CharFilterToUpper);

	bottompane.Bottom(iconshow, 64);

	SetBar();
	ColorChanged();
	BackPaint();

	magnify = 13;
	pen = 1;

	single_mode = false;

	status.Width(200);
	status.NoTransparent();
}

}