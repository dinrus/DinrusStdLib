#include "../Core.h"
//#include <drx/Core/Compress.h>

#ifdef TARGET_POSIX
#include <sys/mman.h>
#include <termios.h>
#endif

namespace drx {

#define LLOG(x) // RLOG(x)
#define LDUMP(x) // RDUMP(x)
#define LLOGHEXDUMP(x, y) // RLOGHEXDUMP(x, y)

void Stream::_Put(ukk data, dword size) {
	u8k *s = (u8k *) data;
	while(size--)
		Put(*s++);
}

dword Stream::_Get(uk data, dword size) {
	i32 c;
	u8 *s = (u8 *) data;
	dword sz;
	for(sz = 0; sz < size && (c = Get()) >= 0; sz++)
		*s++ = c;
	return sz;
}

void Stream::_Put(i32 w) {
	SetError(ERROR_NOT_ENOUGH_SPACE);
}

i32  Stream::_Get() {
	return -1;
}

i32  Stream::_Term() {
	return -1;
}

void Stream::Seek(z64) {
	NEVER();
}

i32 Stream::Skip(i32 size)
{
	i32 r = 0;
	while(size) {
		i32 n = min(i32(rdlim - ptr), size);
		if(n == 0) {
			if(Get() < 0)
				break;
			r++;
			size--;
		}
		else {
			size -= n;
			r += n;
			ptr += n;
		}
	}
	return r;
}

z64 Stream::GetSize() const {
	return 0;
}

void Stream::SetSize(z64) {
	NEVER();
}

bool Stream::IsOpen() const { return false; }

void Stream::Close() {}

void Stream::Flush() {}

Stream::Stream() {
	pos = style = 0;
	buffer = NULL;
	ptr = rdlim = wrlim = NULL;
}

Stream::~Stream() {}

void Stream::LoadError() {
	SetError(ERROR_LOADING_FAILED);
    //СДЕЛАТЬ: Ошибка здесь и в CParser!!!
    //Некоторые использования временно закоментированы!!!
	if(style & STRM_THROW)
		throw LoadingError();
}

Txt Stream::GetErrorText() const
{
   return IsError() ? drx::GetErrorMessage(errorcode) : Txt();
}

bool Stream::GetAll(uk data, i32 size) {
	if(Get(data, size) != size) {
		//LoadError();
		return false;
	}
	return true;
}

void Stream::Put64(ukk data, z64 size)
{
#ifdef CPU_64
	u8 *ptr = (u8 *)data;
	while(size > INT_MAX) {
		Put(ptr, INT_MAX);
		ptr += INT_MAX;
		size -= INT_MAX;
	}
	Put(ptr, (i32)size);
#else
	ASSERT(size <= INT_MAX);
	Put(data, (i32)size);
#endif
}

z64 Stream::Get64(uk data, z64 size)
{
#ifdef CPU_64
	u8 *ptr = (u8 *)data;
	i64 n = 0;
	while(size > INT_MAX) {
		i32 q = Get(ptr, INT_MAX);
		n += q;
		if(q != INT_MAX)
			return n;
		ptr += INT_MAX;
		size -= INT_MAX;
	}
	i32 q = Get(ptr, (i32)size);
	return n + q;
#else
	ASSERT(size <= INT_MAX);
	return Get(data, (i32)size);
#endif
}

bool Stream::GetAll64(uk data, z64 size)
{
	if(Get64(data, size) != size) {
		//LoadError();
		return false;
	}
	return true;
}

size_t Stream::Get(Huge& h, size_t size)
{
	while(h.GetSize() < size) {
		i32 sz = (i32)min((size_t)h.CHUNK, size - h.GetSize());
		i32 len = Get(h.AddChunk(), sz);
		if(len < h.CHUNK) {
			h.Finish(len);
			break;
		}
	}
	return h.GetSize();
}

bool Stream::GetAll(Huge& h, size_t size)
{
	if(Get(h, size) != size) {
		//LoadError();
		return false;
	}
	return true;
}

Txt Stream::Get(i32 size)
{
	TxtBuf b(size);
	i32 n = Get(~b, size);
	b.SetCount(n);
	return Txt(b);
}

Txt Stream::GetAll(i32 size)
{
	Txt result;
	if(size < 4 * 1024*1024)
		result = Get(size);
	else {
		Huge h;
		Get(h, size);
		result = h.Get();
	}
	if(result.GetCount() != size) {
		//LoadError();
		result = Txt::GetVoid();
	}
	return result;
}

i32  Stream::_Get8()
{
	i32 c = Get();
	if(c < 0) {
		//LoadError();
		return -1;
	}
	return c;
}

i32  Stream::_Get16() {
	word w;
	return GetAll(&w, 2) ? w : -1;
}

i32  Stream::_Get32() {
	i32 l;
	return GetAll(&l, 4) ? l : -1;
}

z64  Stream::_Get64() {
	z64 l;
	return GetAll(&l, 8) ? l : -1;
}

i32 Stream::GetUtf8()
{
	i32 code = Get();

	if(code < 0) {
		//LoadError();
		return -1;
	}

	if(code < 0x80)
		return code;

	if(code >= 0xC2) {
		i32 c = 0;
		if(code < 0xE0) {
			i32 c0 = Get();
			if(c0 >= 0x80 && c0 < 0xC0 &&
			   (c = ((code - 0xC0) << 6) + c0 - 0x80) >= 0x80 && c < 0x800)
				return c;
		}
		else
		if(code < 0xF0) {
			i32 c0 = Get();
			i32 c1 = Get();
			if(c1 >= 0x80 && c1 < 0xC0 &&
			   c0 >= 0x80 && c0 < 0xC0 &&
			   (c = ((code - 0xE0) << 12) + ((c0 - 0x80) << 6) + c1 - 0x80) >= 0x800 && c < 0x10000)
				return c;
		}
		else
		if(code < 0xF8) {
			i32 c0 = Get();
			i32 c1 = Get();
			i32 c2 = Get();
			if(c2 >= 0x80 && c2 < 0xC0 &&
			   c1 >= 0x80 && c1 < 0xC0 &&
			   c0 >= 0x80 && c0 < 0xC0 &&
			   (c = ((code - 0xF0) << 18) + ((c0 - 0x80) << 12) + ((c1 - 0x80) << 6) + c2 - 0x80) >= 0x10000 && c < 0x110000)
				return c;
		}
	}

	//LoadError();
	return -1;
}

Txt Stream::GetLine() {
	u8 *q = ptr;
	while(q < rdlim)
		if(*q == '\n') {
			Txt result((tukk )ptr, (i32)(uintptr_t)(q - ptr - (q > ptr && q[-1] == '\r')));
			ptr = q + 1;
			return result;
		}
		else
			q++;
	Txt result((tukk )ptr, (i32)(uintptr_t)(q - ptr));
	ptr = q;
	for(;;) {
		u8 *q = ptr;
		while(q < rdlim && *q != '\n')
			q++;
		result.Cat(ptr, (i32)(uintptr_t)(q - ptr));
		ptr = q;
		i32 c = Get();
		if(c == '\n')
			break;
		if(c < 0) {
			if(result.GetCount() == 0)
				return Txt::GetVoid();
			break;
		}
		result.Cat(c);
	}
	if(*result.Last() == '\r')
		result.Trim(result.GetLength() - 1);
	return result;
}

void Stream::PutUtf8(i32 c)
{
	word code = c;
	if(code < 0x80)
		Put(code);
	else
	if(code < 0x800) {
		Put(0xc0 | (code >> 6));
		Put(0x80 | (code & 0x3f));
	}
	else
	if((code & 0xFF00) == 0xEE00)
		Put(code);
	else {
		Put(0xe0 | (code >> 12));
		Put(0x80 | ((code >> 6) & 0x3f));
		Put(0x80 | (code & 0x3f));
	}
}

void Stream::Put(tukk s)
{
	while(*s) Put(*s++);
}

void Stream::Put(i32 c, i32 count) {

	while(count) {
		i32 n = min(count, (i32)(intptr_t)(wrlim - ptr));
		if(n > 0) {
			memset(ptr, c, n);
			ptr += n;
			count -= n;
		}
		else {
			Put(c);
			count--;
		}
	}
}

void Stream::PutLine(tukk s) {
	Put(s);
	PutEol();
}

void Stream::PutLine(const Txt& s) {
	Put(s);
	PutEol();
}

void  Stream::Put(Stream& s, z64 size, dword click) {
	Buffer<u8> buffer(click);
	while(size) {
		dword n = s.Get(buffer, (i32)min<i64>(click, size));
		if(n == 0)
			break;
		Put(~buffer, n);
		size -= n;
	}
}

Txt Stream::GetAllRLE(i32 size)
{
	Txt result;
	while(result.GetCount() < size) {
		i32 c = Get();
		if(c < 0)
			break;
		if(c == 0xcb) {
			c = Get();
			result.Cat(c, Get());
		}
		else
			result.Cat(c);
	}
	return result.GetCount() == size ? result : Txt::GetVoid();
}

void Stream::SerializeRLE(u8 *data, i32 size)
{
	ASSERT(size >= 0);
	if(IsError()) return;
	u8 *s =   (u8 *)data;
	u8 *lim = s + size;
	if(IsLoading())
		while(s != lim) {
			if(IsEof() || s > lim) {
				//LoadError();
				return;
			}
			u8 c = Get();
			if(c == 0xcb) {
				c = Get();
				i32 n = Get();
				if(s + n > lim) {
					//LoadError();
					return;
				}
				memset(s, c, n);
				s += n;
			}
			else
				*s++ = c;
		}
	else
		while(s < lim) {
			u8  c = *s;
			u8 *t = s + 1;
			u8 *lm = min(s + 250, lim);
			while(*t == c && t < lm)
				t++;
			if(t >= s + 3 || c == 0xcb) {
				Put(0xcb);
				Put(c);
				Put(u8(t - s));
			}
			else {
				Put(*s);

				if(t == s + 2)
					Put(*s);
			}
			if(IsError()) break;
			s = t;
		}
}

void Stream::SerializeRaw(u8 *data, z64 size)
{
	ASSERT(size >= 0);
	if(IsError()) return;
	if(IsLoading())
		GetAll64(data, size);
	else
		Put64(data, size);
}

void Stream::SerializeRaw(word *data, z64 count)
{
	ASSERT(count >= 0);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
	SerializeRaw((u8 *)data, 2 * count);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
}

void Stream::SerializeRaw(i16 *data, z64 count)
{
	SerializeRaw((word *)data, count);
}

void Stream::SerializeRaw(dword *data, z64 count)
{
	ASSERT(count >= 0);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
	SerializeRaw((u8 *)data, 4 * count);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
}

void Stream::SerializeRaw(i32 *data, z64 count)
{
	SerializeRaw((dword *)data, count);
}

void Stream::SerializeRaw(zu64 *data, z64 count)
{
	ASSERT(count >= 0);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
	SerializeRaw((u8 *)data, 8 * count);
#ifdef CPU_BE
	EndianSwap(data, count);
#endif
}

void Stream::SerializeRaw(float *data, z64 count)
{
	SerializeRaw((dword *)data, count);
}

void Stream::SerializeRaw(double *data, z64 count)
{
	SerializeRaw((zu64 *)data, count);
}

void Stream::Pack(dword& w) {
	if(IsError()) return;
	if(IsLoading()) {
		i32 q = Get();
		if(q < 0)
			LoadError();
		else {
			if(q != 255)
				w = q;
			else
				SerializeRaw(&w, 1);
		}
	}
	else {
		if(w < 255)
			Put(w);
		else {
			Put(255);
			SerializeRaw(&w, 1);
		}
	}
}

void    Stream::Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f, bool& g, bool& h) {
	if(IsError()) return;
	if(IsLoading()) {
		i32 ff = Get();
		if(ff < 0) LoadError();
		else {
			a = !!(ff & 0x80);
			b = !!(ff & 0x40);
			c = !!(ff & 0x20);
			d = !!(ff & 0x10);
			e = !!(ff & 0x08);
			f = !!(ff & 0x04);
			g = !!(ff & 0x02);
			h = !!(ff & 0x01);
		}
	}
	else {
		i32 ff = 0;
		if(a) ff |= 0x80;
		if(b) ff |= 0x40;
		if(c) ff |= 0x20;
		if(d) ff |= 0x10;
		if(e) ff |= 0x08;
		if(f) ff |= 0x04;
		if(g) ff |= 0x02;
		if(h) ff |= 0x01;
		Put(ff);
	}
}

