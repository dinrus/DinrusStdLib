#include "IconDes.h"

namespace drx {

void IconDes::Interpolate()
{
	if(!IsCurrent())
		return;
	FinishPaste();
	SaveUndo();
	Slot& c = Current();
	c.base_image = c.image;
	DRX::InterpolateImage(c.image, c.image.GetSize());
	MaskSelection();
}

bool IconDes::BeginTransform()
{
	SaveUndo();
	Refresh();
	SyncShow();
	if(!IsPasting()) {
		if(SelectionRect() == GetImageSize())
			return false;
		Move();
	}
	return true;
}

void IconDes::KeyMove(i32 dx, i32 dy)
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		c.pastepos.x += dx;
		c.pastepos.y += dy;
		MakePaste();
	}
	else {
		Image h = c.image;
		c.image = CreateImage(h.GetSize(), Null);
		DRX::Copy(c.image, Point(dx, dy), h, h.GetSize());
	}
	Sync();
}

void IconDes::MirrorX()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		MirrorHorz(c.paste_image, c.paste_image.GetSize());
		MakePaste();
	}
	else
		MirrorHorz(c.image, c.image.GetSize());
	SyncShow();
}

void IconDes::SymmX()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		if(c.paste_image.GetLength() > 1024 * 1024)
			return;
		Image m = c.paste_image;
		Size sz = m.GetSize();
		MirrorHorz(m, m.GetSize());
		Image h = CreateImage(Size(2 * sz.cx, sz.cy), Null);
		DRX::Copy(h, Point(0, 0), c.paste_image, sz);
		DRX::Copy(h, Point(sz.cx, 0), m, sz);
		c.paste_image = h;
		MakePaste();
	}
	else {
		Size sz = c.image.GetSize();
		if(sz.cx < 2)
			return;
		DRX::Copy(c.image, Point(sz.cx - sz.cx / 2, 0), c.image, Size(sz.cx / 2, sz.cy));
		MirrorHorz(c.image, RectC(sz.cx - sz.cx / 2, 0, sz.cx / 2, sz.cy));
	}
	SyncShow();
}

void IconDes::MirrorY()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		MirrorVert(c.paste_image, c.paste_image.GetSize());
		MakePaste();
	}
	else
		MirrorVert(c.image, c.image.GetSize());
	SyncShow();
}

void IconDes::SymmY()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		if(c.paste_image.GetLength() > 1024 * 1024)
			return;
		Image m = c.paste_image;
		Size sz = m.GetSize();
		MirrorVert(m, m.GetSize());
		Image h = CreateImage(Size(sz.cx, 2 * sz.cy), Null);
		DRX::Copy(h, Point(0, 0), c.paste_image, sz);
		DRX::Copy(h, Point(0, sz.cy), m, sz);
		c.paste_image = h;
		MakePaste();
	}
	else {
		Size sz = c.image.GetSize();
		if(sz.cy < 2)
			return;
		DRX::Copy(c.image, Point(0, sz.cy - sz.cy / 2), c.image, Size(sz.cx, sz.cy / 2));
		MirrorVert(c.image, RectC(0, sz.cy - sz.cy / 2, sz.cx, sz.cy / 2));
	}
	SyncShow();
}

void IconDes::Rotate()
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(BeginTransform()) {
		c.paste_image = RotateClockwise(c.paste_image);
		MakePaste();
	}
	else {
		c.image = RotateClockwise(c.image);
		SetSelect(255);
	}
	SyncShow();
}

