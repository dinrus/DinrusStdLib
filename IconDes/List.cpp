#include "IconDes.h"

namespace drx {

Txt IconDes::FormatImageName(const Slot& c)
{
	Size sz = c.image.GetSize();
	Txt r;
	r << c.name << " " << sz.cx << " x " << sz.cy;
	if(c.flags & IML_IMAGE_FLAG_FIXED)
		r << " Fxd";
	else {
		if(c.flags & IML_IMAGE_FLAG_FIXED_COLORS)
			r << " Clr";
		if(c.flags & IML_IMAGE_FLAG_FIXED_SIZE)
			r << " Sz";
	}
	if(c.flags & IML_IMAGE_FLAG_UHD)
		r << " HD";
	if(c.flags & IML_IMAGE_FLAG_DARK)
		r << " Dk";
	if(c.exp)
		r << " X";
	return r;
}

void IconDes::SyncList()
{
	if(syncinglist)
		return;
	syncinglist++;
	i32 sc = ilist.GetScroll();
	i32 q = ilist.GetKey();
	ilist.Clear();
	Txt s = ToUpper((Txt)~search);
	for(i32 i = 0; i < slot.GetCount(); i++) {
		Slot& c = slot[i];
		if(ToUpper(c.name).Find(s) >= 0)
			ilist.Add(i, FormatImageName(c), c.image);
	}
	ilist.ScrollTo(sc);
	ilist.FindSetCursor(q);
	syncinglist--;
}

void IconDes::Search()
{
	SyncList();
}

void IconDes::GoTo(i32 q)
{
	ilist.FindSetCursor(q);
	if(ilist.IsCursor())
		return;
	search <<= Null;
	SyncList();
	ilist.FindSetCursor(q);
}

static wchar sCharFilterCid(wchar c)
{
	return IsAlNum(c) || c == '_' ? c : 0;
}

void IconDes::PlaceDlg(TopWindow& dlg)
{
	Rect r = ilist.GetScreenRect();
	Size sz = dlg.GetSize();
	dlg.NoCenter().SetRect(max(0, r.left + (r.Width() - sz.cx) / 2), r.bottom + 32, sz.cx, sz.cy);
}

void IconDes::PrepareImageDlg(WithImageLayout<TopWindow>& dlg)
{
	CtrlLayoutOKCancel(dlg, "Новый рисунок");
	dlg.cx <<= 16;
	dlg.cy <<= 16;
	if(IsCurrent()) {
		Size sz = GetImageSize();
		dlg.cx <<= sz.cx;
		dlg.cy <<= sz.cy;

		dword flags = IsCurrent() ? Current().flags : 0;
		dlg.fixed <<= !!(flags & IML_IMAGE_FLAG_FIXED);
		dlg.fixed_colors <<= !!(flags & IML_IMAGE_FLAG_FIXED_COLORS);
		dlg.fixed_size <<= !!(flags & IML_IMAGE_FLAG_FIXED_SIZE);

		dlg.uhd <<= !!(flags & IML_IMAGE_FLAG_UHD);
		dlg.dark <<= !!(flags & IML_IMAGE_FLAG_DARK);

		dlg.uhd ^= dlg.dark ^= dlg.exp ^= dlg.fixed_colors ^= dlg.fixed_size ^= dlg.fixed ^= dlg.name ^=
			[&] { dlg.Break(-1000); };
	}
	dlg.name.SetFilter(sCharFilterCid);
}

void IconDes::SyncDlg(WithImageLayout<TopWindow>& dlg)
{
	bool b = !dlg.fixed;
	dlg.fixed_colors.Enable(b);
	dlg.fixed_size.Enable(b);
}

dword IconDes::GetFlags(WithImageLayout<TopWindow>& dlg)
{
	dword flags = 0;
	if(dlg.fixed)
		flags |= IML_IMAGE_FLAG_FIXED;
	if(dlg.fixed_colors)
		flags |= IML_IMAGE_FLAG_FIXED_COLORS;
	if(dlg.fixed_size)
		flags |= IML_IMAGE_FLAG_FIXED_SIZE;
	if(dlg.uhd)
		flags |= IML_IMAGE_FLAG_UHD;
	if(dlg.dark)
		flags |= IML_IMAGE_FLAG_DARK;
	return flags;
}

void IconDes::PrepareImageSizeDlg(WithImageSizeLayout<TopWindow>& dlg)
{
	CtrlLayoutOKCancel(dlg, "Новый рисунок");
	dlg.cx <<= 16;
	dlg.cy <<= 16;
	if(IsCurrent()) {
		Size sz = GetImageSize();
		dlg.cx <<= sz.cx;
		dlg.cy <<= sz.cy;
	}
}

bool CheckName(WithImageLayout<TopWindow>& dlg)
{
	Txt n = ~dlg.name;
	CParser p(n);
	if(p.IsId()) return true;
	Exclamation("Неверное имя!");
	return false;
}

void IconDes::InsertRemoved(i32 q)
{
	if(q >= 0 && q < removed.GetCount()) {
		i32 ii = ilist.IsCursor() ? (i32)ilist.GetKey() : 0;
		slot.Insert(ii) = removed[q];
		removed.Remove(q);
		SyncList();
		GoTo(ii);
	}
}

void SetRes(Image& m, i32 resolution)
{
	ImageBuffer ib(m);
	ib.SetResolution(findarg(resolution, IMAGE_RESOLUTION_STANDARD, IMAGE_RESOLUTION_UHD,
	                         IMAGE_RESOLUTION_NONE)
	                 >= 0 ? resolution : IMAGE_RESOLUTION_STANDARD);
	m = ib;
}

IconDes::Slot& IconDes::ImageInsert(i32 ii, const Txt& name, const Image& m, bool exp)
{
	Slot& c = slot.Insert(ii);
	c.name = name;
	c.image = m;
	c.exp = exp;
	SyncList();
	GoTo(ii);
	return c;
}

IconDes::Slot& IconDes::ImageInsert(const Txt& name, const Image& m, bool exp)
{
	i32 ii = ilist.IsCursor() ? (i32)ilist.GetKey() : 0;
	if(ii == slot.GetCount() - 1)
		ii = slot.GetCount();
	return ImageInsert(ii, name, m, exp);
}

void IconDes::InsertImage()
{
	WithImageLayout<TopWindow> dlg;
	PrepareImageDlg(dlg);
	for(;;) {
		SyncDlg(dlg);
		i32 c = dlg.Run();
		if(c == IDCANCEL)
			return;
		if(c == IDOK && CheckName(dlg))
			break;
	}
	Image m = CreateImage(Size(~dlg.cx, ~dlg.cy), Null);
	ImageInsert(~dlg.name, m, dlg.exp).flags = GetFlags(dlg);
}

void IconDes::Slice()
{
	if(!IsCurrent())
		return;
	Image src = Current().image;
	Size isz = src.GetSize();
	i32 cc = min(isz.cx, isz.cy);
	if(!cc)
		return;
	WithImageLayout<TopWindow> dlg;
	PrepareImageDlg(dlg);
	dlg.name <<= Current().name;
	dlg.cx <<= cc;
	dlg.cy <<= cc;
	dlg.Title("Срез рисунка");
	for(;;) {
		SyncDlg(dlg);
		i32 c = dlg.Run();
		if(c == IDCANCEL)
			return;
		if(c == IDOK && CheckName(dlg))
			break;
	}
	Txt s = ~dlg.name;
	i32 n = 0;
	i32 ii = ilist.GetKey();
	for(i32 y = 0; y < isz.cy; y += (i32)~dlg.cy)
		for(i32 x = 0; x < isz.cx; x += (i32)~dlg.cx) {
			Image m = Crop(src, x, y, ~dlg.cx, ~dlg.cy);
			ImageInsert(++ii, s + AsTxt(n++), m, ~dlg.exp).flags = GetFlags(dlg);
		}
}

void IconDes::Duplicate()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	ImageInsert(c.name, c.image);
	EditImage();
}

