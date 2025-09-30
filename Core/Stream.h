#ifdef  _DEBUG
#define NEWBLOCKSTREAM
#endif

enum {
	STRM_ERROR   =  0x20,
	STRM_READ    =  0x10,
	STRM_WRITE   =  0x08,
	STRM_SEEK    =  0x04,
	STRM_LOADING =  0x02,
	STRM_THROW   =  0x01,

	ERROR_NOT_ENOUGH_SPACE = -1,
	ERROR_LOADING_FAILED   = -2
};

struct StreamError {};
struct LoadingError : StreamError {};

enum EOLenum { EOL };

class Huge;

class Stream {
protected:
	z64  pos;
	u8  *buffer;
	u8  *ptr;
	u8  *rdlim;
	u8  *wrlim;

	unsigned style:6;
	unsigned errorcode:16;

	i32      version = 0;

	enum {
		BEGINOFLINE = 0x02,
	};

	virtual   void  _Put(i32 w);
	virtual   i32   _Term();
	virtual   i32   _Get();
	virtual   void  _Put(ukk data, dword size);
	virtual   dword _Get(uk data, dword size);

private:
	i32       _Get8();
	i32       _Get16();
	i32       _Get32();
	z64     _Get64();

public:
	virtual   void  Seek(z64 pos);
	virtual   z64 GetSize() const;
	virtual   void  SetSize(z64 size);
	virtual   void  Flush();
	virtual   void  Close();
	virtual   bool  IsOpen() const = 0;

	Stream();
	virtual  ~Stream();

	word      GetStyle() const       { return style; }

	void      SetVersion(i32 ver)    { version = ver; }
	i32       GetVersion() const     { return version; }

	bool      IsError() const        { return style & STRM_ERROR; }
	bool      IsOK() const           { return !IsError(); }
	void      SetError(i32 c = 0)    { style |= STRM_ERROR; errorcode = c; }
#ifdef TARGET_WIN32
	void      SetLastError()         { SetError(GetLastError()); }
#endif
#ifdef TARGET_POSIX
	void      SetLastError()         { SetError(errno); }
#endif
	i32       GetError() const       { return errorcode; }
	Txt    GetErrorText() const;
	void      ClearError()           { style = style & ~STRM_ERROR; errorcode = 0; }

	z64     GetPos() const         { return dword(ptr - buffer) + pos; }
	z64     GetLeft() const        { return GetSize() - GetPos(); }
	void      SeekEnd(z64 rel = 0) { Seek(GetSize() + rel); }
	void      SeekCur(z64 rel)     { Seek(GetPos() + rel); }

	bool      IsEof()                { return ptr >= rdlim && _Term() < 0; }

	void      Put(i32 c)             { if(ptr < wrlim) *ptr++ = c; else _Put(c); }
	i32       Term()                 { return ptr < rdlim ? *ptr : _Term(); }
	i32       Peek()                 { return Term(); }
	i32       Get()                  { return ptr < rdlim ? *ptr++ : _Get(); }

	u8k *PeekPtr(i32 size = 1){ ASSERT(size > 0); return ptr + size <= rdlim ? ptr : NULL; }
	u8k *GetPtr(i32 size = 1) { ASSERT(size > 0); if(ptr + size <= rdlim) { u8 *p = ptr; ptr += size; return p; }; return NULL; }
	u8       *PutPtr(i32 size = 1) { ASSERT(size > 0); if(ptr + size <= wrlim) { u8 *p = ptr; ptr += size; return p; }; return NULL; }
	u8k *GetSzPtr(i32& size)  { Term(); size = i32(rdlim - ptr); u8 *p = ptr; ptr += size; return p; }

	void      Put(ukk data, i32 size)  { ASSERT(size >= 0); if(size) { if(ptr + size <= wrlim) { memcpy8(ptr, data, size); ptr += size; } else _Put(data, size); } }
	i32       Get(uk data, i32 size)        { ASSERT(size >= 0); if(ptr + size <= rdlim) { memcpy8(data, ptr, size); ptr += size; return size; } return _Get(data, size); }