void  Stream::Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f, bool& g) {
	bool h = false; Pack(a, b, c, d, e, f, g, h);
}

void  Stream::Pack(bool& a, bool& b, bool& c, bool& d, bool& e, bool& f) {
	bool h = false; Pack(a, b, c, d, e, f, h, h);
}

void  Stream::Pack(bool& a, bool& b, bool& c, bool& d, bool& e) {
	bool h = false; Pack(a, b, c, d, e, h, h, h);
}

void  Stream::Pack(bool& a, bool& b, bool& c, bool& d) {
	bool h = false; Pack(a, b, c, d, h, h, h, h);
}

void  Stream::Pack(bool& a, bool& b, bool& c) {
	bool h = false; Pack(a, b, c, h, h, h, h, h);
}

void  Stream::Pack(bool& a, bool& b) {
	bool h = false; Pack(a, b, h, h, h, h, h, h);
}

Stream& Stream::operator%(Txt& s) {
	if(IsError()) return *this;
	if(IsLoading()) {
		dword len;
		len = Get();
		if(len != 0xff) {
			if(len & 0x80) {
				len &= 0x7f;
				Get(); // reserved for future use... or removal
			}
		}
		else {
			len = Get32le();
			if(len & 0x80000000) {
				len &= 0x7fffffff;
				Get(); // reserved for future use... or removal
			}
		}
		s = GetAll(len);
		if(s.IsVoid())
			LoadError();
	}
	else {
		dword len = s.GetLength();
		if(len < 127)
			Put(len);
		else {
			Put(0xff);
			Put32le(len);
		}
		SerializeRaw((u8 *)~s, len);
	}
	return *this;
}

