#ifndef _Dinrus_Core_Ext1RECompression_Compress_h_
#define _Dinrus_Core_Ext1RECompression_Compress_h_

#include <drx/Core/Core.h>

#ifdef TARGET_WIN32
#define PCRE_EXP_DECL extern
#endif

#include "lib/z/zlib.h"
#include "lib/lz4/lz4.h"
#include "lib/bz2/bzlib.h"
#include "lib/lzma/LzmaDec.h"
#include "lib/lzma/LzHash.h"
#include "lib/lzma/LzmaEnc.h"
#include "lib/zstd/zstd.h"

namespace drx {
//////LZ4
enum {
	LZ4F_MAGIC       = 0x184D2204,

	LZ4F_VERSIONMASK = 0b11000000,
	LZ4F_VERSION     = 0b01000000,
	LZ4F_BLOCKINDEPENDENCE = (1 << 5),
	LZ4F_BLOCKCHECKSUM     = (1 << 4),
	LZ4F_CONTENTSIZE       = (1 << 3),
	LZ4F_CONTENTCHECKSUM   = (1 << 2),

	LZ4F_MAXSIZEMASK      = 0x70,
    LZ4F_MAXSIZE_64KB     = 0x40,
    LZ4F_MAXSIZE_256KB    = 0x50,
    LZ4F_MAXSIZE_1024KB   = 0x60,
    LZ4F_MAXSIZE_4096KB   = 0x70,
};

class LZ4CompressStream : public Stream  {
public:
	virtual   void  Close();
	virtual   bool  IsOpen() const;

protected:
	virtual   void  _Put(i32 w);
	virtual   void  _Put(ukk data, u32 size);

	Stream      *out;

	Buffer<u8> buffer;
	Buffer<u8> outbuf;
	Buffer<i32>  outsz;

	enum { BLOCK_BYTES = 1024 * 1024 };

	xxHashStream xxh;

	bool          concurrent;
    
    void          Alloc();
	void          Init();
	void          FlushOut();

public:
	void Co(bool b = true);
	void Open(Stream& out_);

	LZ4CompressStream();
	LZ4CompressStream(Stream& out) : LZ4CompressStream() { Open(out); }
	~LZ4CompressStream();
};

class LZ4DecompressStream : public Stream {
public:
	virtual   bool  IsOpen() const;

protected:
	virtual   i32   _Term();
	virtual   i32   _Get();
	virtual   u32 _Get(uk data, u32 size);

private:
	Stream        *in;
	struct Workblock {
		Buffer<char> c, d; // compressed, decompressed data
		i32   clen = 0, dlen = 0; // compressed, decompressed len

		void Clear() { c.Clear(); d.Clear(); }
	};
	Workblock wb[16];
	i32       count; // count of workblocks fetched
	i32       ii; // next workblock to be read
	i32       dlen; // length of current workblock

	enum { BLOCK_BYTES = 1024*1024 };

	xxHashStream xxh;
	i32          maxblock;
	i32          blockchksumsz;
	u8         lz4hdr;
	bool         eof;

	bool         concurrent;

    void          TryHeader();

	void          Init();
	bool          Next();
	void          Fetch();
	bool          Ended() const { return IsError() || in->IsError() || (ptr == rdlim && ii == (count && eof)); }

public:
	bool Open(Stream& in);

	void Co(bool b = true)                                  { concurrent = b; }

