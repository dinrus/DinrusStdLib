#ifndef __Geom_Draw_hrr__
#define __Geom_Draw_hrr__

namespace drx {

//#include "Stream.h"

class ImageWriter : public RasterEncoder {
public:
	ImageWriter() : output(NULL) {}
	ImageWriter(ImageBuffer& output, bool merge = true)                       { Open(output, merge); }
	ImageWriter(ImageBuffer& output, Point pos, bool merge = true)            { Open(output, pos, merge); }
	ImageWriter(ImageBuffer& output, Point pos, Rect clip, bool merge = true) { Open(output, pos, clip, merge); }

	void         Open(ImageBuffer& output, bool merge = true)                 { Open(output, Point(0, 0), merge); }
	void         Open(ImageBuffer& output, Point pos, bool merge = true)      { Open(output, pos, Rect(output.GetSize()), merge); }
	void         Open(ImageBuffer& output, Point pos, Rect clip, bool merge = true);

	virtual void Start(Size sz);
	virtual void WriteLineRaw(u8k *data);

private:
	ImageBuffer  *output;
	Point        pos;
	i32          left, width, offset;
	Rect         clip;
	i32          line;
	Size         src_size;
	bool         merge;
};

class ImageBufferRaster : public Raster {
public:
	ImageBufferRaster(const ImageBuffer& buffer);
	ImageBufferRaster(const ImageBuffer& buffer, const Rect& crop);

	virtual Size       GetSize();
	virtual Info       GetInfo();
	virtual Line       GetLine(i32 line);

private:
	const ImageBuffer& buffer;
	Rect               crop;
};

class HRRInfo
{
	friend class HRR;

public:
	HRRInfo();
	HRRInfo(const Rectf& log_rect, const Rectf& map_rect = Null,
		i32 levels = 5, Color background = White,
		i32 method = METHOD_JPG, i32 quality = DFLT_JPG_QUALITY,
		bool mono = false, Color mono_black = Black, Color mono_white = White);

	void               Serialize(Stream& stream);

	bool               IsEmpty() const       { return levels == 0; }

	i32                GetLevels() const     { return levels; }
	i32                GetMaxSize() const    { return levels ? UNIT << (levels - 1) : 0; }
	Rectf              GetLogRect() const    { return log_rect; }
	Rectf              GetMapRect() const    { return map_rect; }
	Color              GetBackground() const { return background; }
	bool               IsMono() const        { return mono; }
	Color              GetMonoBlack() const  { return mono_black; }
	Color              GetMonoWhite() const  { return mono_white; }
	i32                GetMethod() const     { return method; }
	i32                GetQuality() const    { return quality; }

	One<StreamRaster>  GetDecoder() const;
	One<StreamRasterEncoder> GetEncoder() const;

	static double      GetEstimatedFileSize(i32 _levels, i32 method, i32 quality);

	static i32         Pack(i32 method, i32 quality) { return (method << 16) | (quality & 0xFFFF); }
	static i32         Method(i32 p)         { return p >> 16; }
	static i32         Quality(i32 p)        { return p & 0xFFFF; }

	static Vec<i32> EnumMethods();
	static Vec<i32> EnumQualities(i32 method);
	static Txt      GetName(i32 method, i32 quality);
	static VecMap<i32, Txt> GetPackMap();

public:
	enum
	{
		HALF_BITS = 8,
		HALF      = 1 << HALF_BITS,
		UNIT      = HALF << 1,
		STDLEVEL  = 6,
		MAXLEVELS = 12,
		LCOUNT    = 2,
		UCOUNT    = 1 << LCOUNT,
	};

	enum
	{
		METHOD_JPG,
		METHOD_GIF,
		METHOD_RLE,
		METHOD_PNG,
		METHOD_ZIM,
		METHOD_BZM,

		DFLT_JPG_QUALITY = 50,
	};

protected:
	Color      background;
	Color      mono_black;
	Color      mono_white;
	Rectf      log_rect;
	Rectf      map_rect; // logical rectangle inflated to tightly bounding square
	i32        levels;
	i32        method;
	i32        quality;
	bool       mono;
};

class HRR
{
public:
	struct Block
	{
		Block(const HRR& hrr) : hrr(hrr) {}

