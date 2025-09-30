#include "../Core.h"

namespace drx {

Huge::Huge()
{
	size = 0;
}

u8 *Huge::AddChunk()
{
	size += CHUNK;
	return data.Add().data;
}

void Huge::Finish(i32 last_chunk_size)
{
	size = size - CHUNK + last_chunk_size;
}

void Huge::Get(uk t_, size_t pos, size_t sz) const
{
	ASSERT(pos + sz <= size);
	i32 blki = i32(pos / CHUNK);
	size_t blkpos = pos & (CHUNK - 1);
	u8 *t = (u8 *)t_;
	
	while(sz > 0) {
		size_t m = min(sz, CHUNK - blkpos);
		memcpy8(t, data[blki].data + blkpos, m);
		t += m;
		sz -= m;
		blkpos = 0;
		blki++;
	}
}

Txt Huge::Get() const
{
	if(size >= INT_MAX)
		Panic("Txt is too big!");
	TxtBuf sb((i32)size);
	Get(~sb, 0, size);
	return Txt(sb);
}

}