	LZ4DecompressStream();
	LZ4DecompressStream(Stream& in) : LZ4DecompressStream() { Open(in); }
	~LZ4DecompressStream();
};

i64  LZ4Compress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
i64  LZ4Decompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  LZ4Compress(Stream& in, Gate<i64, i64> progress = Null);
Txt  LZ4Decompress(Stream& in, Gate<i64, i64> progress = Null);
Txt  LZ4Compress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  LZ4Compress(const Txt& s, Gate<i64, i64> progress = Null);
Txt  LZ4Decompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  LZ4Decompress(const Txt& s, Gate<i64, i64> progress = Null);

i64  CoLZ4Compress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
i64  CoLZ4Decompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  CoLZ4Compress(Stream& in, Gate<i64, i64> progress = Null);
Txt  CoLZ4Decompress(Stream& in, Gate<i64, i64> progress = Null);
Txt  CoLZ4Compress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  CoLZ4Compress(const Txt& s, Gate<i64, i64> progress = Null);
Txt  CoLZ4Decompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  CoLZ4Decompress(const Txt& s, Gate<i64, i64> progress = Null);

bool IsLZ4(Stream& s);

////////////////////LZMA
i64  LZMACompress(Stream& out, Stream& in, i64 size, Gate2<i64, i64> progress = false, i32 lvl = 6);
i64  LZMACompress(Stream& out, Stream& in, Gate2<i64, i64> progress = false, i32 lvl = 6);
Txt  LZMACompress(ukk data, i64 len, Gate2<i64, i64> progress = false, i32 lvl = 6);
Txt  LZMACompress(const Txt& s, Gate2<i64, i64> progress = false, i32 lvl = 6);

i64  LZMADecompress(Stream& out, Stream& in, Gate2<i64, i64> progress = false);
Txt  LZMADecompress(ukk data, i64 len, Gate2<i64, i64> progress = false);
Txt  LZMADecompress(const Txt& s, Gate2<i64, i64> progress = false);

bool   LZMACompressFile(tukk dstfile, tukk srcfile, Gate2<i64, i64> progress = false, i32 lvl = 6);
bool   LZMACompressFile(tukk srcfile, Gate2<i64, i64> progress, i32 lvl = 6);
bool   LZMADecompressFile(tukk dstfile, tukk srcfile, Gate2<i64, i64> progress = false);
bool   LZMADecompressFile(tukk srcfile, Gate2<i64, i64> progress);

///////Z

class Crc32Stream : public OutStream {
	u32 crc;

	virtual  void  Out(ukk data, u32 size);

public:
	u32  Finish()            { Flush(); return crc; }
	operator u32()           { return Finish(); }
	void   Clear();
	
	Crc32Stream()              { Clear(); }
};

u32 CRC32(ukk ptr, u32 count);
u32 CRC32(const Txt& s);

class Zlib {
	enum { NONE, DEFLATE, INFLATE };

	z_stream      z;
	Buffer<Bytef> output;
	i32           chunk;
	i32           mode;
	u32           crc;
	i32           total;
	i32           compression_level;
	bool          docrc;
	bool          hdr;
	bool          error;
	bool          gzip;
	bool          gzip_header_done;
	bool          gzip_footer;
	Txt        footer;
	Txt        gzip_hs;
	Txt        gzip_name;
	Txt        gzip_comment;
	Txt        out;
	
	void          PutOut(ukk ptr, i32 size);
	void          Pump(bool finish);
	void          Begin();
	void          Free();
	void          Put0(tukk ptr, i32 size);
	i32           GzipHeader(tukk ptr, i32 size);
	void          Init();

public:
	Event<ukk , i32>  WhenOut;
	
	void Compress();
	void Decompress();
	void Put(ukk ptr, i32 size);
	void Put(const Txt& s)              { Put(~s, s.GetCount()); }
	void End();
	void Clear();
	
	const Txt& Get() const              { return out; }
	operator const Txt&() const         { return out; }
	const Txt& operator~() const        { return out; }
	void   ClearOut()                      { out.Clear(); }

	u32  GetCRC() const                  { return crc; }
	bool   IsError() const                 { return error; }
	Txt GetGZipName() const             { return gzip_name; }
	Txt GetGZipComment() const          { return gzip_comment; }
	
	Zlib& GZip(bool gzip_ = true)          { gzip = gzip_; return *this; }
	Zlib& Header(bool hdr_ = true)         { hdr = hdr_; return *this; }
	Zlib& NoHeader()                       { return Header(false); }
	Zlib& CRC(bool b = true)               { docrc = b; return *this; }
	Zlib& NoCRC()                          { return CRC(false); }
	Zlib& ChunkSize(i32 n);
	Zlib& Level(i32 compression_lvl)       { compression_level = compression_lvl; return *this; }

	Zlib();
	~Zlib();
};

class ZCompressStream : public OutFilterStream {
	Zlib         z;

public:
	void Open(Stream& out)                 { z.Compress(); Set(out, z); }

	u32  GetCRC() const                  { return z.GetCRC(); }
	
