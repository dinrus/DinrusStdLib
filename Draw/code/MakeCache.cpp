#include <drx/Draw/Draw.h>

namespace drx {

#define LLOG(x)  //  DLOG(x)

struct scImageMaker : ValMaker {
	const ImageMaker *m;
	bool  paintonly;

	virtual Txt Key() const {
		TxtBuf s;
		s.Cat(typeid(*m).name());
		RawCat(s, paintonly);
		s.Cat(m->Key());
		return Txt(s);
	}
	virtual i32    Make(Val& object) const {
		Image img = m->Make();
		LLOG("ImageMaker " << object.GetSerialId() << ", size " << object.GetSize() << ", paintonly: " << paintonly);
		if(paintonly && !IsNull(img) && img.GetRefCount() == 1)
			SetPaintOnly__(img);
		object = img;
		return (i32)min((size_t) INT_MAX, img.GetLength()) + 64;
	}
};

void SysImageRealized(const Image& img)
{ // Pixel data copied to host platform, no need to keep pixels data in cache if it is paintonly kind
	if(!IsValCacheActive())
		return;
	LLOG("SysImageRealized " << img.GetSize());
	if(img.data && img.data->paintonly) {
		LLOG("Dropping PAINTONLY pixels of image #" << img.GetSerialId() << ", cache size: " << sImageCache().GetSize() << ", img " << img.GetLength());
		DropPixels___(img.data->buffer);
		ValCacheAdjustSize([](const Val& v) -> i32 {
			if(v.Is<Image>()) {
				const Image& img = v.To<Image>();
				return 64 + (~img ? (i32)min((size_t) INT_MAX, img.GetLength()) : 0);
			}
			return -1;
		});
		LLOG("After drop, cache size: " << TheValCache().GetSize());
	}
}

void SysImageReleased(const Image& img)
{ // CtrlCore removed handle for img, have to remove paintonly
	if(!IsValCacheActive())
		return;
	if(!~img) { // No data -> this is paintonly image
		z64 serial_id = img.GetSerialId();
		LLOG("SysImageReleased " << img.GetSerialId() << ", cache size: " << sImageCache().GetSize() << ", count " << sImageCache().GetCount());
		i32 n = ValCacheRemoveOne([&](const Val& v) -> bool {
			return v.Is<Image>() && v.To<Image>().GetSerialId() == serial_id;
		});
		IGNORE_RESULT(n); // suppress warning about unused 'n' without LLOGs
		LLOG("SysImageReleased count: " << n);
		LLOG("SysImageReleased done cache size: " << sImageCache().GetSize() << ", count " << sImageCache().GetCount());
	}
}

void SetMakeImageCacheMax(i32 m)
{
	SetupValCache(m, 0, 0.125);
}

void  SetMakeImageCacheSize(i32 m)
{
	SetMakeImageCacheMax(m);
}

void SweepMkImageCache()
{
	AdjustValCache();
}

Image MakeImage__(const ImageMaker& m, bool paintonly)
{
	scImageMaker cm;
	cm.m = &m;
	cm.paintonly = paintonly;
	return MakeVal(cm);
}

Image MakeImage(const ImageMaker& m)
{
	return MakeImage__(m, false);
}

Image MakeImagePaintOnly(const ImageMaker& m)
{
	return MakeImage__(m, true);
}

class SimpleImageMaker : public ImageMaker {
	Image (*make)(const Image& image);
	Image image;

public:
	virtual Txt Key() const;
	virtual Image  Make() const;

	SimpleImageMaker(const Image& image, Image (*make)(const Image& image))
	:	make(make),image(image) {}
};

Txt SimpleImageMaker::Key() const
{
	Txt key;
	RawCat(key, image.GetSerialId());
	RawCat(key, make);
	return key;
}

Image SimpleImageMaker::Make() const
{
	return (*make)(image);
}

Image MakeImage(const Image& image, Image (*make)(const Image& image))
{
	return MakeImage(SimpleImageMaker(image, make));
}

struct sCachedRescale : public ImageMaker
{
	Rect  src;
	Size  sz;
	Image img;
	i32   filter;

	virtual Txt Key() const {
		TxtBuf h;
		RawCat(h, src.left);
		RawCat(h, src.top);
		RawCat(h, src.right);
		RawCat(h, src.bottom);
		RawCat(h, sz.cx);
		RawCat(h, sz.cy);
		RawCat(h, img.GetSerialId());
		RawCat(h, filter);
		return Txt(h);
	}

	virtual Image Make() const {
		Image im = IsNull(filter) ? Rescale(img, sz, src) : RescaleFilter(img, sz, src, filter);
		ImageBuffer m(im);
		m.SetHotSpot(sz * (img.GetHotSpot() - src.TopLeft()) / src.GetSize());
		m.Set2ndSpot(sz * (img.Get2ndSpot() - src.TopLeft()) / src.GetSize());
		return static_cast<Image>(m);
	}
};

Image CachedRescale(const Image& m, Size sz, const Rect& src, i32 filter)
{
	if(m.GetSize() == sz && src == sz)
		return m;
	sCachedRescale cr;
	cr.sz = sz;
	cr.src = src;
	cr.img = m;
	cr.filter = filter;
	return MakeImage(cr);
}

Image CachedRescale(const Image& m, Size sz, i32 filter)
{
	return CachedRescale(m, sz, m.GetSize(), filter);
}

Image CachedRescalePaintOnly(const Image& m, Size sz, const Rect& src, i32 filter)
{
	if(m.GetSize() == sz)
		return m;
	sCachedRescale cr;
	cr.sz = sz;
	cr.src = src;
	cr.img = m;
	cr.filter = filter;
	return MakeImagePaintOnly(cr);
}

Image CachedRescalePaintOnly(const Image& m, Size sz, i32 filter)
{
	return CachedRescalePaintOnly(m, sz, m.GetSize(), filter);
}

struct sColorize : public ImageMaker
{
	Image img;
	Color color;

	virtual Txt Key() const {
		TxtBuf h;
		RawCat(h, color);
		RawCat(h, img.GetSerialId());
		return Txt(h);
	}

	virtual Image Make() const {
		return SetColorKeepAlpha(img, color);
	}
};

Image CachedSetColorKeepAlpha(const Image& img, Color color)
{
	sColorize m;
	m.img = img;
	m.color = color;
	return MakeImage(m);
}

Image CachedSetColorKeepAlphaPaintOnly(const Image& img, Color color)
{
	sColorize m;
	m.img = img;
	m.color = color;
	return MakeImagePaintOnly(m);
}

}
