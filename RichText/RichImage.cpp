#include "RichText.h"
#include <drx/Painter/Painter.h>

namespace drx {

#ifdef NEWIMAGE

struct RichImage : public RichObjectType {
	virtual Txt GetTypeName(const Val& v) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz, uk ) const;
	virtual Image  ToImage(z64, const Val& data, Size sz, uk ) const;

	virtual bool   Accept(PasteClip& clip);
	virtual Val  Read(PasteClip& clip);
	virtual Txt GetClipFmts() const;
	virtual Txt GetClip(const Val& data, const Txt& fmt) const;

	typedef RichImage CLASSNAME;
};

Txt RichImage::GetTypeName(const Val& v) const
{
	return "image";
}

// following function pointers are set in CtrlCore (or similar host platform interface package)
static Txt      (*sGetImageClip)(const Image& img, const Txt& fmt);
static bool        (*sAcceptImage)(PasteClip& clip);
static Image       (*sGetImage)(PasteClip& clip);
static tukk (*sClipFmtsImage)();

void InitRichImage(Txt      (*fGetImageClip)(const Image& img, const Txt& fmt),
                   bool        (*fAcceptImage)(PasteClip& clip),
                   Image       (*fGetImage)(PasteClip& clip),
                   tukk (*fClipFmtsImage)())
{
	sGetImageClip = fGetImageClip;
	sAcceptImage = fAcceptImage;
	sGetImage = fGetImage;
	sClipFmtsImage = fClipFmtsImage;
}

bool RichImage::Accept(PasteClip& clip)
{
	return sAcceptImage ? sAcceptImage(clip) : false;
}

Val RichImage::Read(PasteClip& clip)
{
	if(sGetImage)
		return StoreImageAsTxt(sGetImage(clip));
	return Null;
}

Txt RichImage::GetClipFmts() const
{
	if(sClipFmtsImage)
		return sClipFmtsImage();
	return Null;
}

Txt RichImage::GetClip(const Val& data, const Txt& fmt) const
{
	if(sGetImageClip)
		return sGetImageClip(LoadImageFromTxt(data), fmt);
	return Null;
}

Size   RichImage::GetPixelSize(const Val& data) const
{
	return GetImageTxtSize(data);
}

Size   RichImage::GetPhysicalSize(const Val& data) const
{
	Size sz = GetImageTxtDots(data);
	if(sz.cx == 0 || sz.cy == 0)
		sz = 600 * GetPixelSize(data) / 96;
	return sz;
}

void   RichImage::Paint(const Val& data, Draw& w, Size sz, uk ) const
{
	Image x = LoadImageFromTxt(data);
//	Size outsz(min(sz.cx, 4 * x.GetWidth()), min(sz.cy, 4 * x.GetHeight()));
	w.DrawImage(0, 0, sz.cx, sz.cy, x);
}

Image  RichImage::ToImage(z64, const Val& data, Size sz, uk ) const
{
	return Rescale(LoadImageFromTxt(data), sz);
}

INITBLOCK {
	RichObj::Register("image", &Single<RichImage>());
};

RichObj CreateImageObject(const Image& img, i32 cx, i32 cy)
{
	RichObj o = RichObj("image", StoreImageAsTxt(img));
	if(cx || cy)
		o.SetSize(GetRatioSize(o.GetPixelSize(), cx, cy));
	return o;
}

struct RichPNG : public RichObjectType {
	virtual Txt GetTypeName(const Val& v) const;
	virtual Val  Read(const Txt& s) const;
	virtual Txt Write(const Val& v) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz) const;
	virtual Image  ToImage(z64, const Val& data, Size sz, uk ) const;
};

Txt RichPNG::GetTypeName(const Val& v) const
{
	return IsTxt(v) ? "PNG" : "image";
}

Val RichPNG::Read(const Txt& s) const
{
	Image img = StreamRaster::LoadTxtAny(s);
	if(img)
		return img;
	return s;
}

Txt RichPNG::Write(const Val& v) const
{
	if(IsTxt(v))
		return v;
	return StoreImageAsTxt(v);
}

Size RichPNG::GetPhysicalSize(const Val& data) const
{
	if(IsTxt(data))
		return Size(0, 0);
	return Image(data).GetDots();
}

Size RichPNG::GetPixelSize(const Val& data) const
{
	if(IsTxt(data))
		return Size(0, 0);
	return Image(data).GetDots();
}

void RichPNG::Paint(const Val& data, Draw& w, Size sz) const
{
	if(IsTxt(data)) {
		w.DrawRect(sz, SColorFace());
		DrawFrame(w, sz, SColorText());
		w.DrawText(2, 2, "plugin/png отсутствует!");
		return;
	}
	Image x = Image(data);
	Size outsz(min(sz.cx, 4 * x.GetWidth()), min(sz.cy, 4 * x.GetHeight()));
	w.DrawImage(0, 0, outsz.cx, outsz.cy, x);
}

Image RichPNG::ToImage(z64, const Val& data, Size sz, uk ) const
{
	if(IsTxt(data)) {
		ImageAnyDraw iw(sz);
		Paint(data, iw, sz);
		return iw;
	}
	Image x = Image(data);
	Size outsz(min(sz.cx, 4 * x.GetWidth()), min(sz.cy, 4 * x.GetHeight()));
	return Rescale(x, outsz);
}