	void      Put(const Txt& s)   { Put((tukk ) s, s.GetLength()); }
	Txt    Get(i32 size);
	Txt    GetAll(i32 size);

	i32       Skip(i32 size);

	void      LoadThrowing()         { style |= STRM_THROW; }
	void      LoadError();

	bool      GetAll(uk data, i32 size);

	void      Put64(ukk data, z64 size);
	z64     Get64(uk data, z64 size);
	bool      GetAll64(uk data, z64 size);

	size_t    Get(Huge& h, size_t size);
	bool      GetAll(Huge& h, size_t size);

	i32       Get8()                 { return ptr < rdlim ? *ptr++ : _Get8(); }
	i32       Get16()                { if(ptr + 1 >= rdlim) return _Get16(); i32 q = Peek16(ptr); ptr += 2; return q; }
	i32       Get32()                { if(ptr + 3 >= rdlim) return _Get32(); i32 q = Peek32(ptr); ptr += 4; return q; }
	z64     Get64()                { if(ptr + 7 >= rdlim) return _Get64(); z64 q = Peek64(ptr); ptr += 8; return q; }

	i32       GetUtf8();

	Txt    GetLine();

	void      Put16(word q)          { if(ptr + 1 < wrlim) { Poke16(ptr, q); ptr += 2; } else Put(&q, 2); }
	void      Put32(dword q)         { if(ptr + 3 < wrlim) { Poke32(ptr, q); ptr += 4; } else Put(&q, 4); }
	void      Put64(z64 q)         { if(ptr + 7 < wrlim) { Poke64(ptr, q); ptr += 8; } else Put(&q, 8); }

#ifdef CPU_LE
	i32       Get16le()              { return Get16(); }
	i32       Get32le()              { return Get32(); }
	z64     Get64le()              { return Get64(); }
	i32       Get16be()              { return SwapEndian16(Get16()); }
	i32       Get32be()              { return SwapEndian32(Get32()); }
	z64     Get64be()              { return SwapEndian64(Get64()); }

	void      Put16le(word q)        { Put16(q); }
	void      Put32le(dword q)       { Put32(q); }
	void      Put64le(z64 q)       { Put64(q); }
	void      Put16be(word q)        { Put16(SwapEndian16(q)); }
	void      Put32be(dword q)       { Put32(SwapEndian32(q)); }
	void      Put64be(z64 q)       { Put64(SwapEndian64(q)); }
#else
	i32       Get16le()              { return SwapEndian16(Get16()); }
	i32       Get32le()              { return SwapEndian32(Get32()); }
	z64     Get64le()              { return SwapEndian64(Get64()); }
	i32       Get16be()              { return Get16(); }
	i32       Get32be()              { return Get32(); }
	z64     Get64be()              { return Get64(); }

	void      Put16le(word q)        { Put16(SwapEndian16(q)); }
	void      Put32le(dword q)       { Put32(SwapEndian32(q)); }
	void      Put64le(z64 q)       { Put64(SwapEndian64(q)); }
	void      Put16be(word q)        { Put16(q); }
	void      Put32be(dword q)       { Put32(q); }
	void      Put64be(z64 q)       { Put64(q); }
#endif

	void      PutUtf8(i32 c);

	void      Put(tukk s);
	void      Put(i32 c, i32 count);
	void      Put0(i32 count)        { Put(0, count); }

	void      PutCrLf()              { Put16(MAKEWORD('\r', '\n')); }
#ifdef TARGET_WIN32
	void      PutEol()               { PutCrLf(); }
#else
	void      PutEol()               { Put('\n'); }
#endif

	Stream&   operator<<(EOLenum)    { PutEol(); return *this; }

	void      PutLine(tukk s);
	void      PutLine(const Txt& s);