Stream& Stream::operator/(Txt& s) {
	if(IsError()) return *this;
	dword len = s.GetLength();
	Pack(len);
	if(IsLoading()) {
		s = GetAllRLE(len);
		if(s.IsVoid())
			LoadError();
	}
	else
		SerializeRLE((u8 *)~s, len);
	return *this;
}

Stream& Stream::operator%(WTxt& s)
{ // we do not support BE here anymore
	if(IsError()) return *this;
	if(IsLoading()) {
		dword len = Get();
		if(len == 0xff)
			len = Get32le();
		Txt h = GetAll(len * sizeof(char16));
		if(h.IsVoid())
			LoadError();
		else
			s = ToUtf32((const char16 *)~h, len);
	}
	else {
		Vec<char16> x = ToUtf16(s);
		dword len = x.GetCount();
		if(len < 0xff)
			Put(len);
		else {
			Put(0xff);
			Put32le(len);
		}
		SerializeRaw((u8*)x.begin(), len * sizeof(char16));
	}
	return *this;
}

Stream& Stream::operator/(WTxt& s) {
	if(IsError()) return *this;
	Txt h = ToUtf8(s);
	*this / h;
	s = ToUtf32(h);
	return *this;
}

Stream& Stream::operator/(i32& i)            { dword w = 0; if(IsStoring()) w = i + 1; Pack(w); i = w - 1; return *this; }
Stream& Stream::operator/(u32& i)   { dword w = 0; if(IsStoring()) w = i + 1; Pack(w); i = w - 1; return *this; }
Stream& Stream::operator/(long& i)           { dword w = 0; if(IsStoring()) w = i + 1; Pack(w); i = w - 1; return *this; }
Stream& Stream::operator/(u64& i)  { dword w = 0; if(IsStoring()) w = i + 1; Pack(w); i = w - 1; return *this; }

