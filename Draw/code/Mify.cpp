#include <drx/Draw/Draw.h>

namespace drx {
	
#ifdef CPU_SIMD

Image Minify(const Image& img, i32 nx, i32 ny, bool co)
{
	ASSERT(nx > 0 && ny > 0);
	Size ssz = img.GetSize();
	Size tsz = Size((ssz.cx + nx - 1) / nx, (ssz.cy + ny - 1) / ny);
	ImageBuffer ib(tsz);
	i32 scx0 = ssz.cx / nx * nx;
	auto do_line = [&](i32 ty, f32x4 *b, f32x4 *div) {
		memset((uk )b, 0, tsz.cx * sizeof(f32x4));
		memset((uk )div, 0, tsz.cx * sizeof(f32x4));
		f32x4 v1 = f32all(1);
		f32x4 vnx = f32all(nx);
		i32 yy = ny * ty;
		for(i32 yi = 0; yi < ny; yi++) {
			i32 y = yy + yi;
			if(y < ssz.cy) {
				const RGBA *s = img[yy + yi];
				const RGBA *e = s + scx0;
				const RGBA *e2 = s + ssz.cx;
				f32x4 *t = b;
				f32x4 *d = div;
				while(s < e) {
					f32x4 px = 0;
					for(i32 n = nx; n--;)
						px += LoadRGBAF(s++);
					*t += px;
					*d += vnx;
					t++;
					d++;
				}
				if(s < e2) {
					f32x4 px = 0;
					f32x4 dv = 0;
					while(s < e2) {
						px += LoadRGBAF(s++);
						dv += v1;
					}
					*t += px;
					*d += dv;
					t++;
					d++;
				}
				ASSERT(t == b + tsz.cx);
			}
		}
		f32x4 *s = b;
		f32x4 *d = div;
		RGBA *t = ~ib + ty * tsz.cx;
		RGBA *e = t + tsz.cx;
		while(t < e)
			StoreRGBAF(t++, *s++ / *d++);
	};
	if(co) {
		CoWork cw;
		cw * [&] {
			Buffer<f32x4> div(tsz.cx);
			Buffer<f32x4> b(tsz.cx);
			for(;;) {
				i32 y = cw.Next();
				if(y >= tsz.cy)
					break;
				do_line(y, b, div);
			}
		};
	}
	else {
		Buffer<f32x4> div(tsz.cx);
		Buffer<f32x4> b(tsz.cx);
		for(i32 y = 0; y < tsz.cy; y++)
			do_line(y, b, div);
	}
	return static_cast<Image>(ib);
}

#else

Image Minify(const Image& img, i32 nx, i32 ny, bool co)
{
	ASSERT(nx > 0 && ny > 0);
	Size ssz = img.GetSize();
	Size tsz = Size((ssz.cx + nx - 1) / nx, (ssz.cy + ny - 1) / ny);
	ImageBuffer ib(tsz);
	i32 scx0 = ssz.cx / nx * nx;
	auto do_line = [&](i32 ty, RGBAV *b, i32 *div) {
		i32 yy = ny * ty;
		for(i32 i = 0; i < tsz.cx; i++)
			b[i].Clear();
		memset(div, 0, tsz.cx * sizeof(i32));
		for(i32 yi = 0; yi < ny; yi++) {
			i32 y = yy + yi;
			if(y < ssz.cy) {
				const RGBA *s = img[yy + yi];
				const RGBA *e = s + scx0;
				const RGBA *e2 = s + ssz.cx;
				RGBAV *t = b;
				i32 *dv = div;
				while(s < e) {
					for(i32 n = nx; n--;) {
						t->Put(*s++);
						(*dv)++;
					}
					t++;
					dv++;
				}
				while(s < e2) {
					t->Put(*s++);
					(*dv)++;
				}
			}
		}
		const RGBAV *s = b;
		i32k *dv = div;
		RGBA *it = ~ib + ty * tsz.cx;
		for(i32 x = 0; x < tsz.cx; x++)
			*it++ = (s++)->Get(*dv++);
	};
	if(co) {
		CoWork cw;
		cw * [&] {
			Buffer<i32> div(tsz.cx);
			Buffer<RGBAV> b(tsz.cx);
			for(;;) {
				i32 y = cw.Next();
				if(y >= tsz.cy)
					break;
				do_line(y, b, div);
			}
		};
	}
	else {
		Buffer<i32> div(tsz.cx);
		Buffer<RGBAV> b(tsz.cx);
		for(i32 y = 0; y < tsz.cy; y++)
			do_line(y, b, div);
	}
	return ib;
}

#endif

struct MinifyImageMaker : public ImageMaker {
	Image image;
	i32   nx;
	i32   ny;
	bool  co;

	virtual Txt Key() const;
	virtual Image  Make() const;
};

Txt MinifyImageMaker::Key() const
{
	Txt key;
	RawCat(key, image.GetSerialId());
	RawCat(key, nx);
	RawCat(key, ny);
	// we are not adding co as that is just a hint for actual image making
	return key;
}

Image MinifyImageMaker::Make() const
{
	return Minify(image, nx, ny, co);
}

Image MinifyCached(const Image& img, i32 nx, i32 ny, bool co)
{
	MinifyImageMaker m;
	m.image = img;
	m.nx = nx;
	m.ny = ny;
	m.co = co;
	return MakeImage(m);
}

Image Magnify(const Image& img, i32 nx, i32 ny)
{
	if(nx == 1 && ny == 1)
		return img;
	if(nx == 0 || ny == 0)
		return Image();
	Size sz = img.GetSize();
	bool xdown = nx < 0;
	nx = abs(nx);
	i32 ncx = xdown ? sz.cx / nx : sz.cx * nx;
	ImageBuffer b(ncx, sz.cy * ny);
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	RGBA *t = ~b;
	while(s < e) {
		RGBA *q = t;
		const RGBA *le = s + sz.cx;
		while(s < le) {
			Fill(q, *s, nx);
			q += nx;
			s++;
		}
		for(i32 n = ny - 1; n--;) {
			memcpy(q, t, ncx * sizeof(RGBA));
			q += ncx;
		}
		t = q;
	}
	b.SetResolution(img.GetResolution());
	return static_cast<Image>(b);
}

};