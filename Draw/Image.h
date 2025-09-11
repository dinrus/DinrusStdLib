#define NEWIMAGE

enum ImageKind {
	IMAGE_UNKNOWN,
	IMAGE_EMPTY, // deprecated
	IMAGE_ALPHA,
	IMAGE_MASK, // deprecated
	IMAGE_OPAQUE,
};

inline void Fill(RGBA *t, RGBA c, size_t n) { memset32(t, *(dword *)&c, n); }
inline void Copy(RGBA *t, const RGBA *s, i32 n) { memcpy_t(t, s, n); }

i32  Premultiply(RGBA *t, const RGBA *s, size_t len);
i32  Unmultiply(RGBA *t, const RGBA *s, size_t len);

void TransformComponents(RGBA *t, const RGBA *s, i32 len,
	                                         u8k r[], u8k g[], u8k b[], u8k a[]);
void MultiplyComponents(RGBA *t, const RGBA *s, i32 len, i32 num, i32 den = 256);

void AlphaBlend(RGBA *t, const RGBA *s, i32 len);
void AlphaBlend(RGBA *t, const RGBA *s, i32 len, Color color);

void AlphaBlendOpaque(RGBA *t, const RGBA *s, i32 len, Color color);
void AlphaBlendOpaque(RGBA *t, const RGBA *s, i32 len);

void AlphaBlendStraight(RGBA *b, const RGBA *f, i32 len);
void AlphaBlendStraight(RGBA *b, const RGBA *f, i32 len, Color color);
void AlphaBlendStraightOpaque(RGBA *t, const RGBA *s, i32 len);
void AlphaBlendStraightOpaque(RGBA *t, const RGBA *s, i32 len, i32 alpha);

i32  GetChMaskPos32(dword mask);
void AlphaBlendOverBgST(RGBA *b, RGBA bg, i32 len);

u8k *UnpackRLE(RGBA *t, u8k *src, i32 len);
Txt  PackRLE(const RGBA *s, i32 len);

inline i32  Grayscale(i32 r, i32 g, i32 b) { return (77 * r + 151 * g + 28 * b) >> 8; }
inline i32  Grayscale(const RGBA& c)       { return Grayscale(c.r, c.g, c.b); }
inline u8   Saturate255(i32 x)             { return u8(~(x >> 24) & (x | (-(x >> 8) >> 24)) & 0xff); }

class  Image;

enum ImageResolutionIntent {
	IMAGE_RESOLUTION_NONE = -1,
	IMAGE_RESOLUTION_STANDARD = 0,
	IMAGE_RESOLUTION_UHD = 1,
};

class ImageBuffer : NoCopy {
	mutable i32  kind;
	Size         size;
	Buffer<RGBA> pixels;
	Point        hotspot;
	Point        spot2;
	Size         dots;
	i8           resolution;
	bool         paintonce = false;

	void         Set(Image& img);
	void         DeepCopy(const ImageBuffer& img);

	RGBA*        Line(i32 i) const      { ASSERT(i >= 0 && i < size.cy); return (RGBA *)~pixels + i * (size_t)size.cx; }
	friend void  DropPixels___(ImageBuffer& b) { b.pixels.Clear(); }
	void         InitAttrs();

	friend class Image;

public:
	void  SetKind(i32 k)                { kind = k; }
	i32   GetKind() const               { return kind; }
	i32   ScanKind() const;
	i32   GetScanKind() const           { return kind == IMAGE_UNKNOWN ? ScanKind() : kind; }

	void  SetHotSpot(Point p)           { hotspot = p; }
	Point GetHotSpot() const            { return hotspot; }

	void  Set2ndSpot(Point p)           { spot2 = p; }
	Point Get2ndSpot() const            { return spot2; }

	void  SetHotSpots(const Image& src);

	void  SetDots(Size sz)              { dots = sz; }
	Size  GetDots() const               { return dots; }
	void  SetDPI(Size sz);
	Size  GetDPI();

	void  SetResolution(i32 i)          { resolution = i; }
	i32   GetResolution() const         { return resolution; }

	void  CopyAttrs(const ImageBuffer& img);
	void  CopyAttrs(const Image& img);

	Size  GetSize() const               { return size; }
	i32   GetWidth() const              { return size.cx; }
	i32   GetHeight() const             { return size.cy; }
	size_t GetLength() const            { return (size_t)size.cx * size.cy; }