void IconDes::InsertPaste()
{
	Image m = ReadClipboardImage();
	if(IsNull(m)) {
		Exclamation("В буфере обмена отсутствует изображение.");
		return;
	}
	SetRes(m, IMAGE_RESOLUTION_STANDARD);
	ImageInsert("", m);
	EditImage();
}

struct FileImage : ImageMaker {
	Txt filename;
	Size   size;

	virtual Txt Key() const { return filename + '/' + AsTxt(size); }
	virtual Image Make() const {
		if(GetFileLength(filename) > 1024 * 1024 * 20)
			return Null;
		Image m = StreamRaster::LoadFileAny(filename);
		Size sz = m.GetSize();
		if(sz.cx > size.cx || sz.cy > size.cy) {
			if(sz.cx * size.cy > sz.cy * size.cx)
				sz = GetRatioSize(sz, size.cx, 0);
			else
				sz = GetRatioSize(sz, 0, size.cy);
			return Rescale(m, sz);
		}
		return m;
	}
};

struct ImgPreview : Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q, Color ink, Color paper, dword style) const {
		if(!IsNull(q)) {
			FileImage im;
			im.size = r.GetSize();
			im.filename = q;
			w.DrawRect(r, SColorPaper);
			Image m = MakeImage(im);
			Point p = r.CenterPos(m.GetSize());
			w.DrawImage(p.x, p.y, m);
		}
	}
};