void IconDes::SmoothRescale()
{
	if(!IsCurrent())
		return;
	WithRescaleLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Перемерка");
	dlg.cx <<= dlg.cy <<= dlg.method <<= dlg.Breaker();
	Slot& c = Current();
	BeginTransform();
	Image bk = IsPasting() ? c.paste_image : c.image;
	dlg.cx <<= bk.GetWidth();
	dlg.cy <<= bk.GetHeight();
	dlg.keep <<= true;
	dlg.method.Add(Null, "Rylek");
	dlg.method.Add(FILTER_NEAREST, "Nearest");
	dlg.method.Add(FILTER_BILINEAR, "Bilinear");
	dlg.method.Add(FILTER_BSPLINE, "Bspline");
	dlg.method.Add(FILTER_COSTELLO, "Costello");
	dlg.method.Add(FILTER_BICUBIC_MITCHELL, "Bicubic Mitchell");
	dlg.method.Add(FILTER_BICUBIC_CATMULLROM, "Bicubic Catmull Rom");
	dlg.method.Add(FILTER_LANCZOS2, "Lanczos 2");
	dlg.method.Add(FILTER_LANCZOS3, "Lanczos 3");
	dlg.method.Add(FILTER_LANCZOS4, "Lanczos 4");
//	dlg.method.Add(FILTER_LANCZOS5, "Lanczos 5");
	for(;;) {
		Size sz(minmax((i32)~dlg.cx, 1, 9999), minmax((i32)~dlg.cy, 1, 9999));
		Image m = RescaleFilter(bk, sz, ~dlg.method);
		SetRes(m, bk.GetResolution());
		if(IsPasting()) {
			c.paste_image = m;
			MakePaste();
		}
		else {
			c.image = m;
			Refresh();
		}
		SyncImage();
		SyncShow();
		switch(dlg.Run()) {
		case IDCANCEL:
			if(IsPasting()) {
				c.paste_image = bk;
				MakePaste();
			}
			else {
				c.image = bk;
				Refresh();
			}
			SyncImage();
			SyncShow();
			return;
		case IDOK:
			SyncList();
			return;
		}
		if(dlg.keep) {
			if(dlg.cx.HasFocus() && bk.GetWidth() > 0)
				dlg.cy <<= (i32)~dlg.cx * bk.GetHeight() / bk.GetWidth();
			if(dlg.cy.HasFocus() && bk.GetHeight() > 0)
				dlg.cx <<= (i32)~dlg.cy * bk.GetWidth() / bk.GetHeight();
		}
	}
}

Image IconDes::ImageStart()
{
	if(!IsCurrent())
		return CreateImage(Size(1, 1), Black);
	SaveUndo();
	Refresh();
	SyncShow();
	Slot& c = Current();
	if(!IsPasting())
		c.base_image = c.image;
	return IsPasting() ? c.paste_image : c.image;
}

void IconDes::ImageSet(const Image& m)
{
	if(!IsCurrent())
		return;
	Slot& c = Current();
	if(IsPasting()) {
		c.paste_image = m;
		MakePaste();
	}
	else {
		c.image = m;
		MaskSelection();
	}
	Refresh();
	SyncShow();
}