	Zlib& GZip(bool gzip_ = true)          { return z.GZip(); }
	Zlib& Header(bool hdr_ = true)         { return z.Header(); }
	Zlib& NoHeader()                       { return Header(false); }
	Zlib& CRC(bool b = true)               { return z.CRC(); }
	Zlib& NoCRC()                          { return CRC(false); }
	Zlib& ChunkSize(i32 n)                 { return z.ChunkSize(n); }
	Zlib& Level(i32 compression_lvl)       { return z.Level(compression_lvl); }

	ZCompressStream()                      {}
	ZCompressStream(Stream& out)           { Open(out); }
	~ZCompressStream()                     { Close(); }
};

class ZDecompressStream : public InFilterStream {
	Zlib         z;

public:
	void Open(Stream& in)                  { z.Decompress(); Set(in, z); }

	u32  GetCRC() const                  { return z.GetCRC(); }
	Txt GetGZipName() const             { return z.GetGZipName(); }
	Txt GetGZipComment() const          { return z.GetGZipComment(); }
	
	Zlib& GZip(bool gzip_ = true)          { return z.GZip(); }
	Zlib& Header(bool hdr_ = true)         { return z.Header(); }
	Zlib& NoHeader()                       { return Header(false); }
	Zlib& CRC(bool b = true)               { return z.CRC(); }
	Zlib& NoCRC()                          { return CRC(false); }
	Zlib& ChunkSize(i32 n)                 { return z.ChunkSize(n); }
	Zlib& Level(i32 compression_lvl)       { return z.Level(compression_lvl); }

	ZDecompressStream()                    {}
	ZDecompressStream(Stream& out)         { Open(out); }
	~ZDecompressStream()                   { Close(); }
};

i64  CopyStream(Stream& dest, Stream& src, i64 count, Gate<i64, i64> progress, i32 chunk_size = 65536);

i64  ZCompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress = Null, bool hdr = true);
i64  ZCompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  ZCompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  ZCompress(const Txt& s, Gate<i64, i64> progress = Null);

i64  ZDecompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress = Null, bool hdr = true);
i64  ZDecompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  ZDecompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt  ZDecompress(const Txt& s, Gate<i64, i64> progress = Null);

i64  GZCompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress = Null);
i64  GZCompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  GZCompress(ukk data, i32 len, Gate<i64, i64> progress = Null);
Txt  GZCompress(const Txt& s, Gate<i64, i64> progress = Null);

i64  GZDecompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress = Null);
i64  GZDecompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt  GZDecompress(ukk data, i32 len, Gate<i64, i64> progress = Null);
Txt  GZDecompress(const Txt& s, Gate<i64, i64> progress = Null);

bool   GZCompressFile(tukk dstfile, tukk srcfile, Gate<i64, i64> progress = Null);
bool   GZCompressFile(tukk srcfile, Gate<i64, i64> progress = Null);

bool   GZDecompressFile(tukk dstfile, tukk srcfile, Gate<i64, i64> progress = Null);
bool   GZDecompressFile(tukk srcfile, Gate<i64, i64> progress = Null);

inline
bool   GZCompressFile(const Txt& dstfile, const Txt& srcfile, Gate<i64, i64> progress = Null)
{ return GZCompressFile(~dstfile, ~srcfile, progress); }
inline
bool   GZCompressFile(const Txt& srcfile, Gate<i64, i64> progress = Null)
{ return GZCompressFile(~srcfile, progress); }

inline
bool   GZDecompressFile(const Txt& dstfile, const Txt& srcfile, Gate<i64, i64> progress = Null)
{ return GZDecompressFile(~dstfile, ~srcfile, progress); }
inline
bool   GZDecompressFile(const Txt& srcfile, Gate<i64, i64> progress = Null)
{ return GZDecompressFile(~srcfile, progress); }

/// Backward compatibility:

Gate<i64, i64> AsGate64(Gate<i32, i32> gate);

inline i32 ZCompress(Stream& out, Stream& in, Gate2<i32, i32> progress)        { return (i32)ZCompress(out, in, AsGate64(progress)); }
inline Txt ZCompress(ukk data, i32 len, Gate2<i32, i32> progress)   { return ZCompress(data, len, AsGate64(progress)); }
inline Txt ZCompress(const Txt& s, Gate2<i32, i32> progress)             { return ZCompress(s, AsGate64(progress)); }