static void sLoadImage(const Txt& path, Image& result)
{
	if(findarg(ToLower(GetFileExt(path)), ".png", ".gif", ".jpeg", ".jpg") < 0)
		return;
	FileIn in(path);
	if(!in)
		return;
	One<StreamRaster> r = StreamRaster::OpenAny(in);
	if(!r)
		return;
	Size sz = r->GetSize();
	if(sz.cx > 80 || sz.cy > 80)
		return;
	result = r->GetImage();
}

FileSel& IconDes::ImgFile()
{
	static FileSel sel;
	ONCELOCK {
		sel.Type("Файлы рисунков", "*.png *.bmp *.jpg *.jpeg *.gif *.ico");
		sel.AllFilesType();
		sel.Multi();
		sel.WhenIconLazy = sLoadImage;
		sel.Preview(Single<ImgPreview>());
	}
	return sel;
}

i32 CharFilterImageId(i32 c)
{
	return IsAlNum(c) ? c : '_';
}

void IconDes::InsertFile()
{
	if(!ImgFile().ExecuteOpen()) return;
	for(i32 i = 0; i < ImgFile().GetCount(); i++) {
		Txt fn = ImgFile()[i];
		Index<Image> ml;
		if(ToLower(GetFileExt(fn)) == ".ico")
			for(Image m : ReadIcon(LoadFile(fn), true))
				ml.FindAdd(m);
		else {
			Image mm = StreamRaster::LoadFileAny(fn);
			if(IsNull(mm))
				Exclamation(DeQtf(fn) + " не рисунок.");
			else
				ml.Add(mm);
		}
		i32 ii = 0;
		for(Image m : ml) {
			Txt id = Filter(GetFileTitle(fn), CharFilterImageId);
			if(!IsAlpha(*id) && *id != '_')
				id = '_' + id;
			if(ii)
				id << "_" << ii;
			SetRes(m, IMAGE_RESOLUTION_STANDARD);
			ImageInsert(id, m);
			ii++;
		}
	}
}

void IconDes::ExportPngs()
{
	Txt dir = SelectDir();
	if(!dir.IsEmpty())
		for(i32 i = 0; i < GetCount(); i++) {
			dword f = GetFlags(i);
			Txt n = GetName(i);
			if(f & IML_IMAGE_FLAG_UHD)
				n << ".uhd";
			if(f & IML_IMAGE_FLAG_DARK)
				n << ".dark";
			PNGEncoder().SaveFile(AppendFName(dir, n + ".png"), GetImage(i));
		}
}