void IconDes::BlurSharpen()
{
	WithSharpenLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Размытие/Чёткость");
	PlaceDlg(dlg);
	dlg.level <<= 0;
	dlg.level <<= dlg.Breaker();
	dlg.passes <<= 1;
	dlg.passes <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		Image m = bk;
		for(i32 q = 0; q < (i32)~dlg.passes; q++)
			m = Sharpen(m, -i32(256 * (double)~dlg.level));
		ImageSet(m);
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

Image Colorize2(const Image& img, Color color, i32 alpha, i32 gray)
{
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	Unmultiply(w);
	RGBA *t = w;
	u8 r0 = color.GetR();
	u8 g0 = color.GetG();
	u8 b0 = color.GetB();
	alpha = alpha + (alpha >> 7);
	if(gray == 0)
		gray = 1;
	while(s < e) {
		i32 ga = Grayscale(*s);
		if(gray >= 255) {
			ga = ga + (ga >> 7);
			t->r = (alpha * (((ga * r0) >> 8) - s->r) >> 8) + s->r;
			t->g = (alpha * (((ga * g0) >> 8) - s->g) >> 8) + s->g;
			t->b = (alpha * (((ga * b0) >> 8) - s->b) >> 8) + s->b;
		}
		else {
			i32 r, g, b;
			if(ga <= gray) {
				r = ga * r0 / gray;
				g = ga * g0 / gray;
				b = ga * b0 / gray;
			}
			else {
				i32 div = 255 - gray;
				i32 ao = ga - gray;
				i32 ac = div - ao;
				r = (ao * s->r + ac * r0) / div;
				g = (ao * s->g + ac * g0) / div;
				b = (ao * s->b + ac * b0) / div;
			}
			t->r = (alpha * (r - s->r) >> 8) + s->r;
			t->g = (alpha * (g - s->g) >> 8) + s->g;
			t->b = (alpha * (b - s->b) >> 8) + s->b;
		}
		t->a = s->a;
		t++;
		s++;
	}
	Premultiply(w);
	w.SetHotSpots(img);
	return static_cast<Image>(w);
}

void IconDes::Colorize()
{
	WithColorize2Layout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Окрасить");
	PlaceDlg(dlg);
	dlg.level.MinMax(0, 1);
	dlg.level <<= 1;
	dlg.level <<= dlg.Breaker();
	dlg.gray.MinMax(0, 1);
	dlg.gray <<= 1;
	dlg.gray <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		ImageSet(Colorize2(bk, CurrentColor(),
		                  (i32)(minmax((double)~dlg.level, 0.0, 1.0) * 255),
		                  (i32)(minmax((double)~dlg.gray, 0.0, 1.0) * 255)));
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

void IconDes::FreeRotate()
{
	WithFreeRotateLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Вращение");
	PlaceDlg(dlg);
	dlg.angle <<= 0;
	dlg.angle <<= dlg.Breaker();
	Image bk = ImageStart();
	Size tsz = bk.GetSize();
	Image src = Magnify(bk, 3, 3);
	for(;;) {
		Image h = DownSample3x(DRX::Rotate(src, (i32)~dlg.angle * 10));
		Size sz = h.GetSize();
		ImageSet(Crop(h, (sz.cx - tsz.cx) / 2, (sz.cy - tsz.cy) / 2, tsz.cx, tsz.cy));
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

void IconDes::Chroma()
{
	WithColorizeLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Хрома");
	PlaceDlg(dlg);
	dlg.level.Max(10);
	dlg.level <<= 1;
	dlg.level <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		ImageSet(DRX::Grayscale(bk, 256 - (i32)(minmax((double)~dlg.level, 0.0, 4.0) * 255)));
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

void IconDes::Contrast()
{
	WithColorizeLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Констраст");
	PlaceDlg(dlg);
	dlg.level.Max(10);
	dlg.level <<= 1;
	dlg.level <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		ImageSet(DRX::Contrast(bk, (i32)(minmax((double)~dlg.level, 0.0, 4.0) * 255)));
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

void IconDes::Alpha()
{
	WithColorizeLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Прозрачность");
	PlaceDlg(dlg);
	dlg.level.Max(4);
	dlg.level <<= 1;
	dlg.level <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		i32 a = (i32)(minmax((double)~dlg.level, 0.0, 4.0) * 255);
		ImageBuffer ib(bk.GetSize());
		RGBA *t = ib;
		const RGBA *s = bk;
		const RGBA *e = bk + bk.GetLength();
		while(s < e) {
			*t = *s;
			t->a = Saturate255((s->a * a) >> 8);
			s++;
			t++;
		}
		ImageSet(ib);
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

void IconDes::Colors()
{
	WithImgColorLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, "Прозрачность");
	PlaceDlg(dlg);
	dlg.r_mul <<= dlg.g_mul <<= dlg.b_mul <<= dlg.a_mul <<= 1;
	dlg.r_add <<= dlg.g_add <<= dlg.b_add <<= dlg.a_add <<= 0;
	dlg.all = true;
	dlg.r_mul <<= dlg.g_mul <<= dlg.b_mul <<= dlg.a_mul <<=
	dlg.r_add <<= dlg.g_add <<= dlg.b_add <<= dlg.a_add <<=
	dlg.all <<= dlg.Breaker();
	Image bk = ImageStart();
	for(;;) {
		bool all = dlg.all;
		dlg.g_mul.Enable(!all);
		dlg.g_add.Enable(!all);
		dlg.b_mul.Enable(!all);
		dlg.b_add.Enable(!all);
		if(all) {
			dlg.g_mul <<= dlg.b_mul <<= ~dlg.r_mul;
			dlg.g_add <<= dlg.b_add <<= ~dlg.r_add;
		}
		ImageBuffer ib(bk.GetSize());
		RGBA *t = ib;
		const RGBA *s = bk;
		const RGBA *e = bk + bk.GetLength();
		i32 r_mul = i32(256 * (double)~dlg.r_mul);
		i32 r_add = i32(256 * (double)~dlg.r_add);
		i32 g_mul = i32(256 * (double)~dlg.g_mul);
		i32 g_add = i32(256 * (double)~dlg.g_add);
		i32 b_mul = i32(256 * (double)~dlg.b_mul);
		i32 b_add = i32(256 * (double)~dlg.b_add);
		i32 a_mul = i32(256 * (double)~dlg.a_mul);
		i32 a_add = i32(256 * (double)~dlg.a_add);
		while(s < e) {
			Unmultiply(t, t, 1);
			t->r = Saturate255(((r_mul * s->r) >> 8) + r_add);
			t->g = Saturate255(((g_mul * s->g) >> 8) + g_add);
			t->b = Saturate255(((b_mul * s->b) >> 8) + b_add);
			t->a = Saturate255(((a_mul * s->a) >> 8) + a_add);
			Premultiply(t, t, 1);
			s++;
			t++;
		}
		ImageSet(ib);
		switch(dlg.Run()) {
		case IDCANCEL:
			ImageSet(bk);
			return;
		case IDOK:
			return;
		}
	}
}

}
