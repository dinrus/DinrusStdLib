#include <drx/Core/Core.h>

namespace drx {
	
// we simply store data as series of complete Zstd frames, as library gives us no easy way
// to do it in MT

void ZstdCompressStream::Open(Stream& out_, i32 level_)
{
	out = &out_;
	level = level_;
	ClearError();
	pos = 0;
	Alloc();
}

void ZstdCompressStream::Alloc()
{
	i32 N = 16;
	i32 sz = concurrent ? N * BLOCK_BYTES : BLOCK_BYTES;
	buffer.Alloc(sz);
	outbuf.Alloc(N * ZSTD_compressBound(BLOCK_BYTES));
	outsz.Alloc(N);
	Stream::buffer = ~buffer;
	wrlim = ~buffer + sz;
	ptr = ~buffer;
}

void ZstdCompressStream::Co(bool b)
{
	FlushOut();
	concurrent = b;
	Alloc();
}

void ZstdCompressStream::FlushOut()
{
	if(ptr == (u8*)~buffer)
		return;
	
	CoWork co;
	
	i32 osz = (i32)ZSTD_compressBound(BLOCK_BYTES);
	u8* t = ~outbuf;
	i32   ii = 0;
	for(u8* s = ~buffer; s < ptr; s += BLOCK_BYTES) {
		i32 origsize = min((i32)BLOCK_BYTES, i32(ptr - s));
		if(concurrent)
			co & [=, this] {
				outsz[ii] = (i32)ZSTD_compress(t, osz, s, origsize, level);
			};
		else
			outsz[ii] = (i32)ZSTD_compress(t, osz, s, origsize, level);
		ii++;
		t += osz;
	}
	
	if(concurrent)
		co.Finish();
	
	t = ~outbuf;
	for(i32 i = 0; i < ii; i++) {
		i32 clen = outsz[i];
		if(clen < 0) {
			SetError();
			return;
		}
		out->Put(t, clen);
		t += osz;
	}
	
	i32 origsize = i32(ptr - ~buffer);
	pos += origsize;
	ptr = ~buffer;
}

void ZstdCompressStream::Close()
{
	if(out) {
		FlushOut();
		out = NULL;
	}
}

bool ZstdCompressStream::IsOpen() const
{
	return out && out->IsOpen();
}

void ZstdCompressStream::_Put(i32 w)
{
	FlushOut();
	*ptr++ = w;
}

void ZstdCompressStream::_Put(ukk data, u32 size)
{
	tukk s = reinterpret_cast<tukk>(data);

	while(size > 0) {
		if(IsError() || (out && out->IsError()))
			return;
		u32 n = u32(wrlim - ptr);
		if(size >= n) {
			memcpy(ptr, s, n);
			ptr = wrlim;
			FlushOut();
			size -= n;
			s += n;
		}
		else {
			memcpy(ptr, s, size);
			ptr += size;
			break;
		}
	}
}

ZstdCompressStream::ZstdCompressStream()
{
	style = STRM_WRITE;
	concurrent = false;
	out = NULL;
}

ZstdCompressStream::~ZstdCompressStream()
{
	Close();
}

};