void IconDes::InsertIml()
{
	Array<ImlImage> iml;
	i32 f;
	if(LoadIml(SelectLoadFile("Файлы Iml\t*.iml"), iml, f))
		for(const ImlImage& m : iml) {
			ImageInsert(m.name, m.image, m.exp).flags = m.flags;
			GoTo((i32)ilist.GetKey() + 1);
		}
}

void IconDes::ListCursor()
{
	SyncImage();
}

void IconDes::EditImageSize()
{
	Slot& c = Current();
	WithImageSizeLayout<TopWindow> dlg;
	PrepareImageSizeDlg(dlg);
	dlg.Breaker(dlg.cx);
	dlg.Breaker(dlg.cy);
	Image img = c.image;
	dlg.cx <<= img.GetWidth();
	dlg.cy <<= img.GetHeight();
	for(;;) {
		switch(dlg.Run()) {
		case IDCANCEL:
			c.image = img;
			Reset();
			return;
		case IDOK:
			Reset();
			SyncList();
			return;
		}
		c.image = CreateImage(Size(minmax((i32)~dlg.cx, 1, 8192), minmax((i32)~dlg.cy, 1, 8192)), Null);
		DRX::Copy(c.image, Point(0, 0), img, img.GetSize());
		Reset();
	}
}

void IconDes::EditImage()
{
	if(!IsCurrent())
		return;
	if(single_mode) {
		EditImageSize();
		return;
	}
	Slot& c = Current();
	WithImageLayout<TopWindow> dlg;
	PrepareImageDlg(dlg);
	dlg.Title("Изображение");
	dlg.Breaker(dlg.cx);
	dlg.Breaker(dlg.cy);
	Image img = c.image;
	dword flags = c.flags;
	bool exp = c.exp;
	Txt name = c.name;

	dlg.cx <<= img.GetWidth();
	dlg.cy <<= img.GetHeight();
	dlg.name <<= c.name;
	dlg.exp <<= c.exp;
	for(;;) {
		SyncDlg(dlg);
		switch(dlg.Run()) {
		case IDCANCEL:
			c.image = img;
			c.flags = flags;
			c.exp = exp;
			c.name = name;
			Reset();
			return;
		case IDOK:
			if(CheckName(dlg)) {
				c.name = ~dlg.name;
				c.exp = ~dlg.exp;
				c.flags = GetFlags(dlg);
				ilist.Set(1, FormatImageName(c));
				i32 q = ilist.GetKey();
				Reset();
				SyncList();
				GoTo(q);
				return;
			}
		}
		c.name = ~dlg.name;
		c.flags = GetFlags(dlg);
		c.image = CreateImage(Size(minmax((i32)~dlg.cx, 1, 8192), minmax((i32)~dlg.cy, 1, 8192)), Null);
		c.exp = ~dlg.exp;
		DRX::Copy(c.image, Point(0, 0), img, img.GetSize());
		SetHotSpots(c.image, img.GetHotSpot(), img.Get2ndSpot());
		Reset();
	}
}

void IconDes::RemoveImage()
{
	if(!IsCurrent() || !PromptYesNo("Удалить текущий рисунок?"))
		return;
	i32 ii = ilist.GetKey();
	while(removed.GetCount() > 12)
		removed.Remove(0);
	Slot& r = removed.Add();
	r = slot[ii];
	if(r.image.GetWidth() <= 128 && r.image.GetHeight() <= 128)
		r.base_image = Rescale(r.image, Size(16, 16));
	else
		r.base_image = IconDesImg::LargeImage();
	slot.Remove(ii);
	ilist.KillCursor();
	SyncList();
	if(ii < slot.GetCount())
		GoTo(ii);
	else
		ilist.GoEnd();
}

void IconDes::ChangeSlot(i32 d)
{
	if(!IsCurrent())
		return;
	i32 c = ilist.GetCursor();
	d = c + d;
	if(d >= 0 && d < ilist.GetCount())
		ilist.SetCursor(d);
}