inline i32    ZDecompress(Stream& out, Stream& in, Gate2<i32, i32> progress)   { return (i32)ZDecompress(out, in, AsGate64(progress)); }
inline Txt    ZDecompress(const Txt& s, Gate2<i32, i32> progress)           { return ZDecompress(s, AsGate64(progress)); }
inline Txt    ZDecompress(ukk data, i32 len, Gate2<i32, i32> progress) { return ZDecompress(data, len, AsGate64(progress)); }

inline i32    GZCompress(Stream& out, Stream& in, i32 size, Gate2<i32, i32> progress) { return (i32)GZCompress(out, in, size, AsGate64(progress)); }
inline i32    GZCompress(Stream& out, Stream& in, Gate2<i32, i32> progress)           { return (i32)GZCompress(out, in, AsGate64(progress)); }
inline Txt    GZCompress(ukk data, i32 len, Gate2<i32, i32> progress)         { return GZCompress(data, len, AsGate64(progress)); }
inline Txt    GZCompress(const Txt& s, Gate2<i32, i32> progress)                   { return GZCompress(s, AsGate64(progress)); }

inline i32    GZDecompress(Stream& out, Stream& in, i32 size, Gate2<i32, i32> progress) { return (i32)GZDecompress(out, in, size, AsGate64(progress)); }
inline i32    GZDecompress(Stream& out, Stream& in, Gate2<i32, i32> progress)           { return (i32)GZDecompress(out, in, AsGate64(progress)); }
inline Txt    GZDecompress(ukk data, i32 len, Gate2<i32, i32> progress)         { return GZDecompress(data, len, AsGate64(progress)); }
inline Txt    GZDecompress(const Txt& s, Gate2<i32, i32> progress)                   { return GZDecompress(s, AsGate64(progress)); }

Txt FastCompress(ukk s, i32 sz);
Txt FastCompress(const Txt& s);
Txt FastDecompress(const Txt& data);

///ZIP

class UnZip {
	struct File : Moveable<File> {
		u16   bit;
		Txt path;
		u32  time;
		i32    method;
		u32  crc;
		u32  csize;
		u32  usize;
		i64  offset;
	};
	
	Stream      *zip;
	bool         error;
	Vec<File> file;
	i32          current;

	void   ReadDir();

	static Time   GetZipTime(u32 time);

public:
	bool   IsEof() const          { return current >= file.GetCount(); }
	operator bool() const         { return !IsEof() && !IsError(); }
	
	bool   IsError() const        { return error; }
	void   ClearError()           { error = false; }

	i32    GetCount() const       { return file.GetCount(); }
	Txt GetPath(i32 i) const   { return file[i].path; }
	bool   IsFolder(i32 i) const  { return *file[i].path.Last() == '/'; }
	bool   IsFile(i32 i) const    { return !IsFolder(i); }
	i32    GetLength(i32 i) const { return file[i].usize; }
	Time   GetTime(i32 i) const   { return GetZipTime(file[i].time); }

	void   Seek(i32 i)            { ASSERT(i >= 0 && i < file.GetCount()); current = i; }

	bool   IsFolder() const       { return IsFolder(current); }
	bool   IsFile() const         { return !IsFolder(); }
	Txt GetPath() const        { return GetPath(current); }
	i32    GetLength() const      { return GetLength(current); }
	Time   GetTime() const        { return GetTime(current); }

	void   Skip()                 { current++; }
	void   SkipFile()             { current++; }
	bool   ReadFile(Stream& out, Gate<i32, i32> progress = Null);
	Txt ReadFile(Gate<i32, i32> progress = Null);

	Txt ReadFile(tukk path, Gate<i32, i32> progress = Null);

	void   Create(Stream& in);
	void   Close()                { file.Clear(); zip->Close(); }

	UnZip(Stream& in);
	UnZip();
	virtual ~UnZip();
};

class FileUnZip : public UnZip {
	FileIn zip;

public:
	bool Create(tukk name);
	
	FileUnZip(tukk name)                 { Create(name); }
	FileUnZip()                                 {}
};

class MemUnZip : public UnZip {
	MemReadStream zip;

public:
	void Create(ukk ptr, i32 count);

	MemUnZip(ukk ptr, i32 count)       { Create(ptr, count); }
	MemUnZip();
};

class TxtUnZip : public UnZip {
	TxtStream zip;

public:
	void Create(const Txt& s);

	TxtUnZip(const Txt& s)                { Create(s); }
	TxtUnZip();
};