	RGBA *operator[](i32 i)             { return Line(i); }
	const RGBA *operator[](i32 i) const { return Line(i); }
	RGBA *operator~()                   { return pixels; }
	operator RGBA*()                    { return pixels; }
	const RGBA *operator~() const       { return pixels; }
	operator const RGBA*() const        { return pixels; }
	RGBA *Begin()                       { return pixels; }
	const RGBA *Begin() const           { return pixels; }
	RGBA *End()                         { return pixels + GetLength(); }
	const RGBA *End() const             { return pixels + GetLength(); }
	RGBA *begin()                       { return pixels; }
	const RGBA *begin() const           { return pixels; }
	RGBA *end()                         { return pixels + GetLength(); }
	const RGBA *end() const             { return pixels + GetLength(); }

	void  Create(i32 cx, i32 cy);
	void  Create(Size sz)               { Create(sz.cx, sz.cy); }
	bool  IsEmpty() const               { return (size.cx | size.cy) == 0; }
	void  Clear()                       { Create(0, 0); }

	void  PaintOnceHint(bool b = true)  { paintonce = b; }
	bool  IsPaintOnceHint() const       { return paintonce; }

	void  operator=(Image& img);
	void  operator=(ImageBuffer& img);

	ImageBuffer()                       { Create(0, 0); }
	ImageBuffer(i32 cx, i32 cy)         { Create(cx, cy); }
	ImageBuffer(Size sz)                { Create(sz.cx, sz.cy); }
	ImageBuffer(Image& img);
	ImageBuffer(ImageBuffer& b);
// BW, defined in CtrlCore:
	ImageBuffer(ImageDraw& iw);
};

void Premultiply(ImageBuffer& b);
void Unmultiply(ImageBuffer& b);

class Image : public ValType< Image, 150, Moveable_<Image> > {
private:
	struct Data {
		Atomic refcount;
		z64  serial;
		zu64 aux_data;
		i32    paintcount;

		void   Retain()  { AtomicInc(refcount); }
		void   Release() { if(AtomicDec(refcount) == 0) delete this; }

		ImageBuffer buffer;
		bool        paintonly;

		i32         GetKind();

		Data(ImageBuffer& b);
	};

	Data *data;

	void Set(ImageBuffer& b);

	friend class  ImageBuffer;
	friend struct Data;
	friend class  SystemDraw;
	friend void   SetPaintOnly__(Image& img)   { img.data->paintonly = img.data->refcount == 1; }
	friend void   SysImageRealized(const Image& img);
	friend struct scImageMaker;

	void         SetAuxData(zu64 data);

public:
	Size   GetSize() const                     { return data ? data->buffer.GetSize() : Size(0, 0); }
	i32    GetWidth() const                    { return GetSize().cx; }
	i32    GetHeight() const                   { return GetSize().cy; }
	size_t GetLength() const                   { return data ? data->buffer.GetLength() : 0; }
	Point  GetHotSpot() const;
	Point  Get2ndSpot() const;
	Size   GetDots() const;
	Size   GetDPI() const;
	i32    GetKindNoScan() const;
	i32    GetKind() const;
	i32    GetResolution() const;
	bool   IsOpaque() const                    { return GetKind() == IMAGE_OPAQUE; }

	const RGBA *Begin() const                  { return data ? ~data->buffer : NULL; }
	const RGBA *begin() const                  { return Begin(); }
	const RGBA *End() const                    { return Begin() + GetLength(); }
	const RGBA *end() const                    { return End(); }
	const RGBA* operator~() const              { return Begin(); }
	operator const RGBA*() const               { return Begin(); }
	const RGBA* operator[](i32 i) const        { ASSERT(data); return data->buffer[i]; }

	z64 GetSerialId() const                 { return data ? data->serial : 0; }
	bool  IsSame(const Image& img) const      { return GetSerialId() == img.GetSerialId(); }

	bool   operator==(const Image& img) const;
	bool   operator!=(const Image& img) const;
	hash_t GetHashVal() const;
	Txt ToTxt() const;

	void  Serialize(Stream& s);
	void  Xmlize(XmlIO& xio)                  { XmlizeBySerialize(xio, *this); }
	void  Jsonize(JsonIO& jio)                { JsonizeBySerialize(jio, *this); }
	void  Clear();

	Image& operator=(const Image& img);
	Image& operator=(ImageBuffer& img);

	bool IsNullInstance() const         { Size sz = GetSize(); return (sz.cx|sz.cy) == 0; }