void Stream::Magic(dword magic) {
	dword a = magic;
	*this % a;
	if(magic != a) LoadError();
}

// -------------------------- Txt stream -----------------------------

void TxtStream::SetWriteBuffer()
{
	buffer = (u8 *)wdata.Begin();
	rdlim = buffer;
	wrlim = (u8 *)wdata.End();
}

void TxtStream::SetWriteMode()
{
	if(writemode) return;
	intptr_t p = ptr - buffer;
	size = data.GetLength();
	wdata = data;
	SetWriteBuffer();
	ptr = buffer + p;
	writemode = true;
}

void   TxtStream::SetReadMode()
{
	if(!writemode) return;
	wdata.SetLength((dword)GetSize());
	dword p = (dword)(uintptr_t)(ptr - buffer);
	data = wdata;
	buffer = (u8 *) ~data;
	ptr = buffer + p;
	wrlim = buffer;
	rdlim = buffer + data.GetCount();
	writemode = false;
}

void  TxtStream::Open(const Txt& adata)
{
	pos = 0;
	data = adata;
	style = STRM_READ|STRM_WRITE|STRM_SEEK|STRM_LOADING;
	wdata.Clear();
	buffer = (u8 *) ~data;
	ptr = wrlim = buffer;
	rdlim = buffer + data.GetCount();
	writemode = false;
	ClearError();
}

