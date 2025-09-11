#include <drx/Draw/Draw.h>

namespace drx {
	
void  SetHotSpots(Image& m, Point hotspot, Point hotspot2)
{
	ImageBuffer b(m);
	b.SetHotSpot(hotspot);
	b.Set2ndSpot(hotspot2);
	m = b;
}

Image WithHotSpots(const Image& m, Point hotspot, Point hotspot2)
{
	Image h = m;
	SetHotSpots(h, hotspot, hotspot2);
	return h;
}

Image WithHotSpots(const Image& m, i32 x1, i32 y1, i32 x2, i32 y2)
{
	return WithHotSpots(m, Point(x1, y1), Point(x2, y2));
}

Image WithHotSpot(const Image& m, i32 x1, i32 y1)
{
	Image h = m;
	ImageBuffer b(h);
	b.SetHotSpot(Point(x1, y1));
	return static_cast<Image>(b);
}

void  SetResolution(Image& m, i32 res)
{
	ImageBuffer b(m);
	b.SetResolution(res);
	m = b;
}

Image WithResolution(const Image& m, i32 res)
{
	Image h = m;
	SetResolution(h, res);
	return h;
}

Image WithResolution(const Image& m, const Image& res)
{
	return WithResolution(m, res.GetResolution());
}

Image CreateImage(Size sz, const RGBA& rgba)
{
	ImageBuffer ib(sz);
	Fill(~ib, rgba, ib.GetLength());
	return static_cast<Image>(ib);
}

Image CreateImage(Size sz, Color color)
{
	return CreateImage(sz, (RGBA)color);
}

void ScanOpaque(Image& m)
{
	ImageBuffer ib(m);
	ib.SetKind(ib.ScanKind());
	m = ib;
}

force_inline Size DstSrc(ImageBuffer& dest, Point& p, const Image& src, Rect& sr)
{
	if(p.x < 0) {
		sr.left += -p.x;
		p.x = 0;
	}
	if(p.y < 0) {
		sr.top += -p.y;
		p.y = 0;
	}
	sr = sr & src.GetSize();
	Size sz = dest.GetSize() - p;
	sz.cx = min(sz.cx, sr.GetWidth());
	sz.cy = min(sz.cy, sr.GetHeight());
	return sz;
}

void DstSrcOp(ImageBuffer& dest, Point p, const Image& src, const Rect& srect,
                           void (*op)(RGBA *t, const RGBA *s, i32 n))
{
	dest.SetResolution(src.GetResolution());
	Rect sr = srect;
	Size sz = DstSrc(dest, p, src, sr);
	if(sz.cx > 0)
		while(--sz.cy >= 0)
			(*op)(dest[p.y++] + p.x, src[sr.top++] + sr.left, sz.cx);
}

void Copy(ImageBuffer& dest, Point p, const Image& src, const Rect& srect)
{
	DstSrcOp(dest, p, src, srect, Copy);
}

void Over(ImageBuffer& dest, Point p, const Image& src, const Rect& srect)
{
	DstSrcOp(dest, p, src, srect, AlphaBlend);
}

Image GetOver(const Image& dest, const Image& src)
{
	Image r = dest;
	Over(r, src);
	return r;
}

Image Copy(const Image& src, const Rect& srect)
{
	ImageBuffer ib(srect.GetSize());
	Copy(ib, Point(0, 0), src, srect);
	return static_cast<Image>(ib);
}

void Fill(ImageBuffer& dest, const Rect& rect, RGBA color)
{
	Rect r = dest.GetSize() & rect;
	i32 cx = r.GetWidth();
	if(cx)
		for(i32 y = r.top; y < r.bottom; y++)
			Fill(dest[y] + r.left, color, cx);
}

void OverStraightOpaque(ImageBuffer& dest, Point p, const Image& src, const Rect& srect)
{
	DstSrcOp(dest, p, src, srect, AlphaBlendStraightOpaque);
}

void  Copy(Image& dest, Point p, const Image& _src, const Rect& srect)
{
	Image src = _src;
	ImageBuffer b(dest);
	Copy(b, p, src, srect);
	dest = b;
}

void  Over(Image& dest, Point p, const Image& _src, const Rect& srect)
{
	Image src = _src;
	ImageBuffer b(dest);
	Over(b, p, src, srect);
	dest = b;
}

void  Over(Image& dest, const Image& _src)
{
	Over(dest, Point(0, 0), _src, _src.GetSize());
}

void Fill(Image& dest, const Rect& rect, RGBA color)
{
	ImageBuffer b(dest);
	Fill(b, rect, color);
	dest = b;
}

void  OverStraightOpaque(Image& dest, Point p, const Image& _src, const Rect& srect)
{
	Image src = _src;
	ImageBuffer b(dest);
	OverStraightOpaque(b, p, src, srect);
	dest = b;
}

void Crop(RasterEncoder& tgt, Raster& img, const Rect& rc)
{
	Rect r = rc & img.GetSize();
	tgt.Create(r.Size(), img);
	for(i32 y = r.top; y < r.bottom; y++)
		tgt.WriteLine(~img[y] + r.left);
}

Image Crop(const Image& img, const Rect& rc)
{
	if(rc.left == 0 && rc.top == 0 && rc.Size() == img.GetSize())
		return img;
	if((rc & img.GetSize()).IsEmpty())
		return Image();
	ImageRaster  src(img);
	ImageEncoder tgt;
	Crop(tgt, src, rc);
	return WithResolution(tgt, img);
}

Image AddMargins(const Image& img, i32 left, i32 top, i32 right, i32 bottom, RGBA color)
{
	Size sz = img.GetSize();
	ImageBuffer ib(sz.cx + left + right, sz.cy + top + bottom);
	Fill(ib, color, ib.GetLength());
	Copy(ib, Point(left, top), img, img.GetSize());
	return static_cast<Image>(ib);
}

Image Crop(const Image& img, i32 x, i32 y, i32 cx, i32 cy)
{
	return Crop(img, RectC(x, y, cx, cy));
}

bool IsUniform(const RGBA *s, RGBA c, i32 add, i32 n)
{
	while(n-- > 0) {
		if(*s != c)
			return false;
		s += add;
	}
	return true;
}

Rect FindBounds(const Image& m, RGBA bg)
{
	Size isz = m.GetSize();
	Rect r = isz;
	for(r.top = 0; r.top < isz.cy && IsUniform(m[r.top], bg, 1, isz.cx); r.top++)
		;
	for(r.bottom = isz.cy; r.bottom > r.top && IsUniform(m[r.bottom - 1], bg, 1, isz.cx); r.bottom--)
		;
	if(r.bottom <= r.top)
		return Null;
	i32 h = r.GetHeight();
	const RGBA *p = m[r.top];
	for(r.left = 0; r.left < isz.cy && IsUniform(p + r.left, bg, isz.cx, h); r.left++)
		;
	for(r.right = isz.cx; r.right > r.left && IsUniform(p + r.right - 1, bg, isz.cx, h); r.right--)
		;
	return r;
}

void AutoCrop(Image *m, i32 count, RGBA bg)
{
	if(!count)
		return;
	Rect r = FindBounds(m[0]);
	for(i32 i = 1; i < count; i++)
		r.Union(FindBounds(m[i], bg));
	for(i32 i = 0; i < count; i++) {
		Point p1 = m[i].GetHotSpot() - r.TopLeft();
		Point p2 = m[i].Get2ndSpot() - r.TopLeft();
		m[i] = WithHotSpots(Crop(m[i], r), p1.x, p1.y, p2.x, p2.y);
	}
}

Image AutoCrop(const Image& m, RGBA bg)
{
	Image mm = m;
	AutoCrop(&mm, 1, bg);
	return mm;
}

void ClampHotSpots(Image& m)
{
	Point p1 = m.GetHotSpot();
	Point p2 = m.Get2ndSpot();
	Rect clamp = m.GetSize();
	Point p1a = clamp.Bind(p1);
	Point p2a = clamp.Bind(p2);
	if(p1 != p1a || p2 != p2a)
		SetHotSpots(m, p1a, p2a);
}

Image ColorMask(const Image& src, Color key)
{
	ImageBuffer ib(src.GetSize());
	ib.SetResolution(src.GetResolution());
	const RGBA *s = src;
	const RGBA *e = src + src.GetLength();
	RGBA *t = ~ib;
	u8 kr = key.GetR();
	u8 kg = key.GetG();
	u8 kb = key.GetB();
	while(s < e) {
		if(s->r == kr && s->g == kg && s->b == kb)
			*t++ = RGBAZero();
		else
			*t++ = *s;
		s++;
	}
	ib.SetHotSpots(src);
	return static_cast<Image>(ib);
}

void CanvasSize(RasterEncoder& tgt, Raster& img, i32 cx, i32 cy)
{
	tgt.Create(cx, cy, img);
	i32 ccx = min(img.GetWidth(), cx);
	i32 ccy = min(img.GetHeight(), cy);
	for(i32 y = 0; y < ccy; y++) {
		memcpy(~tgt, img[y], ccx * sizeof(RGBA));
		memset(~tgt + ccx, 0, (cx - ccx) * sizeof(RGBA));
		tgt.WriteLine();
	}
	for(i32 y = cy - ccy; --y >= 0;) {
		memset(~tgt, 0, cx * sizeof(RGBA));
		tgt.WriteLine();
	}
}

Image CanvasSize(const Image& img, i32 cx, i32 cy)
{
	ImageRaster  src(img);
	ImageEncoder tgt;
	CanvasSize(tgt, src, cx, cy);
	return WithResolution(tgt, img);
}

Image AssignAlpha(const Image& img, const Image& alpha)
{
	Size sz = Size(min(img.GetWidth(), alpha.GetWidth()),
	               min(img.GetHeight(), alpha.GetHeight()));
	if(sz.cx == 0 || sz.cy == 0)
		return Image();
	ImageBuffer ib(sz);
	for(i32 y = 0; y < sz.cy; y++) {
		const RGBA *s = img[y];
		const RGBA *e = s + sz.cx;
		const RGBA *a = alpha[y];
		RGBA *t = ib[y];
		while(s < e) {
			*t = *s++;
			(t++)->a = (a++)->a;
		}
	}
	ib.SetHotSpots(img);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

i32   EqualightCh(i32 c, i32 l, i32 h)
{
	return Saturate255((c - l) * 255 / (h - l) + l);
}

Image Equalight(const Image& img, i32 thold)
{
	i32 histogram[256];
	ZeroArray(histogram);
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	while(s < e) {
		histogram[Grayscale(*s)]++;
		s++;
	}
	size_t n = (thold * img.GetLength()) >> 8;
	i32 h = 255;
	i32 l = 0;
	while(l < h) {
		if(n < 0)
			break;
		n -= histogram[l++];
		if(n < 0)
			break;
		n -= histogram[h--];
	}
	if(l >= h)
		return img;
	ImageBuffer w(img.GetSize());
	RGBA *t = w;
	s = ~img;
	while(s < e) {
		t->r = EqualightCh(s->r, l, h);
		t->g = EqualightCh(s->g, l, h);
		t->b = EqualightCh(s->b, l, h);
		t->a = s->a;
		s++;
		t++;
	}
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

Image Grayscale(const Image& img)
{
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	RGBA *t = w;
	while(s < e) {
		i32 q = Grayscale(*s);
		t->r = q;
		t->g = q;
		t->b = q;
		t->a = s->a;
		t++;
		s++;
	}
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

Image Grayscale(const Image& img, i32 amount)
{
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	RGBA *t = w;
	i32 na = 256 - amount;
	while(s < e) {
		i32 q = Grayscale(*s);
		t->r = Saturate255((amount * q + na * s->r) >> 8);
		t->g = Saturate255((amount * q + na * s->g) >> 8);
		t->b = Saturate255((amount * q + na * s->b) >> 8);
		t->a = s->a;
		t++;
		s++;
	}
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

Image Colorize(const Image& img, Color color, i32 alpha)
{
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	Unmultiply(w);
	RGBA *t = w;
	u8 r = color.GetR();
	u8 g = color.GetG();
	u8 b = color.GetB();
	alpha = alpha + (alpha >> 7);
	while(s < e) {
		i32 ga = Grayscale(*s);
		ga = ga + (ga >> 7);
		t->r = (alpha * (((ga * r) >> 8) - s->r) >> 8) + s->r;
		t->g = (alpha * (((ga * g) >> 8) - s->g) >> 8) + s->g;
		t->b = (alpha * (((ga * b) >> 8) - s->b) >> 8) + s->b;
		t->a = s->a;
		t++;
		s++;
	}
	Premultiply(w);
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

Image DarkTheme(const Image& img)
{
	if(IsNull(img))
		return img;

	Image simg = Unmultiply(img);
	const RGBA *s = simg.begin();
	const RGBA *e = simg.end();

	ImageBuffer ib(img.GetSize());
	RGBA *t = ~ib;
	
	while(s < e) {
		RGBA h = *s++;
		u8 a = h.a;
		h.a = 255;
		h = DarkTheme(Color(h));
		h.a = a;
		*t++ = h;
	}
	
	Premultiply(ib);

	ib.SetHotSpots(img);
	ib.SetResolution(img.GetResolution());

	return static_cast<Image>(ib);
}

inline
u8 ContrastCh(i32 amount, i32 ch)
{
	return Saturate255(128 + (amount * (ch - 128) >> 8));
}

Image Contrast(const Image& img, i32 amount)
{
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	Unmultiply(w);
	RGBA *t = w;
	while(s < e) {
		t->r = ContrastCh(amount, s->r);
		t->g = ContrastCh(amount, s->g);
		t->b = ContrastCh(amount, s->b);
		t->a = s->a;
		t++;
		s++;
	}
	Premultiply(w);
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

void sLine(RGBA *t, i32 cx, const RasterLine l[3], ImageFilter9& filter)
{
	RGBA h[3][3];
	const RGBA *x[3];
	x[0] = h[0];
	x[1] = h[1];
	x[2] = h[2];
	if(cx == 1) {
		h[0][0] = l[0][0]; h[0][1] = l[0][0]; h[0][2] = l[0][0];
		h[1][0] = l[1][0]; h[1][1] = l[1][0]; h[1][2] = l[1][0];
		h[2][0] = l[2][0]; h[2][1] = l[2][0]; h[2][2] = l[2][0];
		*t = filter(x);
		return;
	}
	h[0][0] = l[0][0]; h[0][1] = l[0][0]; h[0][2] = l[0][1];
	h[1][0] = l[1][0]; h[1][1] = l[1][0]; h[1][2] = l[1][1];
	h[2][0] = l[2][0]; h[2][1] = l[2][0]; h[2][2] = l[2][1];
	*t++ = filter(x);
	for(i32 i = 1; i < cx - 1; i++) {
		x[0] = ~l[0] + i - 1;
		x[1] = ~l[1] + i - 1;
		x[2] = ~l[2] + i - 1;
		*t++ = filter(x);
	}
	h[0][0] = l[0][cx - 2]; h[0][1] = l[0][cx - 1]; h[0][2] = l[0][cx - 1];
	h[1][0] = l[1][cx - 2]; h[1][1] = l[1][cx - 1]; h[1][2] = l[1][cx - 1];
	h[2][0] = l[2][cx - 2]; h[2][1] = l[2][cx - 1]; h[2][2] = l[2][cx - 1];
	x[0] = h[0];
	x[1] = h[1];
	x[2] = h[2];
	*t++ = filter(x);
}

void  Filter(RasterEncoder& target, Raster& src, ImageFilter9& filter)
{
	Size sz = src.GetSize();
	target.Create(sz, src);
	if(sz.cy < 1)
		return;
	RasterLine l[3];
	if(sz.cy == 1) {
		l[0] = src[0];
		l[1] = src[0];
		l[2] = src[0];
		sLine(target, sz.cx, l, filter);
		return;
	}
	l[0] = src[0];
	l[1] = src[0];
	l[2] = src[1];
	sLine(target, sz.cx, l, filter);
	target.WriteLine();
	for(i32 y = 1; y < sz.cy - 1; y++) {
		l[0] = pick(l[1]);
		l[1] = pick(l[2]);
		l[2] = src[y + 1];
		sLine(target, sz.cx, l, filter);
		target.WriteLine();
	}
	l[0] = pick(l[1]);
	l[1] = pick(l[2]);
	l[2] = src[sz.cy - 1];
	sLine(target, sz.cx, l, filter);
	target.WriteLine();
}

Image Filter(const Image& img, ImageFilter9& filter)
{
	ImageEncoder tgt;
	ImageRaster src(img);
	Filter(tgt, src, filter);
	return WithResolution(tgt, img);
}

struct RGBAI {
	i32 r, g, b, a;

	RGBAI() { r = g = b = a= 0; }
};

static void sGetS(RGBA q, RGBAI& p, i32 mul)
{
	p.r += mul * q.r;
	p.g += mul * q.g;
	p.b += mul * q.b;
	p.a += mul * q.a;
}

struct sSharpenFilter : ImageFilter9 {
	i32 amount;

	virtual RGBA operator()(const RGBA **mx);
};

RGBA sSharpenFilter::operator()(const RGBA **mx)
{
	RGBAI q;
	sGetS(mx[0][0], q, 7);
	sGetS(mx[0][1], q, 9);
	sGetS(mx[0][2], q, 7);
	sGetS(mx[1][0], q, 9);
	sGetS(mx[1][2], q, 9);
	sGetS(mx[2][0], q, 7);
	sGetS(mx[2][1], q, 9);
	sGetS(mx[2][2], q, 7);
	const RGBA& s = mx[1][1];
	RGBA t;
	i32 na = 256 + amount;
	t.b = Saturate255((na * (s.b << 6) - amount * q.b) >> 14);
	t.g = Saturate255((na * (s.g << 6) - amount * q.g) >> 14);
	t.r = Saturate255((na * (s.r << 6) - amount * q.r) >> 14);
	t.a = Saturate255((na * (s.a << 6) - amount * q.a) >> 14);
	return t;
}

void Sharpen(RasterEncoder& target, Raster& src, i32 amount)
{
	Size sz = src.GetSize();
	target.Create(sz, src);
	sSharpenFilter f;
	f.amount = amount;
	Filter(target, src, f);
}

Image Sharpen(const Image& img, i32 amount)
{
	ImageEncoder tgt;
	ImageRaster src(img);
	Sharpen(tgt, src, amount);
	return WithResolution(tgt, img);
}

struct sEtchFilter : ImageFilter9 {
	virtual RGBA operator()(const RGBA **mx);
};

RGBA sEtchFilter::operator()(const RGBA **mx)
{
	RGBA t;
	RGBA s = mx[1][1];
	if(s.a > 0x80 && s.r + s.g + s.b < 500) {
		t.r = t.g = t.b = 128;
		t.a = s.a;
		return t;
	}
	s = mx[0][0];
	if(s.a > 0x80 && s.r + s.g + s.b < 500) {
		t.r = t.g = t.b = 255;
		t.a = s.a;
		return t;
	}
	Zero(t);
	return t;
}

Image Etched(const Image& img)
{
	sEtchFilter ef;	
	return Premultiply(Filter(Unmultiply(img), ef));
}

Image SetColorKeepAlpha(const Image& img, Color c)
{
	RGBA rgba = c;
	const RGBA *s = ~img;
	const RGBA *e = s + img.GetLength();
	ImageBuffer w(img.GetSize());
	RGBA *t = w;
	while(s < e) {
		*t = rgba;
		(t++)->a = (s++)->a;
	}
	Premultiply(w);
	w.SetHotSpots(img);
	w.SetResolution(img.GetResolution());
	return static_cast<Image>(w);
}

Image CreateHorzFadeOut(Size sz, Color color)
{
	ImageBuffer ib(sz);
	RGBA c = color;
	for(i32 q = 0; q < sz.cx; q++) {
		c.a = q * 255 / sz.cx;
		RGBA *t = ~ib + q;
		for(i32 n = sz.cy; n > 0; n--) {
			*t = c;
			t += sz.cx;
		}
	}
	Premultiply(ib);
	return static_cast<Image>(ib);
}

struct FadeOutMaker : ImageMaker {
	Size  sz;
	Color color;

	virtual Txt Key() const {
		char h[sizeof(Size) + sizeof(Color)];
		memcpy(h, &sz, sizeof(sz));
		memcpy(h + sizeof(Size), &color, sizeof(Color));
		return Txt(h, sizeof(h));
	}

	virtual Image  Make() const {
		return CreateHorzFadeOut(sz, color);
	}
};

Image  HorzFadeOut(Size sz, Color color)
{
	FadeOutMaker m;
	m.sz = sz;
	m.color = color;
	return MakeImage(m);
}

Image HorzFadeOut(i32 cx, i32 cy, Color color)
{
	return HorzFadeOut(Size(cx, cy), color);
}

void SetNormalizedHotSpots(ImageBuffer& ib, i32 x1, i32 y1, i32 x2, i32 y2)
{
	Rect r(x1, y1, x2, y2);
	r.Normalize();
	ib.SetHotSpot(r.TopLeft());
	ib.Set2ndSpot(r.BottomRight());
}

Image  RotateClockwise(const Image& img)
{
	Size sz = img.GetSize();
	ImageBuffer ib(sz.cy, sz.cx);
	for(i32 x = 0; x < sz.cx; x++)
		for(i32 y = 0; y < sz.cy; y++)
			ib[x][y] = img[sz.cy - y - 1][x];
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, sz.cy - p1.y - 1, p1.x, sz.cy - p2.y - 1, p2.x);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image  RotateAntiClockwise(const Image& img)
{
	Size sz = img.GetSize();
	ImageBuffer ib(sz.cy, sz.cx);
	for(i32 x = 0; x < sz.cx; x++)
		for(i32 y = 0; y < sz.cy; y++)
			ib[x][y] = img[y][sz.cx - x - 1];
	
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, p1.y, sz.cx - p1.x - 1, p2.y, sz.cx - p2.x - 1);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image Rotate180(const Image& orig)
{
	Size sz = orig.GetSize();
	ImageBuffer dest(sz);
	for(i32 rw = 0; rw < sz.cy; rw++)
		for(i32 cl = 0; cl < sz.cx; cl++)
			dest[rw][cl] = orig[sz.cy - rw - 1][sz.cx - cl - 1];
	Point p1 = orig.GetHotSpot();
	Point p2 = orig.Get2ndSpot();
	SetNormalizedHotSpots(dest, sz.cy - p1.y - 1, sz.cx - p1.x - 1, sz.cy - p2.y - 1, sz.cx - p2.x - 1);
	dest.SetResolution(dest.GetResolution());
	return static_cast<Image>(dest);
}

Image Transpose(const Image& img)
{
	Size sz = img.GetSize();
	ImageBuffer ib(sz.cy, sz.cx);
	for(i32 x = 0; x < sz.cx; x++)
		for(i32 y = 0; y < sz.cy; y++)
			ib[x][y] = img[y][x];
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, p1.y, p1.x, p2.y, p2.x);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image Transverse(const Image& img)
{
	Size sz = img.GetSize();
	ImageBuffer ib(sz.cy, sz.cx);
	for(i32 x = 0; x < sz.cx; x++)
		for(i32 y = 0; y < sz.cy; y++)
			ib[x][y] = img[sz.cy - y - 1][sz.cx - x - 1];
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, sz.cy - p1.y - 1, sz.cx - p1.x - 1, sz.cy - p2.y - 1, sz.cx - p2.x - 1);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image MirrorHorz(const Image& img)
{
	Size sz = img.GetSize();
	Image h = img;
	ImageBuffer ib(h);
	for(i32 y = 0; y < sz.cy; y++) {
		RGBA *b = ib[y] + 0;
		RGBA *e = b + sz.cx - 1;
		while(b < e) {
			Swap(*b, *e);
			b++;
			e--;
		}
	}
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, sz.cx - p1.x - 1, p1.y, sz.cx - p2.x - 1, p2.y);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image MirrorVert(const Image& img)
{
	Size sz = img.GetSize();
	Image h = img;
	ImageBuffer ib(h);

	for(i32 y = 0; y < sz.cy / 2; y++) {
		RGBA *b = ib[y];
		RGBA *e = ib[sz.cy - y - 1];
		for(i32 x = 0; x < sz.cx; x++) {
			Swap(*b, *e);
			b++;
			e++;
		}
	}
	Point p1 = img.GetHotSpot();
	Point p2 = img.Get2ndSpot();
	SetNormalizedHotSpots(ib, p1.x, sz.cy - p1.y - 1, p2.x, sz.cy - p2.y - 1);
	ib.SetResolution(img.GetResolution());
	return static_cast<Image>(ib);
}

Image FlipImage(const Image& m, i32 mode)
{
	switch(mode) {
	case FLIP_MIRROR_HORZ:
		return MirrorHorz(m);
	case FLIP_ROTATE_180:
		return Rotate180(m);
	case FLIP_MIRROR_VERT:
		return MirrorVert(m);
	case FLIP_TRANSPOSE:
		return Transpose(m);
	case FLIP_ROTATE_CLOCKWISE:
		return RotateClockwise(m);
	case FLIP_TRANSVERSE:
		return Transverse(m);
	case FLIP_ROTATE_ANTICLOCKWISE:
		return RotateAntiClockwise(m);
	}
	return m;
}

static Pointf Cvp(double x, double y, double sina, double cosa)
{
	return Pointf(x * cosa + y * sina, -x * sina + y * cosa);
}

Image Rotate(const Image& m, i32 angle)
{
	Size isz = m.GetSize();
	Pointf centerf = Pointf(Point(isz)) / 2.0;
	double sina, cosa;
	Draw::SinCos(-angle, sina, cosa);
	Pointf p1 = Cvp(-centerf.x, -centerf.y, sina, cosa);
	Pointf p2 = Cvp(centerf.x, -centerf.y, sina, cosa);
	Size sz2 = Size(2 * (i32)max(tabs(p1.x), tabs(p2.x)),
	                2 * (i32)max(tabs(p1.y), tabs(p2.y)));
	ImageBuffer ib(sz2);
	Fill(~ib, RGBAZero(), ib.GetLength());
	RGBA *t = ~ib;
	Draw::SinCos(angle, sina, cosa);
	i32 sini = i32(sina * 128);
	i32 cosi = i32(cosa * 128);
	Buffer<i32> xmx(sz2.cx);
	Buffer<i32> xmy(sz2.cx);
	for(i32 x = 0; x < sz2.cx; x++) {
		i32 xx = x + x - sz2.cx;
		xmx[x] = i32(xx * cosi);
		xmy[x] = -i32(xx * sini);
	}
	for(i32 y = 0; y < sz2.cy; y++) {
		i32 yy = y + y - sz2.cy;
		i32 ymx = i32(yy * sini) + (isz.cx << 7);
		i32 ymy = i32(yy * cosi) + (isz.cy << 7);
		for(i32 x = 0; x < sz2.cx; x++) {
			i32 xs = (xmx[x] + ymx) >> 8;
			i32 ys = (xmy[x] + ymy) >> 8;
			*t++ = xs >= 0 && xs < isz.cx && ys >= 0 && ys < isz.cy ? m[ys][xs] : RGBAZero();
		}
	}
	ib.SetResolution(m.GetResolution());
	return static_cast<Image>(ib);
}

Image Dither(const Image& m, i32 dival)
{
	static u8 dither[8][8]= {
		{ 1, 49, 13, 61, 4, 52, 16, 64 },
        { 33, 17, 45, 29, 36, 20, 48, 32 },
        { 9, 57, 5, 53, 12, 60, 8, 56 },
        { 41, 25, 37, 21, 44, 28, 40, 24 },
        { 3, 51, 15, 63, 2, 50, 14, 62 },
        { 35, 19, 47, 31, 34, 18, 46, 30 },
        { 11, 59, 7, 55, 10, 58, 6, 54 },
        { 43, 27, 39, 23, 42, 26, 38, 22 },
	};
	
	Size isz = m.GetSize();
	ImageBuffer ib(m.GetSize());
	const RGBA *s = ~m;
	RGBA *t = ~ib;
	for(i32 y = 0; y < isz.cy; y++)
		for(i32 x = 0; x < isz.cx; x++) {
			i32 g = Grayscale(*s++) * 100 / dival;
			*t++ = g > dither[y & 7][x & 7] ? White() : Black();
		}
	ib.SetResolution(m.GetResolution());
	return static_cast<Image>(ib);
}

Image GaussianBlur(const Image& img, i32 radius, bool co)
{
	// This code is adapted from Ivan Kutskir's fast blur implementation, published under MIT license.
	// See: http://blog.ivank.net/fastest-gaussian-blur.html
	
	// An implementation of well known fast box and gaussian blur
	// approximation algorithms by Wojciech Jarosz and Peter Kovesi.
	// See: https://elynxsdk.free.fr/ext-docs/Blur/Fast_box_blur.pdf
	// See: https://www.peterkovesi.com/papers/FastGaussianSmoothing.pdf
	
	auto ApplyBoxBlur = [&](const Image& src, i32 r) -> Image
	{
		double avg = 1.0 / (r + r + 1);
	
		Size sz = src.GetSize();
	
		ImageBuffer tmp(sz);
		ImageBuffer out(sz);

		auto DoLine = [&](i32 i) {
			i32 ti = 0;
			i32 li = ti;
			i32 ri = r;
			const RGBA& fv = src[i][0];
			const RGBA& lv = src[i][sz.cx - 1];
			dword rsum = fv.r * (r + 1);
			dword gsum = fv.g * (r + 1);
			dword bsum = fv.b * (r + 1);
			dword asum = fv.a * (r + 1);
			for(i32 j = 0; j < r; j++) {
				const RGBA& p = src[i][j];
				rsum += p.r;
				gsum += p.g;
				bsum += p.b;
				asum += p.a;
			}
			for(i32 j = 0; j <= r; j++) {
				const RGBA& p = src[i][ri++];
				RGBA& q       = tmp[i][ti++];
				q.r = u8((rsum += p.r - fv.r) * avg);
				q.g = u8((gsum += p.g - fv.g) * avg);
				q.b = u8((bsum += p.b - fv.b) * avg);
				q.a = u8((asum += p.a - fv.a) * avg);
			}
			for(i32 j = r + 1; j < sz.cx - r; j++) {
				const RGBA& p = src[i][ri++];
				const RGBA& q = src[i][li++];
				RGBA& t       = tmp[i][ti++];
				t.r = u8((rsum += p.r - q.r) * avg);
				t.g = u8((gsum += p.g - q.g) * avg);
				t.b = u8((bsum += p.b - q.b) * avg);
				t.a = u8((asum += p.a - q.a) * avg);
			}
			for(i32 j = sz.cx - r; j < sz.cx ; j++) {
				const RGBA& p = src[i][li++];
				RGBA& q       = tmp[i][ti++];
				q.r = u8((rsum += lv.r - p.r) * avg);
				q.g = u8((gsum += lv.g - p.g) * avg);
				q.b = u8((bsum += lv.b - p.b) * avg);
				q.a = u8((asum += lv.a - p.a) * avg);
			}
		};


		if(co)
			CoFor(sz.cy, [&](i32 i) { DoLine(i); });
		else
			for(i32 i = 0; i < sz.cy; i++)
				DoLine(i);

		auto DoColumn = [&](i32 i) {
			i32 ti = 0;
			i32 li = ti;
			i32 ri = r;
			const RGBA& fv = tmp[ti][i];
			const RGBA& lv = tmp[sz.cy - 1][i];
			dword rsum = fv.r * (r + 1);
			dword gsum = fv.g * (r + 1);
			dword bsum = fv.b * (r + 1);
			dword asum = fv.a * (r + 1);
			for(i32 j = 0; j < r; j++) {
				const RGBA& p = tmp[j][i];
				rsum += p.r;
				gsum += p.g;
				bsum += p.b;
				asum += p.a;
			}
			for(i32 j = 0; j <= r; j++) {
				const RGBA& p = tmp[ri++][i];
				RGBA& q       = out[ti++][i];
				q.r = u8((rsum += p.r - fv.r) * avg);
				q.g = u8((gsum += p.g - fv.g) * avg);
				q.b = u8((bsum += p.b - fv.b) * avg);
				q.a = u8((asum += p.a - fv.a) * avg);
			}
			for(i32 j = r + 1; j < sz.cy - r; j++) {
				const RGBA& p = tmp[ri++][i];
				const RGBA& q = tmp[li++][i];
				RGBA&       t = out[ti++][i];
				t.r = u8((rsum += p.r - q.r) * avg);
				t.g = u8((gsum += p.g - q.g) * avg);
				t.b = u8((bsum += p.b - q.b) * avg);
				t.a = u8((asum += p.a - q.a) * avg);
			}
			for(i32 j = sz.cy - r; j < sz.cy; j++) {
				const RGBA& p = tmp[li++][i];
				RGBA& q       = out[ti++][i];
				q.r = u8((rsum += lv.r - p.r) * avg);
				q.g = u8((gsum += lv.g - p.g) * avg);
				q.b = u8((bsum += lv.b - p.b) * avg);
				q.a = u8((asum += lv.a - p.a) * avg);
			}
		};

		if(co) {
			std::atomic<i32> ii(0);
			CoDo([&] {
				for(i32 i = 16 * ii++; i < sz.cx; i = 16 * ii++) { // go by cacheline
					i32 e = min(i + 16, sz.cx);
					for(i32 j = i; j < e; j++)
						DoColumn(j);
				}
			});
		}
		else
			for(i32 i = 0; i < sz.cx; i++)
				DoColumn(i);

		out.SetHotSpots(src);
		out.SetResolution(src.GetResolution());
		return static_cast<Image>(out);
	};

	if(radius < 1 || IsNull(img))
		return img;
	
	double wl = ffloor(sqrt((12 * sqr(radius) / 3) + 1));
	if(fmod(wl, 2) == 0) wl--;
	double wu = wl + 2;
	double m = fround((12 * sqr(radius) - 3 * sqr(wl) - 4 * 3 * wl - 3 * 3) / (-4 * wl - 4));
	Vec<i32> sizes;
	Image t = img;
	for (i32 i = 0; i < 3; i++)
		t = ApplyBoxBlur(t, i32(((i < m ? wl : wu) - 1) / 2));
	return pick(t);
}

}
