Image CreateImage(Size sz, const RGBA& rgba);
Image CreateImage(Size sz, Color color);
Image SetColorKeepAlpha(const Image& img, Color c);

void  SetHotSpots(Image& m, Point hotspot, Point hotspot2 = Point(0, 0));
Image WithHotSpots(const Image& m, Point hotspot, Point hotspot2 = Point(0, 0));
Image WithHotSpots(const Image& m, i32 x1, i32 y1, i32 x2, i32 y2);
Image WithHotSpot(const Image& m, i32 x1, i32 y1);

void  SetResolution(Image& m, i32 res);
Image WithResolution(const Image& m, i32 res);
Image WithResolution(const Image& m, const Image& res);

void  ScanOpaque(Image& m);
void  DstSrcOp(ImageBuffer& dest, Point p, const Image& src, const Rect& srect,
                           void (*op)(RGBA *t, const RGBA *s, i32 n));

void Over(ImageBuffer& dest, Point p, const Image& src, const Rect& srect);
void Over(Image& dest, const Image& src);
void Copy(ImageBuffer& dest, Point p, const Image& src, const Rect& srect);
void Fill(ImageBuffer& dest, const Rect& rect, RGBA color);

void  Copy(Image& dest, Point p, const Image& src, const Rect& srect);
void  Over(Image& dest, Point p, const Image& src, const Rect& srect);
Image GetOver(const Image& dest, const Image& src);
void  Fill(Image& dest, const Rect& rect, RGBA color);

Image Copy(const Image& src, const Rect& srect);

void  OverStraightOpaque(ImageBuffer& dest, Point p, const Image& src, const Rect& srect);
void  OverStraightOpaque(Image& dest, Point p, const Image& _src, const Rect& srect);

void  Crop(RasterEncoder& tgt, Raster& img, const Rect& rc);
Image Crop(const Image& img, const Rect& rc);
Image Crop(const Image& img, i32 x, i32 y, i32 cx, i32 cy);

Image AddMargins(const Image& img, i32 left, i32 top, i32 right, i32 bottom, RGBA color);

Rect  FindBounds(const Image& m, RGBA bg = RGBAZero());
Image AutoCrop(const Image& m, RGBA bg = RGBAZero());
void  AutoCrop(Image *m, i32 count, RGBA bg = RGBAZero());

void  ClampHotSpots(Image& m);

Image ColorMask(const Image& src, Color transparent);

void  CanvasSize(RasterEncoder& tgt, Raster& img, i32 cx, i32 cy);
Image CanvasSize(const Image& img, i32 cx, i32 cy);

Image AssignAlpha(const Image& img, const Image& new_alpha);

Image Grayscale(const Image& img);
Image Grayscale(const Image& img, i32 amount);
Image Contrast(const Image& img, i32 amount = 256);

Image HorzFadeOut(i32 cx, i32 cy, Color color);
Image HorzFadeOut(Size sz, Color color);

void DrawRasterData(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& data);

class RescaleImage {
	Raster       *src;
	Size          tsz;
	Vec<dword> horz;
	Vec<dword> vert;
	void        (*row_proc)(dword *dest, const RGBA *src, const dword *map);
	Size          size;
	i32           cx4;
	i32           count;
	i32           segment;
	i32           entry;
	i32           step;
	i32           segspan;
	bool          bigseg;
	Buffer<dword> row_buffers;
	i32           first;
	i32           full;
	const dword  *offsets;
	i32           offset;
	i32           y;

	struct Ln {
		RasterLine line;
		i32        ii;
	};

	Ln            cache[4];
	i32           cii;
	const RGBA   *GetLine(i32 i);

public:
	void Create(Size sz, Raster& src, const Rect& src_rc);
	void Get(RGBA *line);

	Image CoRescale(Size sz, const Image& img, const Rect& src_rc);
};

bool  Rescale(RasterEncoder& tgt, Size sz, Raster& src, const Rect& src_rc,
              Gate<i32, i32> progress = Null);
Image Rescale(const Image& src, Size sz, const Rect& src_rc, Gate<i32, i32> progress = Null);
Image Rescale(const Image& src, Size sz, Gate<i32, i32> progress = Null);
Image Rescale(const Image& src, i32 cx, i32 cy, Gate<i32, i32> progress = Null);

struct ImageFilter9 {
	virtual RGBA operator()(const RGBA **mx) = 0;
	virtual ~ImageFilter9() {}
};

Image Filter(const Image& img, ImageFilter9& filter);
void  Filter(RasterEncoder& target, Raster& src, ImageFilter9& filter);

Image Etched(const Image& img);
Image Sharpen(const Image& img, i32 amount = 100);
Image Dither(const Image& m, i32 dival = 394);
Image GaussianBlur(const Image& img, i32 radius, bool co = false);

Image RotateClockwise(const Image& img);
Image RotateAntiClockwise(const Image& img);
Image Rotate180(const Image& orig);
Image MirrorHorz(const Image& img);
Image MirrorVert(const Image& img);
Image Rotate(const Image& m, i32 angle);
Image Transpose(const Image& img);
Image Transverse(const Image& img);

Image FlipImage(const Image& m, i32 mode);

Image Colorize(const Image& img, Color color, i32 alpha = 100);
Image Equalight(const Image& img, i32 thold = 10);

Image DarkTheme(const Image& img);

//Chameleon support
i32   Diff(RGBA a, RGBA b);
Image Unglyph(const Image& m, Color& c, double& factor);
Image Unglyph(const Image& m, Color& c);
Image Unglyph(const Image& m);
Image VertBlend(Image img1, Image img2, i32 y0, i32 y1);
Image HorzBlend(Image img1, Image img2, i32 x0, i32 x1);
Image HorzSymm(Image src);