void IconDes::ListMenu(Bar& bar)
{
	using namespace IconDesKeys;
	if(single_mode)
		bar.Add(IsCurrent(), AK_RESIZE_SINGLE, IconDesImg::Edit(), OTVET(EditImage));
	else {
		bar.Add(AK_INSERT_IMAGE, IconDesImg::Insert(), OTVET(InsertImage));
		bar.Add(IsCurrent(), AK_IMAGE, IconDesImg::Edit(), OTVET(EditImage));
		bar.Add(IsCurrent(), AK_REMOVE_IMAGE, IconDesImg::Remove(), OTVET(RemoveImage));
		bar.Add(IsCurrent(), AK_DUPLICATE, IconDesImg::Duplicate(), OTVET(Duplicate));
		bar.Add(AK_INSERT_CLIP, IconDesImg::InsertPaste(), OTVET(InsertPaste));
		bar.Add(AK_INSERT_FILE, IconDesImg::InsertFile(), OTVET(InsertFile));
		bar.Add(AK_INSERT_IML, IconDesImg::InsertIml(), OTVET(InsertIml));
		bar.Add(AK_EXPORT_PNGS, IconDesImg::ExportPngs(), OTVET(ExportPngs));
		bar.Separator();
		i32 q = ilist.GetKey();
		bar.Add(IsCurrent() && q > 0, AK_MOVE_UP, IconDesImg::MoveUp(),
		        OTVET1(MoveSlot, -1));
		bar.Add(IsCurrent() && q < slot.GetCount() - 1, AK_MOVE_DOWN, IconDesImg::MoveDown(),
		        OTVET1(MoveSlot, 1));
		if(removed.GetCount()) {
			bar.Separator();
			for(i32 i = removed.GetCount() - 1; i >= 0; i--) {
				Slot& r = removed[i];
				bar.Add("Вставить " + FormatImageName(r), r.base_image, OTVET1(InsertRemoved, i));
			}
		}
	}
	bar.Separator();
	EditBar(bar);
	ListMenuEx(bar);
}

void IconDes::ListMenuEx(Bar& bar) {}


void IconDes::Clear()
{
	ilist.Clear();
	slot.Clear();
	Reset();
}

IconDes::Slot& IconDes::AddImage(const Txt& name, const Image& image, bool exp)
{
	i32 q = slot.GetCount();
	Slot& c = slot.Add();
	c.name = name;
	c.image = image;
	c.exp = exp;
	ilist.Add(q, FormatImageName(c), c.image);
	ilist.GoBegin();
	return c;
}

i32 IconDes::GetCount() const
{
	return slot.GetCount();
}

Image IconDes::GetImage(i32 ii) const
{
	return slot[ii].image;
}

Txt IconDes::GetName(i32 ii) const
{
	return slot[ii].name;
}

dword IconDes::GetFlags(i32 ii) const
{
	return slot[ii].flags;
}

bool IconDes::FindName(const Txt& name)
{
	for(i32 i = 0; i < slot.GetCount(); i++)
		if(slot[i].name == name) {
			GoTo(i);
			return true;
		}
	return false;
}

Txt IconDes::GetCurrentName() const
{
	if(ilist.IsCursor())
		return GetName(ilist.GetKey());
	return Txt();
}

bool IconDes::GetExport(i32 ii) const
{
	return slot[ii].exp;
}

void IconDes::MoveSlot(i32 d)
{
	if(!IsCurrent())
		return;
	i32 c = ilist.GetKey();
	d = c + d;
	if(d >= 0 && d < slot.GetCount()) {
		slot.Swap(c, d);
		search <<= Null;
		SyncList();
		GoTo(d);
	}
}

void IconDes::DnDInsert(i32 line, PasteClip& d)
{
	if(GetInternalPtr<ArrayCtrl>(d, "icondes-icon") == &ilist && IsCurrent() &&
	   line >= 0 && line <= slot.GetCount() && d.Accept()) {
		i32 c = ilist.GetKey();
		slot.Move(c, line);
		if(c <= line)
			line--;
		search <<= Null;
		SyncList();
		GoTo(line);
	}
}

void IconDes::Drag()
{
	ilist.DoDragAndDrop(InternalClip(ilist, "icondes-icon"), ilist.GetDragSample(), DND_MOVE);
}

}