	void      Put(Stream& s, z64 size = INT64_MAX, dword click = 4096);

//  Stream as serialization streamer
	void      SetLoading()                 { ASSERT(style & STRM_READ); style |= STRM_LOADING; }
	void      SetStoring()                 { ASSERT(style & STRM_WRITE); style &= ~STRM_LOADING; }
	bool      IsLoading() const            { return style & STRM_LOADING; }
	bool      IsStoring() const            { return !IsLoading(); }

	void      SerializeRaw(u8 *data, z64 count);
	void      SerializeRaw(word *data, z64 count);
	void      SerializeRaw(i16 *data, z64 count);
	void      SerializeRaw(dword *data, z64 count);
	void      SerializeRaw(i32 *data, z64 count);
	void      SerializeRaw(zu64 *data, z64 count);
	void      SerializeRaw(float *data, z64 count);
	void      SerializeRaw(double *data, z64 count);

	Txt    GetAllRLE(i32 size);
	void      SerializeRLE(u8 *data, i32 count);

	Stream&   SerializeRaw(u8 *data)     { if(IsLoading()) *data = Get(); else Put(*data); return *this; }
	Stream&   SerializeRaw(word *data)     { if(IsLoading()) *data = Get16le(); else Put16le(*data); return *this; }
	Stream&   SerializeRaw(dword *data)    { if(IsLoading()) *data = Get32le(); else Put32le(*data); return *this; }
	Stream&   SerializeRaw(zu64 *data)   { if(IsLoading()) *data = Get64le(); else Put64le(*data); return *this; }

	Stream&   operator%(bool& d)           { u8 b; if(IsStoring()) b = d; SerializeRaw(&b); d = b; return *this; }
	Stream&   operator%(char& d)           { return SerializeRaw((u8 *)&d); }
	Stream&   operator%(i8& d)    { return SerializeRaw((u8 *)&d); }
	Stream&   operator%(u8& d)  { return SerializeRaw((u8 *)&d); }
	Stream&   operator%(short& d)          { return SerializeRaw((word *)&d); }
	Stream&   operator%(unsigned short& d) { return SerializeRaw((word *)&d); }
	Stream&   operator%(i32& d)            { return SerializeRaw((dword *)&d); }
	Stream&   operator%(u32& d)   { return SerializeRaw((dword *)&d); }
	Stream&   operator%(long& d)           { return SerializeRaw((dword *)&d); }
	Stream&   operator%(u64& d)  { return SerializeRaw((dword *)&d); }
	Stream&   operator%(float& d)          { return SerializeRaw((dword *)&d); }
	Stream&   operator%(double& d)         { return SerializeRaw((zu64 *)&d); }
	Stream&   operator%(z64& d)          { return SerializeRaw((zu64 *)&d); }
	Stream&   operator%(zu64& d)         { return SerializeRaw((zu64 *)&d); }

	Stream&   operator%(Txt& s);
	Stream&   operator/(Txt& s);

	Stream&   operator%(WTxt& s);
	Stream&   operator/(WTxt& s);

	void      Pack(dword& i);
	Stream&   operator/(i32& i);
	Stream&   operator/(u32& i);
	Stream&   operator/(long& i);
	Stream&   operator/(u64& i);

	void      Magic(dword magic = 0x7d674d7b);

	void      Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f, bool& g, bool& h);
	void      Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f, bool& g);
	void      Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f);
	void      Pack(bool& a, bool& b, bool& c, bool& d, bool& e);
	void      Pack(bool& a, bool& b, bool& c, bool& d);
	void      Pack(bool& a, bool& b, bool& c);
	void      Pack(bool& a, bool& b);