INITBLOCK {
	RichObj::Register("PNG", &Single<RichPNG>());
};

struct RichRawImage : public RichObjectType {
	virtual Txt GetTypeName(const Val& v) const;
	virtual Val  Read(const Txt& s) const;
	virtual Txt Write(const Val& v) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz, uk ) const;
	virtual Image  ToImage(z64, const Val& data, Size sz, uk ) const;
};

Txt RichRawImage::GetTypeName(const Val& v) const
{
	return "rawimage";
}

Val RichRawImage::Read(const Txt& s) const
{
	return s;
}

Txt RichRawImage::Write(const Val& v) const
{
	return v;
}

Size RichRawImage::GetPhysicalSize(const Val& data) const
{
	Txt s = data;
	TxtStream ss(s);
	One<StreamRaster> r = StreamRaster::OpenAny(ss);
	if(r)
		return r->GetInfo().dots;
	else
	if(IsTxt(data) && IsSVG(~data)) {
		Rectf f = GetSVGBoundingBox(~data);
		Zoom z = GetRichTextStdScreenZoom();
		return z.d * (Size)f.GetSize() / z.m;
	}
	return Size(0, 0);
}

Size RichRawImage::GetPixelSize(const Val& data) const
{
	Txt s = data;
	TxtStream ss(s);
	One<StreamRaster> r = StreamRaster::OpenAny(ss);
	if(r)
		return r->GetSize();
	else
	if(IsTxt(data) && IsSVG(~data)) {
		Rectf f = GetSVGBoundingBox(~data);
		return (Size)f.GetSize();
	}
	return Size(0, 0);
}

void RichRawImage::Paint(const Val& data, Draw& w, Size sz, uk ) const
{
	Txt s = data;
	TxtStream ss(s);
	One<StreamRaster> r = StreamRaster::OpenAny(ss);
	if(r) {
		Size isz = r->GetSize();
		if(GetIsJPGFn() && GetIsJPGFn()(~r) && GetPdfDrawJPEGFn())
			GetPdfDrawJPEGFn()(w, 0, 0, sz.cx, sz.cy, data);
		else
		if(isz.cx * isz.cy > sz.cx * sz.cy) { // conserve memory by scaling down from source
			ImageEncoder m;
			Rescale(m, sz, *r, isz);
			w.DrawImage(0, 0, sz.cx, sz.cy, m);
		}
		else
			w.DrawImage(0, 0, sz.cx, sz.cy, r->GetImage()); // scale up by Draw to give e.g. PDF chance to store unscaled
	}
	else
	if(IsSVG(s))
		w.DrawImage(0, 0, RenderSVGImage(sz, s));
}

Image RichRawImage::ToImage(z64 serial_id, const Val& data, Size sz, uk ) const
{
	Txt s = data;
	TxtStream ss(s);
	One<StreamRaster> r = StreamRaster::OpenAny(ss);
	if(r) {
		struct ImgFormatLoader : ImageMaker {
			z64  serial_id;
			Txt s;

			virtual Txt Key() const  { Txt x; RawCat(x, serial_id); return x; }
			virtual Image  Make() const {
				TxtStream ss(s);
				One<StreamRaster> r = StreamRaster::OpenAny(ss);
				return r ? r->GetImage() : Image();
			}
		} loader;
		loader.serial_id = serial_id;
		loader.s = s;
		return Rescale(MakeImage(loader), sz);
	}
	else
	if(IsTxt(data) && IsSVG(~data))
		return RenderSVGImage(sz, ~data);
	return Null;
}

INITBLOCK {
	RichObj::Register("rawimage", &Single<RichRawImage>());
};

RichObj CreateRawImageObject(const Txt& s, i32 cx, i32 cy)
{
	RichObj o = RichObj("rawimage", s);
	o.InitSize(cx, cy);
	return o;
}

struct RichImlImage : public RichObjectType {
	virtual Txt GetTypeName(const Val& v) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz) const;
	virtual Image  ToImage(z64, const Val& data, Size sz, uk ) const;
	virtual bool   IsText() const;
	
	Image Get(const Val& v) const;
};

Image RichImlImage::Get(const Val& v) const
{
	return GetImlImage((Txt)v);
}

Txt RichImlImage::GetTypeName(const Val& v) const
{
	return "iml";
}

bool RichImlImage::IsText() const
{
	return true;
}

Size RichImlImage::GetPhysicalSize(const Val& data) const
{
	return Get(data).GetSize();
}

Size RichImlImage::GetPixelSize(const Val& data) const
{
	return Get(data).GetSize();
}

void RichImlImage::Paint(const Val& data, Draw& w, Size sz) const
{
	w.DrawImage(0, 0, sz.cx, sz.cy, Get(data));
}

Image RichImlImage::ToImage(z64, const Val& data, Size sz, uk ) const
{
	return Rescale(Get(data), sz);
}

INITBLOCK {
	RichObj::Register("iml", &Single<RichImlImage>());
};

#endif

INITIALIZER(RichImage) {}

}
