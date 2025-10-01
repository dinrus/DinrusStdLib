enum {
	RASTER_1,
	RASTER_2,
	RASTER_4,
	RASTER_8,
	RASTER_8ALPHA,
	RASTER_16,
	RASTER_24,
	RASTER_32,
	RASTER_32ALPHA,
	RASTER_32PREMULTIPLIED,

	RASTER_MSBFIRST = 0x80,

	RASTER_MAP_R = 32,
	RASTER_SHIFT_R = 3,
	RASTER_MAP_G = 64,
	RASTER_SHIFT_G = 2,
	RASTER_MAP_B = 16,
	RASTER_SHIFT_B = 4,

	RASTER_MAP_MAX = 64
};

struct PaletteCv {
	Buffer<u8> cv;

	u8 *At(i32 r, i32 b)           { return cv + (r << 10) + (b << 6); }
	u8  Get(const RGBA& b) const   { return cv[(i32(b.r >> RASTER_SHIFT_R) << 10) +
	                                             (i32(b.g >> RASTER_SHIFT_G)) +
	                                             (i32(b.b >> RASTER_SHIFT_B) << 6)]; }
	PaletteCv()                      { cv.Alloc(RASTER_MAP_R * RASTER_MAP_G * RASTER_MAP_B); }
};

class RasterFormat {
	u8    type;
	dword rmask, gmask, bmask;
	u8    rpos, gpos, bpos, apos;

	static void TailBits(RGBA *t, u8k *src, i32 cx, u8 andm, u8 shift, const RGBA *palette);
	static void TailBitsMSB1st(RGBA *t, u8k *src, i32 cx, u8 shift1, u8 andm, u8 shift, const RGBA *palette);

public:
	void Set1lf()                    { type = RASTER_1; }
	void Set1mf()                    { type = RASTER_1|RASTER_MSBFIRST; }
	void Set2lf()                    { type = RASTER_2; }
	void Set2mf()                    { type = RASTER_2|RASTER_MSBFIRST; }
	void Set4lf()                    { type = RASTER_4; }
	void Set4mf()                    { type = RASTER_4|RASTER_MSBFIRST; }
	void Set8()                      { type = RASTER_8; }
	void Set8A()                     { type = RASTER_8ALPHA; }
	void Set16le(dword rmask, dword gmask, dword bmask);
	void Set16be(dword rmask, dword gmask, dword bmask);
	void Set24le(dword rmask, dword gmask, dword bmask);
	void Set24be(dword rmask, dword gmask, dword bmask);
	void Set32le(dword rmask, dword gmask, dword bmask, dword amask = 0);
	void Set32be(dword rmask, dword gmask, dword bmask, dword amask = 0);
	void Set32leStraight(dword rmask, dword gmask, dword bmask, dword amask);
	void Set32beStraight(dword rmask, dword gmask, dword bmask, dword amask);
	void SetRGBA();
	void SetRGBAStraight();

	i32  GetType() const             { return type; }
	i32  IsRGBA() const;
	i32  GetByteCount(i32 cx) const;
	i32  GetBpp() const;
	bool HasAlpha() const;
	i32  GetColorCount() const;
	i32  GetPaletteCount() const;

	void Read(RGBA *t, u8k *s, i32 cx, const RGBA *palette) const;
	void Write(u8 *t, const RGBA *s, i32 cx, const PaletteCv *palcv) const;
};

enum FlipMode {
	FLIP_NONE                 = 0,
	FLIP_MIRROR_HORZ          = 1,
	FLIP_ROTATE_180           = 2,
	FLIP_MIRROR_VERT          = 3,
	FLIP_TRANSPOSE            = 4,
	FLIP_ROTATE_CLOCKWISE     = 5,
	FLIP_TRANSVERSE           = 6,
	FLIP_ROTATE_ANTICLOCKWISE = 7,
};

class Raster {
public:
	class Line {
		mutable const RGBA *data;
		u8k                *fmtdata;
		Raster             *raster;
		mutable bool        free;
		bool                fmtfree;

		friend class       Raster;

		void               MakeRGBA() const;
		void Free()                             { if(free) delete[] data; if(fmtfree) delete[] fmtdata; }
		void Pick(Line&& b);

	public:
		const RGBA         *GetRGBA() const     { if(!data) MakeRGBA(); return data; }
		u8k                *GetRawData() const  { return fmtdata; }
		operator const RGBA *() const           { return GetRGBA(); }
		const RGBA *operator~() const           { return GetRGBA(); }

		Line(const RGBA *data, bool free)
			: data(data), fmtdata((u8 *)data), raster(NULL), free(free), fmtfree(false) {}
		Line(u8k *fmtdata, Raster *raster, bool fmtfree)
			: data(NULL), fmtdata(fmtdata), raster(raster), free(false), fmtfree(fmtfree) {}
		Line(Line&& b)                          { Pick(pick(b)); }
		Line()                                  { data = NULL; fmtdata = NULL; raster = NULL; free = fmtfree = false; }
		~Line()                                 { Free(); }

		void operator=(Line&& b)                { Free(); Pick(pick(b)); }
	};

	struct Info {
		i32   bpp;
		i32   colors;
		Size  dots;
		Point hotspot;
		i32   kind;
		i32   orientation;
		
		Info();
	};

public:
	virtual void    SeekPage(i32 page);
	virtual i32     GetActivePage() const;
	virtual i32     GetPageCount();
	virtual i32     GetPageAspect(i32 n);
	virtual i32     GetPageDelay(i32 n);
	virtual Rect    GetPageRect(i32 n);
	virtual i32		GetPageDisposal(i32 n);

