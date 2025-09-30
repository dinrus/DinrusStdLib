
namespace drx {

void LZ4CompressStream::Open(Stream& out_)
{
	out = &out_;
	ClearError();
	pos = 0;
	xxh.Reset();
	Alloc();
	pos = 0;
	u8 h[7];
	Poke32le(h, LZ4F_MAGIC);
	h[4] = LZ4F_VERSION | LZ4F_BLOCKINDEPENDENCE | LZ4F_CONTENTCHECKSUM;
	h[5] = LZ4F_MAXSIZE_1024KB;
	h[6] = xxHash(h + 4, 2) >> 8;
	out->Put(h, 7);
}

void LZ4CompressStream::Alloc()
{
	i32 N = 16;
	i32 sz = concurrent ? N * BLOCK_BYTES : BLOCK_BYTES;
	buffer.Alloc(sz);
	outbuf.Alloc(N * LZ4_compressBound(BLOCK_BYTES));
	outsz.Alloc(N);
	Stream::buffer = ~buffer;
	wrlim = ~buffer + sz;
	ptr = ~buffer;
}

void LZ4CompressStream::Co(bool b)
{
	FlushOut();
	concurrent = b;
	Alloc();
}

void LZ4CompressStream::FlushOut()
{
	if(ptr == (u8*)~buffer)
		return;
	
	CoWork co;
	
	i32 osz = LZ4_compressBound(BLOCK_BYTES);
	u8* t = ~outbuf;
	i32   ii = 0;
	for(u8* s = ~buffer; s < ptr; s += BLOCK_BYTES) {
		i32 origsize = min((i32)BLOCK_BYTES, i32(ptr - s));
		if(concurrent)
			co & [=, this] {
				outsz[ii] = LZ4_compress_default((tuk)s, (tuk)t, origsize, osz);
			};
		else
			outsz[ii] = LZ4_compress_default((tuk)s, (tuk)t, origsize, osz);
		ii++;
		t += osz;
	}
	
	if(concurrent)
		co.Finish();
	
	u8* s = ~buffer;
	t = ~outbuf;
	for(i32 i = 0; i < ii; i++) {
		i32 origsize = min((i32)BLOCK_BYTES, i32(ptr - s));
		i32 clen = outsz[i];
		if(clen < 0) {
			SetError();
			return;
		}
		if(clen >= origsize || clen == 0) {
			out->Put32le(0x80000000 | origsize);
			out->Put(s, origsize);
		}
		else {
			out->Put32le(clen);
			out->Put(t, clen);
		}
		s += BLOCK_BYTES;
		t += osz;
	}
	
	i32 origsize = i32(ptr - ~buffer);
	xxh.Put(~buffer, origsize);
	pos += origsize;
	ptr = ~buffer;
}

void LZ4CompressStream::Close()
{
	if(out) {
		FlushOut();
		out->Put32le(0);
		out->Put32le(xxh.Finish());
		out = NULL;
	}
}

bool LZ4CompressStream::IsOpen() const
{
	return out && out->IsOpen();
}

void LZ4CompressStream::_Put(i32 w)
{
	FlushOut();
	*ptr++ = w;
}

void LZ4CompressStream::_Put(ukk data, u32 size)
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

LZ4CompressStream::LZ4CompressStream()
{
	style = STRM_WRITE;
	concurrent = false;
	out = NULL;
}

LZ4CompressStream::~LZ4CompressStream()
{
	Close();
}

};