bool   IsSingleColor(const Image& m, const Rect& rect);

Image  RecreateAlpha(const Image& overwhite, const Image& overblack);
i32    ImageMargin(const Image& m, i32 p, i32 dist);
i32    ImageMarginV(const Image& _m, i32 p, i32 dist);
Rect   GetImageMargins(const Image& m, RGBA margin_color);

struct ChPartMaker {
	Image image;
	Color border;
	Color bg;

	bool  t, b, l, r;
	u8  tl, tr, bl, br;

	void ResetShape();
	Image Make() const;

	ChPartMaker(const Image& m);
};

// Image cache

struct ImageMaker {
	virtual Txt Key() const = 0;
	virtual Image  Make() const = 0;
	virtual ~ImageMaker() {}
};

void  SysImageRealized(const Image& img); // SystemDraw realized Image handle in GUI
void  SysImageReleased(const Image& img); // SystemDraw dropped Image handle

Image MakeImage(const ImageMaker& m);
Image MakeImage(const Image& image, Image (*make)(const Image& image));

void  SweepMkImageCache();

void  ClearMakeImageCache();
void  SetMakeImageCacheSize(i32 m);
void  SetMakeImageCacheMax(i32 m);

Image MakeImagePaintOnly(const ImageMaker& m);

Image RescaleFilter(const Image& img, Size sz, const Rect& sr,
                    double (*kernel_fn)(double x), i32 kernel_width, Gate<i32, i32> progress,
                    bool co);
Image RescaleFilter(const Image& img, Size sz,
                    double (*kernel_fn)(double x), i32 kernel_width, Gate<i32, i32> progress,
                    bool co);
Image RescaleFilter(const Image& img, i32 cx, i32 cy,
                    double (*kernel_fn)(double x), i32 kernel_width, Gate<i32, i32> progress,
                    bool co);

enum {
	FILTER_NEAREST = 0,
	FILTER_BILINEAR = 1,
	FILTER_BSPLINE = 2,
	FILTER_COSTELLO = 3,
	FILTER_BICUBIC_MITCHELL = 4,
	FILTER_BICUBIC_CATMULLROM = 5,
	FILTER_LANCZOS2 = 6,
	FILTER_LANCZOS3 = 7,
	FILTER_LANCZOS4 = 8,
	FILTER_LANCZOS5 = 9,
};

Image RescaleFilter(const Image& img, Size sz, const Rect& sr, i32 filter, Gate<i32, i32> progress = Null, bool co = false);
Image RescaleFilter(const Image& img, Size sz, i32 filter, Gate<i32, i32> progress = Null);
Image RescaleFilter(const Image& img, i32 cx, i32 cy, i32 filter, Gate<i32, i32> progress = Null);

Image CoRescaleFilter(const Image& img, Size sz, const Rect& sr, i32 filter, Gate<i32, i32> progress = Null);
Image CoRescaleFilter(const Image& img, Size sz, i32 filter, Gate<i32, i32> progress = Null);
Image CoRescaleFilter(const Image& img, i32 cx, i32 cy, i32 filter, Gate<i32, i32> progress = Null);

Image CachedRescale(const Image& m, Size sz, const Rect& src, i32 filter = Null);
Image CachedRescale(const Image& m, Size sz, i32 filter = Null);
Image CachedRescalePaintOnly(const Image& m, Size sz, const Rect& src, i32 filter = Null);
Image CachedRescalePaintOnly(const Image& m, Size sz, i32 filter = Null);

Image CachedSetColorKeepAlpha(const Image& img, Color color);
Image CachedSetColorKeepAlphaPaintOnly(const Image& img, Color color);

Image Magnify(const Image& img, i32 nx, i32 ny);
Image Minify(const Image& img, i32 nx, i32 ny, bool co = false);
Image MinifyCached(const Image& img, i32 nx, i32 ny, bool co);

Image Upscale2x(const Image& src);
Image Downscale2x(const Image& src);

void SetUHDMode(bool b = true);
bool IsUHDMode();
void SyncUHDMode();

Image DPI(const Image& m);
Image DPI(const Image& img, i32 expected);

inline i32    DPI(i32 a)          { return IsUHDMode() ? 2 * a : a; }
inline double DPI(double a)       { return IsUHDMode() ? 2 * a : a; }
inline Size   DPI(Size sz)        { return IsUHDMode() ? 2 * sz : sz; }
inline Size   DPI(i32 cx, i32 cy) { return Size(DPI(cx), DPI(cy)); }

inline Image DPI(const Image& a, const Image& b) { return IsUHDMode() ? b : a; }

struct RGBAV {
	dword r, g, b, a;

	void Set(dword v) { r = g = b = a = v; }
	void Clear()      { Set(0); }
	void Put(dword weight, const RGBA& src) {
		r += weight * src.r;
		g += weight * src.g;
		b += weight * src.b;
		a += weight * src.a;
	}
	void Put(const RGBA& src) {
		r += src.r;
		g += src.g;
		b += src.b;
		a += src.a;
	}
	RGBA Get(i32 div) const {
		RGBA c;
		c.r = u8(r / div);
		c.g = u8(g / div);
		c.b = u8(b / div);
		c.a = u8(a / div);
		return c;
	}
};

// Obsolete, replace with RescaleFilter!
Image RescaleBicubic(const Image& src, Size sz, const Rect& src_rc, Gate<i32, i32> progress = Null);
Image RescaleBicubic(const Image& img, Size sz, Gate<i32, i32> progress = Null);
Image RescaleBicubic(const Image& img, i32 cx, i32 cy, Gate<i32, i32> progress = Null);