		void        Init(Size size, RGBA color);

		ImageBuffer block;
		Size        size;
		Rect        area;
		Rectf       log_area;
		i32         level;
		const HRR&  hrr;
	};

	struct Cursor {
		Cursor(HRR& owner, const Rectf& extent, double measure, i32 alpha = 100,
			Color mono_black = Null, Color mono_white = Null, Color blend_bgnd = Null);

		bool Fetch(Rectf& part_rc);
		Image Get();

		HRR& owner;
		Rectf extent;
		double measure;
		Color mono_black;
		Color mono_white;
		i32 alpha;
		Color blend_bgnd;

		One<StreamRaster> raster;
		i32 level;
		i32 total;
		Rect rc;
		Point block;
		i32 cimg;
	};

	friend struct Cursor;

	enum
	{
		DEFAULT_CACHE_SIZEOF_LIMIT = 20000000,
	};

	typedef Gate1<Block&>      Writeback;

	HRR();
	HRR(tukk path, bool read_only = true);

	bool                       Open(tukk path, bool read_only = true);
	void                       Close();

	bool                       Create(const HRRInfo& _info, tukk path);
	void                       Write(Writeback drawback, bool downscale = true);
	Rectf                      GetLogBlockRect(i32 level, const Rect& rc) const;
	Matrixf                    GetPixMapMatrix(i32 level, i32 x, i32 y) const;
	z64                      GetFileWriteSize() const;

	void                       SetCacheSizeLimit(i32 cl)       { FlushCache(cache_sizeof_limit = cl); }
	i32                        GetCacheSizeLimit() const       { return cache_sizeof_limit; }
	i32                        GetCacheSize() const            { return cache_sizeof + directory_sizeof; }
	void                       ClearCache();

	Txt                     GetMap(Txt key) const        { return map.Get(key, Null); }
	void                       SetMap(Txt key, Txt value);
	void                       ClearMap(Txt key)            { SetMap(key, Null); }
	void                       ClearMap()                      { map.Clear(); }
	const VecMap<Txt, Txt>& GetMap() const            { return map; }
	void                       FlushMap();

	bool                       IsOpen() const                  { return stream.IsOpen(); }
	bool                       IsError() const                 { return stream.IsError(); }
	i32                        SizeOfInstance() const;

	void                       Paint(Draw& draw, Rect dest, Rectf src,
		i32 wash = 100, i32 max_pixel = 1, Color mono_black = Null, Color mono_white = Null, Color blend_bgnd = Null);

	void                       Paint(Draw& draw, const Matrixf& trg_pix, GisTransform transform,
		i32 wash = 100, i32 max_pixel = 1, Color mono_black = Null, Color mono_white = Null, Color blend_bgnd = Null);

	static i32                 GetProgressCount(i32 levels, bool downscale = true);

	const HRRInfo&             GetInfo() const                 { return info; }

	static One<StreamRasterEncoder> (*CreateEncoder)(const HRRInfo& info);
	static One<StreamRaster>        (*CreateDecoder)(const HRRInfo& info);
	static One<StreamRasterEncoder> StdCreateEncoder(const HRRInfo& info);
	static One<StreamRaster>        StdCreateDecoder(const HRRInfo& info);

private:
	bool                       Write(Writeback drawback, bool downscale, i32 level, i32 px, i32 py,
		StreamRasterEncoder& format, Block *put);
	void                       Serialize();
	void                       FlushCache(i32 limit);

private:
	HRRInfo                    info;
	FileStream                 stream;
	i32                        version;
	Vec<z64>              pixel_directory_offset;
	Vec<z64>              mask_directory_offset;
//	Vec< Vec<i32> >      pixel_directory;
//	Vec< Vec<i32> >      mask_directory;
	i32                        directory_sizeof;
	ArrayMap<Point, Image>     image_cache;
	VecMap<i32, Size>       size_cache;
	i32                        cache_sizeof;
	i32                        cache_sizeof_limit;
	VecMap<Txt, Txt>  map;
	i32                        map_offset;
};

}

#endif//__Geom_Draw_hrr__