class Zip {
	Stream *zip;

	struct File {
		Txt    path;
		u32    time;
		i32    version;
		i32    gpflag;
		i32    method;
		u32    crc;
		u32    csize;
		u32    usize;
	};
	Array<File> file;

	u32   done;

	One<Zlib> pipeZLib;
	Crc32Stream crc32; // for uncompressed files
	bool        uncompressed;

	void WriteFile0(ukk ptr, i32 size, tukk path, Gate<i32, i32> progress, Time tm, i32 method);

	void FileHeader(tukk path, Time tm);

	void PutCompressed(ukk data, i32 size);
	
	typedef Zip CLASSNAME;

public:
	Callback WhenError;

	void BeginFile(tukk path, Time tm = GetSysTime(), bool deflate = true);
	void BeginFile(OutFilterStream& oz, tukk path, Time tm = GetSysTime(), bool deflate = true);
	void Put(ukk data, i32 size);
	void EndFile();
	bool IsFileOpened() const                 { return pipeZLib || uncompressed; }

	void WriteFolder(tukk path, Time tm = GetSysTime());
	void WriteFile(ukk ptr, i32 size, tukk path, Gate<i32, i32> progress = Null, Time tm = GetSysTime(), bool deflate = true);
	void WriteFile(const Txt& s, tukk path, Gate<i32, i32> progress = Null, Time tm = GetSysTime(), bool deflate = true);

	void Create(Stream& out);
	void Finish();
	
	bool IsError()                           { return zip && zip->IsError(); }

	u32  GetLength() const                 { return done; }
	
	Zip();
	Zip(Stream& out);
	virtual ~Zip();
};

class FileZip : public Zip {
	FileOut zip;

public:
	bool Create(tukk name);
	bool Finish();
	
	FileZip(tukk name)                 { Create(name); }
	FileZip()                                 {}
	~FileZip()                                { Finish(); }
};

class TxtZip : public Zip {
	TxtStream zip;

public:
	void   Create();
	Txt    Finish();

	TxtZip()                               { Create(); }
};
///ZSTD

#define ZSTD_STATIC_LINKING_ONLY

class ZstdCompressStream : public Stream  {
public:
	virtual   void  Close();
	virtual   bool  IsOpen() const;

protected:
	virtual   void  _Put(i32 w);
	virtual   void  _Put(ukk data, u32 size);

	Stream      *out;

	Buffer<u8> buffer;
	Buffer<u8> outbuf;
	Buffer<i32>  outsz;

	enum { BLOCK_BYTES = 1024*1024 };

	i32           level;

	bool          concurrent;
    
    void          Alloc();
	void          Init();
	void          FlushOut();

public:
	void Co(bool b = true);
	void Open(Stream& out, i32 level = 1);

	ZstdCompressStream();
	ZstdCompressStream(Stream& out, i32 level = 1) : ZstdCompressStream() { Open(out, level); }
	~ZstdCompressStream();
};

class ZstdDecompressStream : public Stream {
public:
	virtual   bool  IsOpen() const;

protected:
	virtual   i32   _Term();
	virtual   i32   _Get();
	virtual   u32   _Get(uk data, u32 size);

private:
	Stream        *in;
	struct Workblock {
		Txt          compressed_data; // can contain more frames
		i32          frame_at, frame_sz; // position and compressed size of frame
		i32          decompressed_sz;
		Buffer<char> decompressed_data; // decompressed data
		bool         irregular_d = false; // d reallocated to accomodate bigger result

		ukk FramePtr() { return ~compressed_data + frame_at; }
		void Clear()           { compressed_data.Clear(); decompressed_data.Clear(); irregular_d = false; }
	};

	Txt    compressed_data; // buffer to store compressed data
	i32    compressed_at; // where are we in above buffer

	Workblock wb[16];
	i32       count; // count of workblocks fetched
	i32       ii; // next workblock to be read
	i32       dlen; // length of current workblock

	enum { BLOCK_BYTES = 1024*1024 }; // expected decompressed size

	i32          maxblock;
	i32          blockchksumsz;
	u8           lz4hdr;
	bool         eof;

	bool         concurrent;

    void          TryHeader();