void  TxtStream::Create()
{
	Open(Txt());
	SetStoring();
	SetWriteMode();
	ClearError();
}

z64 TxtStream::GetSize() const
{
	return writemode ? max<i64>(GetPos(), size) : data.GetLength();
}

Txt TxtStream::GetResult()
{
	SetReadMode();
	return data;
}

void  TxtStream::_Put(ukk d, dword sz)
{
	SetWriteMode();
	if(ptr + sz >= wrlim) {
		size_t p = ptr - buffer;
		if(limit != INT_MAX && p + sz > (size_t)limit)
			throw LimitExc();
		if(p + sz >= INT_MAX)
			Panic("Превышено ограничение в 2 ГБ для TxtStream!");
		i32 len = (i32)max((z64)128, min((z64)limit, max(2 * GetSize(), GetSize() + sz)));
		wdata.SetLength(len);
		SetWriteBuffer();
		ptr = buffer + p;
	}
	memcpy8(ptr, d, sz);
	ptr += sz;
}

void TxtStream::Reserve(i32 n)
{
	SetWriteMode();
	intptr_t p = ptr - buffer;
	wdata.SetLength((i32)GetSize() + n);
	SetWriteBuffer();
	ptr = buffer + p;
}

void  TxtStream::_Put(i32 w)
{
	u8 h = w;
	_Put(&h, 1);
}

dword TxtStream::_Get(uk data, dword sz)
{
	SetReadMode();
	dword read = min((dword)(uintptr_t)(rdlim - ptr), sz);
	memcpy8(data, ptr, read);
	ptr += read;
	return read;
}

i32  TxtStream::_Get()
{
	SetReadMode();
	return ptr < rdlim ? *ptr++ : -1;
}

i32  TxtStream::_Term() {
	SetReadMode();
	return ptr < rdlim ? *ptr : -1;
}

void  TxtStream::Seek(z64 pos) {
	size = (dword)GetSize();
	if(pos > size) {
		SetWriteMode();
		size = (dword)pos;
		wdata.SetLength((dword)pos + 100);
		SetWriteBuffer();
	}
	ptr = buffer + min(GetSize(), pos);
}

void  TxtStream::SetSize(z64 asize) {
	SetWriteMode();
	dword p = (dword)(uintptr_t)GetPos();
	Seek(asize);
	size = (dword)asize;
	Seek(min(p, size));
}

bool  TxtStream::IsOpen() const {
	return true;
}

// -------------------- Memory read-write stream ------------------------

void MemStream::Seek(z64 pos) {
	ptr = buffer + min(pos, z64(rdlim - buffer));
}

z64 MemStream::GetSize() const {
	return rdlim - buffer;
}

dword MemStream::_Get(uk data, dword size) {
	if(size > (dword)(intptr_t)(rdlim - ptr))
		size = (dword)(intptr_t)(rdlim - ptr);
	memcpy8(data, ptr, size);
	ptr += size;
	return size;
}

void  MemStream::_Put(ukk data, dword size) {
	if(size > (dword)(uintptr_t)(wrlim - ptr)) {
		SetError(ERROR_NOT_ENOUGH_SPACE);
		return;
	}
	memcpy8(ptr, data, size);
	ptr += size;
}

bool  MemStream::IsOpen() const {
	return true;
}

void MemStream::Create(uk data, z64 size)
{
	style = STRM_WRITE|STRM_READ|STRM_SEEK|STRM_LOADING;
	ptr = buffer = (u8 *) data;
	wrlim = rdlim = buffer + (size_t)size;
	pos = 0;
}

MemStream::MemStream(uk data, z64 size) {
	Create(data, size);
}

MemStream::MemStream() {}

// ----------------------- Memory read streamer -------------------------

void MemReadStream::Create(ukk data, z64 size)
{
	MemStream::Create((uk )data, size);
	style = STRM_READ|STRM_SEEK|STRM_LOADING;
	wrlim = buffer;
}

MemReadStream::MemReadStream(ukk data, z64 size)
{
	Create(data, size);
}

