//#include "../Compress.h"

#define LLOG(x) // LOG(x)

namespace drx {

void ZstdDecompressStream::Init()
{
	for(i32 i = 0; i < 16; i++)
		wb[i].Clear();
	ii = 0;
	count = 0;
	dlen = 0;
	pos = 0;
	eof = false;
	static u8 h;
	ptr = rdlim = buffer = &h;
	compressed_data.Clear();
	compressed_at = 0;
	ClearError();
}

bool ZstdDecompressStream::Open(Stream& in_)
{
	Init();
	in = &in_;
	return true;
}

bool ZstdDecompressStream::Next()
{
	pos += ptr - buffer;
	ptr = rdlim = buffer;
	if(ii < count) {
		const Workblock& w = wb[ii++];
		ptr = (u8*)~w.decompressed_data;
		rdlim = ptr + w.decompressed_sz;
		Stream::buffer = ptr;
		return true;
	}
	return false;
}

void ZstdDecompressStream::Fetch()
{
	if(Next())
		return;
	if(eof)
		return;
	CoWork co;
	bool   error = false;
	ii = 0;
	count = concurrent ? 16 : 1;
	for(i32 i = 0; i < count; i++) {
		Workblock& w = wb[i];
		
		size_t frameSize;
		for(;;) {
			i32 sz = compressed_data.GetCount() - compressed_at;
			tukk at = ~compressed_data + compressed_at;
			frameSize = ZSTD_findFrameCompressedSize(at, sz);
			if(!ZSTD_isError(frameSize))
				break;
			// need to read more compressed data
			if(in->IsEof()) {
				if(compressed_data.GetCount() != compressed_at) {
					SetError();
					return;
				}

				eof = true;
				count = i;
				goto eof;
			}
			TxtBuf b(sz + count * BLOCK_BYTES);
			memcpy(~b, at, sz);
			b.SetCount(sz + in->Get(~b + sz, count * BLOCK_BYTES));
			compressed_data = b;
			compressed_at = 0;
		}
		
		if(frameSize > 1024*1024*1024) {
			SetError();
			return;
		}
		
		w.compressed_data = compressed_data;
		w.frame_at = compressed_at;
		w.frame_sz = (i32)frameSize;
		
		compressed_at += w.frame_sz;
		
		u64 sz = ZSTD_getFrameContentSize(w.FramePtr(), w.frame_sz);
		if(sz == ZSTD_CONTENTSIZE_ERROR || sz > 1024*1024*1024) {
			SetError();
			return;
		}
		
		w.decompressed_sz = (i32)sz;
		
		if(w.decompressed_sz > BLOCK_BYTES) {
			w.decompressed_data.Alloc(w.decompressed_sz);
			w.irregular_d = true;
		}
		else
		if(!w.decompressed_data || w.irregular_d) {
			w.decompressed_data.Alloc(BLOCK_BYTES);
			w.irregular_d = false;
		}

		auto decompress = [=, this] {
			Workblock& w = wb[i];
			if(ZSTD_isError(ZSTD_decompress(~w.decompressed_data, w.decompressed_sz, w.FramePtr(), w.frame_sz))) {
				SetError();
				return;
			}
		};
		
		if(concurrent)
			co & decompress;
		else
			decompress();
	}
eof:
	if(concurrent)
		co.Finish();
	if(error)
		SetError();
	else
		Next();
}

bool ZstdDecompressStream::IsOpen() const
{
	return in->IsOpen() && !IsError();
}

i32 ZstdDecompressStream::_Term()
{
	if(Ended())
		return -1;
	Fetch();
	return ptr == rdlim ? -1 : *ptr;
}

i32 ZstdDecompressStream::_Get()
{
	if(Ended())
		return -1;
	Fetch();
	return ptr == rdlim ? -1 : *ptr++;
}

u32 ZstdDecompressStream::_Get(uk data, u32 size)
{
	u8* t = (u8*)data;
	while(size) {
		if(Ended())
			break;
		u32 n = u32(rdlim - ptr);
		if(size < n) {
			memcpy(t, ptr, size);
			t += size;
			ptr += size;
			break;
		}
		else {
			memcpy(t, ptr, n);
			t += n;
			size -= n;
			ptr = rdlim;
			Fetch();
		}
	}
	
	return u32(t - (u8*)data);
}

ZstdDecompressStream::ZstdDecompressStream()
{
	style = STRM_READ|STRM_LOADING;
	in = NULL;
	concurrent = false;
}

ZstdDecompressStream::~ZstdDecompressStream()
{
}

bool IsZstd(Stream& s)
{
	i64 pos = s.GetPos();
	bool b = (u32)s.Get32le() == 0xFD2FB528;
	s.Seek(pos);
	return b;
}

};