	void          Init();
	bool          Next();
	void          Fetch();
	bool          Ended() const { return IsError() || in->IsError() || (ptr == rdlim && ii == (count && eof)); }

public:
	bool Open(Stream& in);
	void Co(bool b = true)                                    { concurrent = b; }

	ZstdDecompressStream();
	ZstdDecompressStream(Stream& in) : ZstdDecompressStream() { Open(in); }
	~ZstdDecompressStream();
};

i64 ZstdCompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
i64 ZstdDecompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt ZstdCompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt ZstdCompress(const Txt& s, Gate<i64, i64> progress = Null);
Txt ZstdDecompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt ZstdDecompress(const Txt& s, Gate<i64, i64> progress = Null);

i64 CoZstdCompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
i64 CoZstdDecompress(Stream& out, Stream& in, Gate<i64, i64> progress = Null);
Txt CoZstdCompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt CoZstdCompress(const Txt& s, Gate<i64, i64> progress = Null);
Txt CoZstdDecompress(ukk data, i64 len, Gate<i64, i64> progress = Null);
Txt CoZstdDecompress(const Txt& s, Gate<i64, i64> progress = Null);

bool IsZstd(Stream& s);

//BZ2
namespace bz2 {
	class Lib {
		enum { NONE, DEFLATE, INFLATE };
	
		bz_stream     z;
		Buffer<char>  output;
		i32           chunk;
		i32           mode;
		i32           total;
		i32           compression_level;
		bool          error;
		bool          rdall;
		bool          eos;
		Txt           out;
		
		void          SetError(bool v) { error = v; }
		void          PutOut(ukk ptr, i32 size);
		void          Pump(bool finish);
		void          Begin();
		void          Free();
		i32           GzipHeader(tukk ptr, i32 size);
		void          Init();
	
	public:
		Event<ukk , i32>  WhenOut;
		
		void Compress();
		void Decompress(bool all = true);
		void Put(ukk ptr, i32 size);
		void Put(const Txt& s)              { Put(~s, s.GetCount()); }
		void End();
		void Clear();
		
		const Txt& Get() const              { return out; }
		operator const Txt&() const         { return out; }
		const Txt& operator~() const        { return out; }
		void ClearOut()                        { out.Clear(); }
	
		bool   IsError() const                 { return error; }
		bool   IsEOS() const                   { return eos; }
		
		Lib& ChunkSize(i32 n);
		Lib& Level(i32 compression_lvl)        { compression_level = compression_lvl; return *this; }
	
		Lib();
		~Lib();
	};

	class CompressStream : public OutFilterStream {
		Lib z;
	
	public:
		void Open(Stream& out)                 { z.Compress(); Set(out, z); }
	
		Lib& ChunkSize(i32 n)                  { return z.ChunkSize(n); }
		Lib& Level(i32 compression_lvl)        { return z.Level(compression_lvl); }
	
		CompressStream()                       {}
		CompressStream(Stream& out)            { Open(out); }
		~CompressStream()                      { Close(); }
	};
	
	class DecompressStream : public InFilterStream {
		Lib z;
	
	public:
		void Open(Stream& in, bool all = true) { z.Decompress(all); Set(in, z);
		                               FilterEof = [=, this]() -> bool { return z.IsEOS(); }; }
		                               
		Lib& ChunkSize(i32 n)                  { return z.ChunkSize(n); }
	
		DecompressStream()                             {}
		DecompressStream(Stream& out, bool all = true) { Open(out, all); }
		~DecompressStream()                            { Close(); }
	};
}

typedef bz2::CompressStream BZ2CompressStream;
typedef bz2::DecompressStream BZ2DecompressStream;

void          BZ2Compress(Stream& out, Stream& in, Gate<i32, i32> progress = Null);
void          BZ2Decompress(Stream& out, Stream& in, Gate<i32, i32> progress = Null);

Txt        BZ2Compress(Stream& in, Gate<i32, i32> progress = Null);
Txt        BZ2Decompress(Stream& in, Gate<i32, i32> progress = Null);

Txt        BZ2Compress(ukk data, i64 len, Gate<i32, i32> progress);
Txt        BZ2Decompress(ukk data, i64 len, Gate<i32, i32> progress = Null);

Txt        BZ2Compress(const Txt& data, Gate<i32, i32> progress = Null);
Txt        BZ2Decompress(const Txt& data, Gate<i32, i32> progress = Null);


};//endns
#endif