MemReadStream::MemReadStream() {}

// --------------------------- Size stream -----------------------

z64 SizeStream::GetSize() const
{
	return i64(ptr - buffer + pos);
}

void SizeStream::_Put(ukk , dword sz)
{
	wrlim = buffer + sizeof(h);
	pos += ptr - buffer + sz;
	ptr = buffer;
}

void SizeStream::_Put(i32 w)
{
	_Put(NULL, 1);
}

bool SizeStream::IsOpen() const
{
	return true;
}

SizeStream::SizeStream()
{
	style = STRM_WRITE;
	buffer = ptr = h;
}

// ------------------------------ Compare stream ----------------------------

CompareStream::CompareStream() {
	stream = NULL;
	equal = false;
	size = 0;
	buffer = h;
}

CompareStream::CompareStream(Stream& astream) {
	stream = NULL;
	buffer = h;
	Open(astream);
}

void CompareStream::Open(Stream& astream) {
	ASSERT(astream.IsOpen());
	Close();
	style = STRM_WRITE|STRM_SEEK;
	stream = &astream;
	size = pos = 0;
	wrlim = buffer + 1024;
	ptr = buffer;
	equal = true;
	ClearError();
}

bool CompareStream::IsOpen() const {
	return !!stream;
}

z64 CompareStream::GetSize() const {
	return max(z64(ptr - buffer + pos), size);
}

void CompareStream::Close() {
	if(!stream) return;
	if(GetPos() > size)
		size = GetPos();
	Flush();
	if(stream->GetSize() != GetSize())
		equal = false;
	stream = NULL;
}

void CompareStream::SetSize(z64 asize) {
	Flush();
	pos += ptr - buffer;
	ptr = buffer;
	size = asize;
	if(pos > size)
		pos = size;
}

void CompareStream::Seek(z64 apos) {
	Flush();
	i64 sz = ptr - buffer + pos;
	if(sz > size)
		size = sz;
	pos = apos;
	ptr = buffer;
}

void CompareStream::Compare(z64 pos, ukk data, i32 size) {
	ASSERT(stream);
	if(!size) return;
	Buffer<u8> b(size);
	if(stream->GetPos() != pos)
		stream->Seek(pos);
	if(stream->Get(b, size) != size || memcmp(b.operator u8k *(), data, size))
		equal = false;
}

void CompareStream::Flush() {
	Compare(pos, buffer, (i32)(ptr - buffer));
}

void CompareStream::_Put(ukk data, dword size) {
	wrlim = buffer + sizeof(h);
	ASSERT(ptr <= wrlim);
	Flush();
	pos += ptr - buffer;
	ptr = buffer;
	u8 *b = (u8 *) data;
	while(size && equal) {
		i32 sz = min<i32>(size, sizeof(h));
		Compare(pos, b, sz);
		pos += sz;
		b += sz;
		size -= sz;
	}
}

void CompareStream::_Put(i32 w) {
	u8 b = w;
	_Put(&b, 1);
}

OutStream::OutStream()
{
	i32k bsz = 64 * 1024;
	h = (u8 *)MemoryAlloc(bsz);
	buffer = ptr = h;
	wrlim = h + bsz;
}

OutStream::~OutStream()
{	// Note: cannot call Close here !
	MemoryFree(h);
}

void OutStream::_Put(i32 w)
{
	Flush();
	*ptr++ = w;
}

void OutStream::_Put(ukk data, dword size)
{
	if(ptr == buffer)
		Out(data, size);
	else
	if(ptr + size < wrlim) {
		memcpy8(ptr, data, size);
		ptr += size;
	}
	else {
		Flush();
		Out(data, size);
	}
}

void OutStream::Flush()
{
	if(ptr != buffer) {
		Out(buffer, i32(ptr - buffer));
		ptr = h;
	}
}

void OutStream::Close()
{
	Flush();
}

bool OutStream::IsOpen() const
{
	return true;
}

void TeeStream::Out(ukk data, dword size)
{
	a.Put(data, size);
	b.Put(data, size);
}

struct NilStreamClass : public Stream {
	virtual void    _Put(i32 w)    {}
	virtual bool    IsOpen() const { return true; }
	virtual   i32   _Term()        { return -1; }
	virtual   i32   _Get()         { return -1; }
};