	bool IsEmpty() const                { return IsNullInstance(); }
	operator Val() const              { return RichToVal(*this); }

	bool IsPaintOnly() const            { return data && data->paintonly; }
	bool IsPaintOnceHint() const        { return data && data->buffer.IsPaintOnceHint(); }

	Image()                             { data = NULL; }
	Image(const Nuller&)                { data = NULL; }
	Image(const Val& src);
	Image(const Image& img);
	Image(Image (*fn)());
	Image(ImageBuffer& b);
	~Image();

	// Defined in CtrlCore or by Rainbow:
	static Image Arrow();
	static Image Wait();
	static Image IBeam();
	static Image No();
	static Image SizeAll();
	static Image SizeHorz();
	static Image SizeVert();
	static Image SizeTopLeft();
	static Image SizeTop();
	static Image SizeTopRight();
	static Image SizeLeft();
	static Image SizeRight();
	static Image SizeBottomLeft();
	static Image SizeBottom();
	static Image SizeBottomRight();
	static Image Cross();
	static Image Hand();

	// standard mouse cursor support

	zu64       GetAuxData() const;

	// required by system image cache managemenent

	i32 GetRefCount() const         { return data ? (i32)data->refcount : 0; }
};

Image Premultiply(const Image& img);
Image Unmultiply(const Image& img);

struct ImageIml : Moveable<ImageIml> {
	Image  image;
	dword  flags = 0;
};

Vec<ImageIml> UnpackImlData(ukk ptr, i32 len);
Vec<ImageIml> UnpackImlData(const Txt& d);

enum {
	GUI_MODE_NORMAL   = 0,
	GUI_MODE_DARK     = 1,
	GUI_MODE_UHD      = 2,
	GUI_MODE_DARK_UHD = 3,
};

enum {
	IML_IMAGE_FLAG_FIXED        = 0x1,
	IML_IMAGE_FLAG_FIXED_COLORS = 0x2,
	IML_IMAGE_FLAG_FIXED_SIZE   = 0x4,
	IML_IMAGE_FLAG_UHD          = 0x8,
	IML_IMAGE_FLAG_DARK         = 0x10,
};

Image MakeImlImage(const Txt& id, Function<ImageIml (i32, const Txt&)> GetRaw, dword global_flags);

class Iml {
	struct IImage : Moveable<IImage> {
		std::atomic<bool>  loaded;
		Image              image;

		IImage() { loaded = false; }
	};
	struct Data : Moveable<Data> {
		tukk data;
		i32   len, count;
	};
	Vec<Data> data[4]; // 0 normal, 1 HiDPI - HD, 2 DK - Dark, 3 HDK - HiDPI + dark
	VecMap<Txt, IImage> map;
	tukk *name;
	dword global_flags = 0;
	bool  premultiply;

	Index<Txt> ex_name[3]; // 0 HiDPI - HD, 1 DK - Dark, 2 HDK - HiDPI + dark

	void  Init(i32 n);

public:
	void   Reset();
	i32    GetCount() const                  { return map.GetCount(); }
	Txt GetId(i32 i)                      { return map.GetKey(i); }
	Image  Get(i32 i);
	i32    Find(const Txt& id) const      { return map.Find(id); }
	void   Set(i32 i, const Image& img);

	ImageIml GetRaw(i32 mode, i32 i); // tries to get image for mode, can return Null
	ImageIml GetRaw(i32 mode, const Txt& id); // tries to get image for mode by id, can return Null

// these methods serve for .iml import
	Iml(tukk *name, i32 n);//Deprecated - legacy .iml
	void AddData(u8k *data, i32 len, i32 count, i32 mode = 0);
	void AddId(i32 mode1, tukk name);
	void Premultiplied()                   { premultiply = false; }
	void GlobalFlag(dword f)               { global_flags |= f; }

	static void ResetAll(); // clears all .iml caches
};

void   Register(tukk imageclass, Iml& iml);

i32    GetImlCount();
Txt    GetImlName(i32 i);
Iml&   GetIml(i32 i);
i32    FindIml(tukk name);
Image  GetImlImage(tukk name);
void   SetImlImage(tukk name, const Image& m);

Txt    StoreImageAsTxt(const Image& img);
Image  LoadImageFromTxt(const Txt& s);
Size   GetImageTxtSize(const Txt& src);
Size   GetImageTxtDots(const Txt& src);

#include "Raster.h"
#include "ImageOp.h"
#include "SIMD.h"