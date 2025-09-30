#include "../Core.h"

#define LLOG(x)

namespace drx {

z64 NewInVectorSerial()
{
	static z64 x;
	INTERLOCKED {
		++x;
	}
	return x;
}


thread_local z64 invector_cache_serial_;
thread_local i32   invector_cache_blki_;
thread_local i32   invector_cache_offset_;
thread_local i32   invector_cache_end_;

void SetInvectorCache__(z64 serial, i32 blki, i32 offset, i32 end)
{
	invector_cache_serial_ = serial;
	invector_cache_blki_ = blki;
	invector_cache_offset_ = offset;
	invector_cache_end_ = end;
}

void ClearInvectorCache__()
{
	invector_cache_serial_ = 0;
}

i32 FindInvectorCache__(z64 serial, i32& pos, i32& off)
{
	if(invector_cache_serial_ == serial && pos >= invector_cache_offset_ &&
	   pos < invector_cache_end_) {
		LLOG("Found in cache, serial: " << invector_cache_serial_ << ", offset: " << invector_cache_offset_ << ", end: " << invector_cache_end_);
		off = invector_cache_offset_;
		pos -= off;
		return invector_cache_blki_;
	}
	return -1;
}

void Bits::Clear()
{
	if(bp)
		MemoryFree(bp);
	alloc = 0;
	bp = NULL;
}

void Bits::Realloc(i32 nalloc)
{
	size_t sz = sizeof(dword) * nalloc;
	dword *nbp = (dword *)MemoryAllocSz(sz);
	nalloc = i32(sz / sizeof(dword));
	if(bp) {
		Copy(nbp, bp, bp + min(alloc, nalloc));
		MemoryFree(bp);
	}
	if(nalloc > alloc)
		Fill(nbp + alloc, nbp + nalloc, (dword)0);
	bp = nbp;
	alloc = nalloc;
}

void Bits::Expand(i32 q)
{
	Realloc(3 * q / 2 + 1);
}

void Bits::Reserve(i32 nbits)
{
	i32 n = (nbits + 31) >> 5;
	if(n > alloc)
		Realloc(n);
}

i32 Bits::GetLast() const
{
	i32 lasti = alloc - 1;
	while(lasti > 0 && bp[lasti] == 0)
		lasti--;
	return lasti;
}

void Bits::Shrink()
{
	i32 nalloc = GetLast() + 1;
	if(nalloc != alloc)
		Realloc(nalloc);
}

dword *Bits::CreateRaw(i32 n_dwords)
{

	Clear();
	Realloc(n_dwords);
	return bp;
}

Txt Bits::ToTxt() const
{
	TxtBuf ss;
	for(i32 i = GetLast(); i >= 0; i--)
		ss << FormatIntHex(bp[i]);
	return Txt(ss);
}

void Bits::Serialize(Stream& s)
{
	i32 dwords;
	if(s.IsStoring())
		dwords = GetLast() + 1;
	s % dwords;
	if(dwords < 0)
		s.LoadError();
	if(s.IsLoading())
		CreateRaw(dwords);
	s.SerializeRaw(bp, dwords);
}

}