Stream& NilStream()
{
	return Single<NilStreamClass>();
}

#ifndef TARGET_WINCE
class CoutStream : public Stream {
#ifdef TARGET_WIN32
	Txt buffer;

	void Flush() {
		ONCELOCK {
			SetConsoleOutputCP(65001); // set console to UTF8 mode
		}
		static HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		dword dummy;
		WriteFile(h, ~buffer, buffer.GetCount(), &dummy, NULL);
		buffer.Clear();
	}
#endif


	void Put0(i32 w) {
#ifdef TARGET_WIN32
		buffer.Cat(w);
		if(CheckUtf8(buffer) || buffer.GetCount() > 8)
			Flush();
#else
		putchar(w);
#endif
	}
	virtual void    _Put(i32 w) {
		if(w == '\n') {
#ifdef TARGET_WIN32
			Put0('\r');
#endif
			Put0('\n');
		}
		else
		if(w != '\r')
			Put0(w);
	}
	virtual   bool  IsOpen() const { return true; }
#ifdef TARGET_POSIX
	virtual   void   Flush()       { fflush(stdout); }
#endif
};

Stream& Cout()
{
	return Single<CoutStream>();
}

class CerrStream : public Stream {
	virtual void    _Put(i32 w) {
	#ifdef TARGET_WIN32
		static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
		char s[1];
		s[0] = w;
		dword dummy;
		WriteFile(h, s, 1, &dummy, NULL);
	#else
		putc(w, stderr);
	#endif
	}
#ifdef TARGET_POSIX
	virtual   void  _Put(ukk data, dword size) {
		fwrite(data, 1, size, stderr);
	}
#endif
	virtual   bool  IsOpen() const { return true; }
};

Stream& Cerr()
{
	return Single<CerrStream>();
}
#endif


Txt ReadStdIn()
{
	Txt r;
	for(;;) {
		i32 c = getchar();
		if(c < 0)
			return r.GetCount() ? r : Txt::GetVoid();
		if(c == '\n')
			return r;
		r.Cat(c);
	}
}


Txt ReadSecret()
{
	DisableEcho();
	Txt s = ReadStdIn();
	EnableEcho();
	Cout().PutEol();
	return s;
}

void EnableEcho(bool b)
{
#ifdef TARGET_POSIX
	termios t;
	tcgetattr(STDIN_FILENO, &t);
	if(b) t.c_lflag |=  ECHO;
	else  t.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSADRAIN, &t);
#elif TARGET_WIN32
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(h, &mode);
	if(b) mode |=  ENABLE_ECHO_INPUT;
	else  mode &= ~ENABLE_ECHO_INPUT;
	SetConsoleMode(h, mode);
#endif
}

void DisableEcho()
{
	EnableEcho(false);
}


// ---------------------------------------------------------------------------

Txt LoadStream(Stream& in) {
	if(in.IsOpen()) {
		in.ClearError();
		i64 size = in.GetLeft();
		if(size >= 0 && size < INT_MAX) {
			TxtBuf s((i32)size);
			in.Get(s, (i32)size);
			if(!in.IsError())
				return Txt(s);
		}
	}
	return Txt::GetVoid();
}

Txt LoadFile(tukk filename) {
	FindFile ff(filename);
	if(ff && ff.IsFile()) {
	#ifdef TARGET_POSIX
		if(ff.GetLength() == 0) { // handle special cases like /proc/...
			i32 fd = open(filename,O_RDONLY);
			if(fd >= 0) {
				i32k CHUNK = 32768;
				TxtBuf s;
				for(;;) {
					i32 n = s.GetCount();
					s.SetCount(n + CHUNK);
					i32 len = read(fd, ~s + n, CHUNK);
					if(len != CHUNK) {
						if(len >= 0)
							s.SetCount(n + len);
						close(fd);
						return static_cast<Txt>(s);
					}
				}
			}
			return Txt::GetVoid();
		}
	#endif
		FileIn in(filename);
		return LoadStream(in);
	}
	return Txt::GetVoid();
}

bool SaveStream(Stream& out, const Txt& data) {
	if(!out.IsOpen() || out.IsError()) return false;
	out.Put((tukk )data, data.GetLength());
	out.Close();
	return out.IsOK();
}

bool SaveFile(tukk filename, const Txt& data) {
	FileOut out(filename);
	return SaveStream(out, data);
}