#ifdef DEPRECATED
	i32       GetW()                 { return Get16(); }
	i32       GetL()                 { return Get32(); }
	i32       GetIW()                { return Get16le(); }
	i32       GetIL()                { return Get32le(); }
	i32       GetMW()                { return Get16be(); }
	i32       GetML()                { return Get32be(); }
	void      PutW(i32 c)            { Put16(c); }
	void      PutL(i32 c)            { Put32(c); }
	void      PutIW(i32 c)           { Put16le(c); }
	void      PutIL(i32 c)           { Put32le(c); }
	void      PutMW(i32 c)           { Put16be(c); }
	void      PutML(i32 c)           { Put32be(c); }
	void      PutW(const char16 *s, i32 count)       { Put(s, count * 2); }
	dword     GetW(char16 *s, i32 count)             { return Get(s, count * 2) / 2; }
	bool      GetAllW(char16 *s, i32 count)          { return GetAll(s, count * 2); }
#endif
private: // No copy
	Stream(const Stream& s);
	void operator=(const Stream& s);
};

class TxtStream : public Stream {
protected:
	virtual  void  _Put(i32 w);
	virtual  i32   _Term();
	virtual  i32   _Get();
	virtual  void  _Put(ukk data, dword size);
	virtual  dword _Get(uk data, dword size);

public:
	virtual  void  Seek(z64 pos);
	virtual  z64 GetSize() const;
	virtual  void  SetSize(z64 size);
	virtual  bool  IsOpen() const;

protected:
	bool           writemode;
	Txt         data;
	TxtBuf   wdata;
	dword          size;
	i32            limit = INT_MAX;

	void           InitReadMode();
	void           SetWriteBuffer();
	void           SetReadMode();
	void           SetWriteMode();

public:
	void        Open(const Txt& data);
	void        Create();
	void        Reserve(i32 n);

	Txt      GetResult();
	operator    Txt()                              { return GetResult(); }

	void        Limit(i32 sz)                         { limit = sz; }

	struct LimitExc : public StreamError {};

	TxtStream()                           { Create(); }
	TxtStream(const Txt& data)         { Open(data); }
};

class MemStream : public Stream {
protected:
	virtual   void  _Put(ukk data, dword size);
	virtual   dword _Get(uk data, dword size);

public:
	virtual   void  Seek(z64 pos);
	virtual   z64 GetSize() const;
	virtual   bool  IsOpen() const;

public:
	void Create(uk data, z64 size);

	MemStream();
	MemStream(uk data, z64 size);
};

class MemReadStream : public MemStream {
public:
	void Create(ukk data, z64 size);

	MemReadStream(ukk data, z64 size);
	MemReadStream();
};

class BlockStream : public Stream {
protected:
	virtual  void  _Put(i32 w);
	virtual  i32   _Term();
	virtual  i32   _Get();
	virtual  void  _Put(ukk data, dword size);
	virtual  dword _Get(uk data, dword size);

public:
	virtual  void  Seek(z64 pos);
	virtual  z64 GetSize() const;
	virtual  void  SetSize(z64 size);
	virtual  void  Flush();

private:
	i32           pagesize;
	z64         pagemask;
	z64         pagepos;
	bool          pagedirty;

	z64         streamsize;

	void          SetPos(z64 p);
	void          SyncSize();
	bool          SyncPage();
	bool          SyncPos();
	void          ReadData(uk data, z64 at, i32 size);

protected:
	virtual  dword Read(z64 at, uk ptr, dword size);
	virtual  void  Write(z64 at, ukk data, dword size);
	virtual  void  SetStreamSize(z64 size);

public:
	enum {
		READ, CREATE, APPEND, READWRITE,

		NOWRITESHARE = 0x10,
		SHAREMASK = 0x70,
#ifdef DEPRECATED
		DELETESHARE = 0x20,
		NOREADSHARE = 0x40,
#endif
	};

	dword     GetBufferSize() const           { return pagesize; }
	void      SetBufferSize(dword newsize);
	z64     GetStreamSize() const           { return streamsize; }

	BlockStream();
	virtual ~BlockStream();

protected:
	void     OpenInit(dword mode, z64 file_size);
};