	virtual bool    Create();
	virtual Size    GetSize() = 0;
	virtual Info    GetInfo();
	virtual Line    GetLine(i32 line) = 0;
	virtual bool    IsError();
	virtual i32     GetPaletteCount();
	virtual const RGBA *GetPalette();
	virtual const RasterFormat *GetFormat();

	virtual Val     GetMetaData(Txt id);
	virtual void    EnumMetaData(Vec<Txt>& id_list);

	i32    GetWidth()                              { return GetSize().cx; }
	i32    GetHeight()                             { return GetSize().cy; }
	Line   operator[](i32 i)                       { return GetLine(i); }

	Image  GetImage(i32 x, i32 y, i32 cx, i32 cy, const Gate<i32, i32> progress = Null);
	Image  GetImage(const Gate<i32, i32> progress = Null);

	virtual ~Raster();
};

typedef Raster::Line RasterLine;
typedef Raster::Info RasterInfo;

void CreatePaletteCv(const RGBA *palette, i32 ncolors, PaletteCv& cv_pal);
void CreatePalette(Raster& raster, RGBA *palette, i32 ncolors);
void CreatePalette(Raster& raster, RGBA *palette, i32 ncolors, PaletteCv& cv);

class ImageRaster : public Raster {
	Image img;

public:
	virtual Size    GetSize();
	virtual Line    GetLine(i32 line);
	virtual Info    GetInfo();

	ImageRaster(const Image& img) : img(img) {}
};

class MemoryRaster : public Raster {
	RasterFormat    format;
	Info            info;
	Size            size;
	Vec< Buffer<u8> > lines;
	Vec<RGBA>    palette;

public:
	virtual Size               GetSize()         { return size; }
	virtual Info               GetInfo()         { return info; }
	virtual Line               GetLine(i32 line);
	virtual i32                GetPaletteCount() { return palette.GetCount(); }
	virtual const RGBA         *GetPalette()     { return palette.Begin(); }
	virtual const RasterFormat *GetFormat()      { return &format; }

	void                       Load(Raster& raster);
	i32                        GetLength() const;

	MemoryRaster();
	MemoryRaster(Raster& raster)                 { Load(raster); }
};

class StreamRaster : public Raster {
	Stream *s;
	bool    error;

	typedef StreamRaster *(*RasterFactory)();
	template <class T> static StreamRaster *FactoryFn() { return new T; }
	static void AddFormat(RasterFactory f);
	static Vec<uk >& Map();

public:
	Stream& GetStream()                 { return *s; }
	bool    Open(Stream& _s);
	bool    IsError();

	void    SetError()                  { error = true; }

	Image Load(Stream& s, const Gate<i32, i32> progress = Null);
	Image LoadFile(tukk fn, const Gate<i32, i32> progress = Null);
	Image LoadTxt(const Txt& s, const Gate<i32, i32> progress = Null);

	template <class T>
	static void Register()              { AddFormat(&StreamRaster::FactoryFn<T>); }

	static One<StreamRaster> OpenAny(Stream& s);
	static Image LoadAny(Stream& s, Gate<i32, i32> progress = Null);
	static Image LoadFileAny(tukk fn, Gate<i32, i32> progress = Null);
	static Image LoadTxtAny(const Txt& s, Gate<i32, i32> progress = Null);

	StreamRaster()                      { error = true; }
};

class RasterEncoder {
	Size           size, dots;
	Point          hotspot;
	RGBA          *line;
	Buffer<u8>   scanline;
	i32            line_bytes;
	Buffer<RGBA>   h;
	Buffer<RGBA>   palette;
	One<PaletteCv> palette_cv;

	RGBA          *Pal();

protected:
	RasterFormat   format;
	void           SetLine(RGBA *_line);

public:
	virtual i32  GetPaletteCount();
	virtual void Start(Size sz) = 0;
	virtual void WriteLineRaw(u8k *data) = 0;

	void WriteLine();
	void WriteLine(const RGBA *s);

	const RasterFormat& GetFormat() const        { return format; }
	const RGBA         *GetPalette();
	const PaletteCv    *GetPaletteCv()           { return ~palette_cv; }

	operator RGBA *()                            { return line; }
	RGBA *operator~()                            { return line; }
	Size GetSize() const                         { return size; }
	i32  GetWidth() const                        { return GetSize().cx; }
	i32  GetHeight() const                       { return GetSize().cy; }

	void  SetHotSpot(Point p)                    { hotspot = p; }
	Point GetHotSpot() const                     { return hotspot; }

	void SetDots(Size _dots)                     { dots = _dots; }
	Size GetDots() const                         { return dots; }

	void SetPalette(const RGBA *palette);
	void SetPalette(Raster& pal_raster);

	void Create(Size sz);
	void Create(i32 cx, i32 cy)                  { Create(Size(cx, cy)); }

	void Create(Size sz, const RGBA *palette);
	void Create(i32 cx, i32 cy, const RGBA *palette);
	void Create(Size sz, Raster& pal_raster);
	void Create(i32 cx, i32 cy, Raster& pal_raster);

	RasterEncoder();
	virtual ~RasterEncoder();
};

class ImageEncoder : public RasterEncoder {
	i32         ii;
	ImageBuffer ib;

public:
	virtual void Start(Size sz);
	virtual void WriteLineRaw(u8k *data);

	operator const RGBA *()             { return Image(ib); }
	operator Image()                    { return Image(ib); }
};

class StreamRasterEncoder : public RasterEncoder {
	Stream *s;

public:
	Stream& GetStream()                 { return *s; }
	void    SetStream(Stream& _s)       { s = &_s; }

	void    Save(Stream& s, Raster& raster);
	bool    SaveFile(tukk fn, Raster& raster);
	Txt     SaveTxt(Raster& raster);

	void    Save(Stream& s, const Image& img);
	bool    SaveFile(tukk fn, const Image& img);
	Txt     SaveTxt(const Image& img);
};