i64 CopyStream(Stream& dest, Stream& src, i64 count)
{
	return CopyStream(dest, src, count, Null);
}

i64 CopyStream(Stream& dest, Stream& src, i64 count, Gate<i64, i64> progress, i32 chunk_size)
{
	i32 block = (i32)min<i64>(count, chunk_size);
	Buffer<u8> temp(block);
	i32 loaded;
	i64 done = 0;
	i64 total = count;
	while(count > 0 && (loaded = src.Get(~temp, (i32)min<i64>(count, block))) > 0) {
		dest.Put(~temp, loaded);
		if(dest.IsError())
			return -1;
		count -= loaded;
		done += loaded;
		if(progress(done, total))
			return -1;
	}
	return done;
}

void CheckedSerialize(const Event<Stream&> serialize, Stream& stream, i32 version)
{
	i32 pos = (i32)stream.GetPos();
	stream.Magic(0x61746164);
	if(!IsNull(version))
		stream.Magic(version);
	serialize(stream);
	stream.Magic(0x00646e65);
	pos = i32(stream.GetPos() - pos);
	stream.Magic(pos);
}

bool Load(Event<Stream&> serialize, Stream& stream, i32 version) {
	TxtStream backup;
	backup.SetStoring();
	serialize(backup);
	ASSERT(!backup.IsError());
	stream.SetLoading();
	stream.LoadThrowing();
	try {
		CheckedSerialize(serialize, stream, version);
	}
	catch(LoadingError) {
		backup.Seek(0);
		backup.SetLoading();
		serialize(backup);
		ASSERT(!backup.IsError());
		return false;
	}
	catch(ValTypeError) {
		backup.Seek(0);
		backup.SetLoading();
		serialize(backup);
		ASSERT(!backup.IsError());
		return false;
	}
	return true;
}

bool Store(Event<Stream&> serialize, Stream& stream, i32 version) {
	stream.SetStoring();
	CheckedSerialize(serialize, stream, version);
	return !stream.IsError();
}

Txt Cfgname(tukk file) {
	return file ? Txt(file) : ConfigFile();
}

bool LoadFromFile(Event<Stream&> serialize, tukk file, i32 version) {
	FileIn f(Cfgname(file));
	return f ? Load(serialize, f, version) : false;
}

bool StoreToFile(Event<Stream&> serialize, tukk file, i32 version) {
	FileOut f(Cfgname(file));
	if(!f || !Store(serialize, f, version))
		return false;
	f.Close();
	return !f.IsError();
}

Stream& Pack16(Stream& s, i32& i) {
	if(s.IsLoading()) {
		i = (i16) s.Get16le();
		if(i == -32768)
			i = s.Get32le();
	}
	else
		if(i < -32767 || i > 32767) {
			s.Put16le((word)-32768);
			s.Put32le(i);
		}
		else
			s.Put16le((word)i);
	return s;
}

Stream& Pack16(Stream& s, i32& i1, i32& i2) {
	Pack16(s, i1);
	Pack16(s, i2);
	return s;
}

Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3) {
	Pack16(s, i1, i2);
	Pack16(s, i3);
	return s;
}

Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3, i32& i4) {
	Pack16(s, i1, i2, i3);
	Pack16(s, i4);
	return s;
}

Stream& Pack16(Stream& s, i32& i1, i32& i2, i32& i3, i32& i4, i32& i5) {
	Pack16(s, i1, i2, i3, i4);
	Pack16(s, i5);
	return s;
}

i32 StreamHeading(Stream& stream, i32 ver, i32 minver, i32 maxver, tukk tag)
{
	if(stream.IsLoading() && stream.IsEof() || stream.IsError())
		return Null;
	Txt text = tag;
	dword len = text.GetLength();
	stream.Pack(len);
	if(stream.IsLoading()) {
		if(stream.IsError() || (i32)len != text.GetLength()) {
			stream.SetError();
			return Null;
		}
		TxtBuf b(len);
		stream.SerializeRaw((u8 *)~b, len);
		Txt in = b;
		if(stream.IsError() || in != text) {
			stream.SetError();
			return Null;
		}
	}
	else
		stream.SerializeRaw((u8 *)(tukk)text, len);
	stream / ver;
	if(ver < minver || ver > maxver) {
		stream.SetError();
		return Null;
	}
	return ver;
}

}