class FileStream : public BlockStream {
protected:
	virtual  void  SetStreamSize(z64 size);
	virtual  dword Read(z64 at, uk ptr, dword size);
	virtual  void  Write(z64 at, ukk data, dword size);

public:
	virtual  void  Close();
	virtual  bool  IsOpen() const;

protected:
#ifdef TARGET_WIN32
	HANDLE    handle;
#endif
#ifdef TARGET_POSIX
	i32       handle;
#endif

	void      SetPos(z64 pos);
	void      Init(z64 size);

public:
	operator  bool() const                 { return IsOpen(); }
	FileTime  GetTime() const;

#ifdef TARGET_WIN32
	void      SetTime(const FileTime& tm);
	bool      Open(tukk filename, dword mode);

	FileStream(tukk filename, dword mode);
#endif

#ifdef TARGET_POSIX
	bool      Open(tukk filename, dword mode, mode_t acm = 0644);
	FileStream(tukk filename, dword mode, mode_t acm = 0644);
	FileStream(i32 std_handle);
#endif

	FileStream();
	~FileStream();

#ifdef TARGET_WIN32
	HANDLE    GetHandle() const            { return handle; }
#endif
#ifdef TARGET_POSIX
	i32       GetHandle() const            { return handle; }
#endif
};

class FileOut : public FileStream {
public:
#ifdef TARGET_POSIX
	bool Open(tukk fn, mode_t acm = 0644);
#endif
#ifdef TARGET_WIN32
	bool Open(tukk fn);
#endif

	FileOut(tukk fn)                { Open(fn); }
	FileOut()                              {}
};

class FileAppend : public FileStream {
public:
	bool Open(tukk fn)         { return FileStream::Open(fn, FileStream::APPEND|FileStream::NOWRITESHARE); }

	FileAppend(tukk fn)        { Open(fn); }
	FileAppend()                      {}
};

class FileIn : public FileStream {
public:
	bool Open(tukk fn)         { return FileStream::Open(fn, FileStream::READ); }

	FileIn(tukk fn)            { Open(fn); }
	FileIn()                          {}
};

class SizeStream : public Stream {
protected:
	virtual void  _Put(i32 w);
	virtual void  _Put(ukk data, dword size);

public:
	virtual z64 GetSize() const;
	virtual bool  IsOpen() const;

protected:
	u8    h[256];

public:
	operator z64() const            { return GetSize(); }

	void     Open()                   { ptr = buffer; ClearError(); }

	SizeStream();
};

class CompareStream : public Stream {
protected:
	virtual  void  _Put(i32 w);
	virtual  void  _Put(ukk data, dword size);

public:
	virtual  void  Seek(z64 pos);
	virtual  z64 GetSize() const;
	virtual  void  SetSize(z64 size);
	virtual  void  Close();
	virtual  bool  IsOpen() const;
	virtual  void  Flush();

private:
	Stream  *stream;
	bool     equal;
	z64    size;
	u8     h[1024];

	void     Compare(z64 pos, ukk data, i32 size);

public:
	void     Open(Stream& aStream);

	bool     IsEqual()                         { Flush(); return equal; }
	operator bool()                            { Flush(); return equal; }

	CompareStream();
	CompareStream(Stream& aStream);
};

class OutStream : public Stream {
	u8 *h;

protected:
	virtual  void  _Put(i32 w);
	virtual  void  _Put(ukk data, dword size);
	virtual  bool  IsOpen() const;

	virtual  void  Out(ukk data, dword size) = 0;

public:
	virtual  void  Close();

	void     Flush();

	OutStream();
	~OutStream();
};

class TeeStream : public OutStream {
protected:
	virtual  void  Out(ukk data, dword size);

private:
	Stream& a;
	Stream& b;

public:
	TeeStream(Stream& a, Stream& b) : a(a), b(b) {}
	~TeeStream()                                 { Close(); }
};

class FileMapping
{
public:
	FileMapping(tukk file = NULL, bool delete_share = false);
	~FileMapping() { Close(); }

