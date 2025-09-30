#include <drx/Core/Core.h>

namespace drx {

i32 IndexCommon::empty[1] = { -1 };

IndexCommon::IndexCommon()
{
	hash = NULL;
	map = empty;
	mask = 0;
	unlinked = -1;
}

void IndexCommon::Pick(IndexCommon& b)
{
	Free();

	hash = b.hash;
	map = b.map;
	mask = b.mask;
	unlinked = b.unlinked;
	
	b.hash = NULL;
	b.map = empty;
	b.mask = 0;
	b.unlinked = -1;
}

void IndexCommon::Copy(const IndexCommon& b, i32 count)
{
	memcpy_t(hash, b.hash, count);
	mask = b.mask;
	unlinked = b.unlinked;

	FreeMap();
	map = (i32 *)MemoryAlloc((mask + 1) * sizeof(i32));
	memcpy_t(map, b.map, mask + 1);
}

void IndexCommon::Swap(IndexCommon& b)
{
	DRX::Swap(hash, b.hash);
	DRX::Swap(map, b.map);
	DRX::Swap(mask, b.mask);
	DRX::Swap(unlinked, b.unlinked);
}

IndexCommon::~IndexCommon()
{
	Free();
}

void IndexCommon::FreeMap()
{
	if(map != empty)
		MemoryFree(map);
}

void IndexCommon::Free()
{
	if(hash)
		MemoryFree(hash);
	FreeMap();
}

void IndexCommon::Remap(i32 count)
{
	Fill(map, map + mask + 1, -1);
	for(i32 i = 0; i < count; i++) // todo: unlinked
		if(hash[i].hash)
			Link(i, hash[i].hash);
}

void IndexCommon::Reindex(i32 count)
{
	FreeMap();
	map = (i32 *)MemoryAlloc((mask + 1) * sizeof(i32));
	Remap(count);
}

void IndexCommon::Clear()
{
	Free();
	hash = NULL;
	map = empty;
	mask = 0;
	unlinked = -1;
}

void IndexCommon::GrowMap(i32 count)
{
	mask = (mask << 1) | 3;
	Reindex(count);
}

Vec<i32> IndexCommon::GetUnlinked() const
{
	Vec<i32> r;
	i32 i = unlinked;
	if(i >= 0) {
		do {
			i = hash[i].prev;
			r.Add(i);
		}
		while(i != unlinked);
	}
	return r;
}

void IndexCommon::AdjustMap(i32 count, i32 alloc)
{
	if(alloc == 0) {
		FreeMap();
		map = empty;
		mask = 0;
		return;
	}
	dword msk = 0;
	while(msk < (dword)alloc)
		msk = (msk << 1) | 3;
	if(msk != mask) {
		mask = msk;
		Reindex(count);
	}
}

void IndexCommon::MakeMap(i32 count)
{
	mask = 0;
	AdjustMap(count, count);
}

void IndexCommon::Trim(i32 n, i32 count)
{
	if(n == 0) {
		i32 n = (i32)(mask + 1);
		for(i32 i = 0; i < n; i++)
			map[i] = -1;
		unlinked = -1;
		return;
	}
	
	for(i32 i = n; i < count; i++) { // remove items in trimmed area from buckets / unlinked
		Hash& hh = hash[i];
		if(hh.hash)
			Del(map[hh.hash & mask], hh, i);
		else
			Del(unlinked, hh, i);
	}
}

void IndexCommon::Sweep(i32 n)
{
	i32 ti = 0;
	for(i32 i = 0; i < n; i++)
		if(hash[i].hash)
			hash[ti++].hash = hash[i].hash;
	Remap(ti);
	unlinked = -1;
}

}