	bool        Open(tukk file, bool delete_share = false);
	bool        Create(tukk file, z64 filesize, bool delete_share = false);
	bool        Expand(z64 filesize);
	bool        Map(z64 offset, size_t len);
	bool        Unmap();
	bool        Close();

	bool        IsOpen() const            { return hfile != INVALID_HANDLE_VALUE; }

	z64       GetFileSize() const       { return filesize; }
	Time        GetTime() const;
	Txt      GetData(z64 offset, i32 len);

	z64       GetOffset() const         { return offset; }
	size_t      GetCount() const          { return size; }

	z64       GetRawOffset() const      { return rawoffset; }
	size_t      GetRawCount() const       { return rawsize; }

	u8k *operator ~ () const       { ASSERT(IsOpen()); return base; }
	u8k *Begin() const             { ASSERT(IsOpen()); return base; }
	u8k *End() const               { ASSERT(IsOpen()); return base + size; }
	u8k *GetIter(i32 i) const      { ASSERT(IsOpen() && i >= 0 && (size_t)i <= size); return base + i; }
	u8k& operator [] (i32 i) const { ASSERT(IsOpen() && i >= 0 && (size_t)i < size); return base[i]; }

	u8       *operator ~ ()             { ASSERT(IsOpen()); return base; }
	u8       *Begin()                   { ASSERT(IsOpen()); return base; }
	u8       *End()                     { ASSERT(IsOpen()); return base + size; }
	u8       *GetIter(i32 i)            { ASSERT(IsOpen() && i >= 0 && (size_t)i <= size); return base + i; }
	u8&       operator [] (i32 i)       { ASSERT(IsOpen() && i >= 0 && (size_t)i < size); return base[i]; }

private:
#ifdef TARGET_WIN32
	HANDLE      hfile;
	HANDLE      hmap;
#endif
#ifdef TARGET_POSIX
	enum { INVALID_HANDLE_VALUE = -1 };
	i32	        hfile;
	struct stat hfstat;
#endif
	u8       *base;
	u8       *rawbase;
	z64       filesize;
	z64       offset;
	z64       rawoffset;
	size_t      size;
	size_t      rawsize;
	bool        write;
};


Txt LoadStream(Stream& in);
bool   SaveStream(Stream& out, const Txt& data);

i64 CopyStream(Stream& dest, Stream& src, i64 count = INT64_MAX);

#ifndef TARGET_WINCE
void    CoutUTF8();
Stream& Cout();
Stream& Cerr();
Txt  ReadStdIn();
Txt  ReadSecret();
void    EnableEcho(bool b = true);
void    DisableEcho();
#endif

Stream& NilStream();

Txt LoadFile(tukk filename);
bool   SaveFile(tukk filename, const Txt& data);

template <class T>
inline Stream& operator%(Stream& s, T& x)
{
	x.Serialize(s);
	return s;
}

inline Stream& operator<<(Stream& s, tukk x)
{
	s.Put(x);
	return s;
}

inline Stream& operator<<(Stream& s, tuk x)
{
	s.Put(x);
	return s;
}

inline Stream& operator<<(Stream& s, const Txt &x)
{
	s.Put(x);
	return s;
}

inline Stream& operator<<(Stream& s, char x)
{
	s.Put((i32) x);
	return s;
}

inline Stream& operator<<(Stream& s, ukk x)
{
	s << FormatPtr(x);
	return s;
}

inline Stream& operator<<(Stream& s, uk x)
{
	s << FormatPtr(x);
	return s;
}

template <class T>
inline Stream& operator<<(Stream& s, const T& x) {
	s << AsTxt(x);
	return s;
}

// ------

Stream& Pack16(Stream& s, i32& i);
Stream& Pack16(Stream& s, i32& i1, i32& i2);
Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3);
Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3, i32& i4);
Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3, i32& i4, i32& i5);

i32     StreamHeading(Stream& stream, i32 ver, i32 minver, i32 maxver, tukk tag);