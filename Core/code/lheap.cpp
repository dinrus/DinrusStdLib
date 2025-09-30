#include "../Core.h"

#define LTIMING(x)    // RTIMING(x)
#define LHITCOUNT(x)  // RHITCOUNT(x)
#define LLOG(x)       //  LOG((uk )this << ' ' << x)

namespace drx {

#ifdef DRX_HEAP

#include "../HeapImp.h"

i32 Heap::lclass[] = { 0, 4, 5, 6, 7, 8, 9, 11, 13, 15, 18, 22, 27, 33, 40, 49, 60, 73, 89, 109, 134, 164, 201, 225, 255 };
i32 Heap::free_lclass[LPAGE + 1]; // free block size -> lclass, size is >= class sz
i32 Heap::alloc_lclass[LPAGE + 1]; // allocation size -> lclass, size <= class sz


void Heap::LargeHeapDetail::LinkFree(BlkHeader_<LUNIT> *h)
{
	Dbl_LinkAfter(h, freelist[free_lclass[h->GetSize()]]);
}

void Heap::LInit()
{
	ASSERT(__countof(lheap.freelist) == __countof(lclass));
	ONCELOCK {
		i32 ai = 0;
		i32 fi = 0;
		for(i32 i = 0; i <= 255; i++) {
			if(i > lclass[ai])
				ai++;
			if(i >= lclass[fi + 1])
				fi++;
			alloc_lclass[i] = ai;
			free_lclass[i] = fi;
		}
	}
	for(i32 i = 0; i <= __countof(lheap.freelist); i++)
		Dbl_Self(lheap.freelist[i]);
	big->LinkSelf();
}

uk Heap::TryLAlloc(i32 i0, word wcount)
{
	LTIMING("TryLAlloc");
	for(i32 i = i0; i < __countof(lheap.freelist); i++) {
		LBlkHeader *l = lheap.freelist[i];
		LBlkHeader *h = l->next;
		if(h != l) {
			ASSERT(h->GetSize() >= wcount);
			if(h->GetSize() == LPAGE && this != &aux) {
				free_lpages--;
				ASSERT(free_lpages >= 0);
			}
			lheap.MakeAlloc(h, wcount);
			h->heap = this;
			return (BlkPrefix *)h + 1;
		}
		LHITCOUNT("TryLAlloc 2");
	}
	return NULL;
}

#ifdef LSTAT
i32 stat[65536];

EXITBLOCK {
	i32 cnt = 0;
	for(i32 i = 0; i < 65536; i++) {
		cnt += stat[i];
		if(stat[i])
			RLOG(i * 256 << ": " << stat[i] << " / " << cnt);
	}
}
#endif

uk Heap::LAlloc(size_t& size)
{
	if(!initialized)
		Init();

	if(size > LUNIT * LPAGE - sizeof(BlkPrefix)) { // big block allocation
		LTIMING("Big alloc");
		Mutex::Lock __(mutex);
		size_t count = (size + sizeof(DLink) + sizeof(BlkPrefix) + 4095) >> 12;
		DLink *d = (DLink *)HugeAlloc(count);
		d->Link(big);
		d->size = size = (count << 12) - sizeof(DLink) - sizeof(BlkPrefix);
		BlkPrefix *h = (BlkPrefix *)(d + 1);
		h->heap = NULL; // mark this as huge block
		big_size += size;
		big_count++;
		LLOG("Big alloc " << size << ": " << h + 1);
		return h + 1;
	}

	word wcount = word((size + sizeof(BlkPrefix) + LUNIT - 1) >> 8);

#ifdef LSTAT
	stat[wcount]++;
#endif

	LTIMING("LAlloc");

	size = ((i32)wcount * LUNIT) - sizeof(BlkPrefix);
	i32 i0 = alloc_lclass[wcount];

	if(large_remote_list)  // there might be blocks of this heap freed in other threads
		LargeFreeRemote(); // free them first

	uk ptr = TryLAlloc(i0, wcount);
	if(ptr)
		return ptr;

	Mutex::Lock __(mutex);
	aux.LargeFreeRemoteRaw();
	ptr = aux.TryLAlloc(i0, wcount);
	if(ptr) { // found in aux, we need to move large page from aux to this heap
		LLOG("Found in aux");
		BlkPrefix *h = (BlkPrefix *)ptr - 1;
		while(!h->IsFirst()) // find the start of large page to get page header
			h = h->GetPrevHeader(LUNIT);
		MoveLargeTo((DLink *)((u8 *)h - LOFFSET), this);
		return ptr;
	}

	LTIMING("Large More");
	DLink *ml = (DLink *)HugeAlloc(((LPAGE + 1) * LUNIT) / 4096);
	ml->Link(large);
	LBlkHeader *h = ml->GetFirst();
	lheap.AddChunk(h, LPAGE);
	lheap.MakeAlloc(h, wcount);
	h->heap = this;
	return (BlkPrefix *)h + 1;
}

void Heap::FreeLargePage(DLink *l)
{
	LLOG("Moving empty large " << (uk )l << " to global storage, lcount " << lcount);
	l->Unlink();
	Mutex::Lock __(mutex);
	HugeFree(l);
}

void Heap::LFree(uk ptr)
{
	BlkPrefix *h = (BlkPrefix *)ptr - 1;

	if(h->heap == this) {
		LTIMING("Large Free");
		LBlkHeader *fh = lheap.Free((LBlkHeader *)h);
		if(fh->GetSize() == LPAGE) {
			if(free_lpages >= max_free_lpages || this == &aux) {
				LTIMING("FreeLargePage");
				fh->UnlinkFree();
				FreeLargePage((DLink *)((u8 *)fh - LOFFSET));
			}
			else
				free_lpages++;
		}
		return;
	}

	Mutex::Lock __(mutex);
	if(h->heap == NULL) { // this is big block
		LTIMING("Big Free");
		DLink *d = (DLink *)h - 1;
		big_size -= d->size;
		big_count--;
		d->Unlink();
		LLOG("Big free " << (uk ) ptr << " size " << h->size);
		HugeFree(d);
		return;
	}

	LTIMING("Remote Free");
	// this is remote heap
	FreeLink *f = (FreeLink *)ptr;
	f->next = h->heap->large_remote_list;
	h->heap->large_remote_list = f;
}

bool   Heap::TryRealloc(uk ptr, size_t& newsize)
{
	LTIMING("TryRealloc");
	ASSERT(ptr);

#ifdef _DEBUG
	if(IsSmall(ptr))
		return false;
#endif

	BlkPrefix *h = (BlkPrefix *)ptr - 1;

	if(h->heap == this) {
		if(newsize > LUNIT * LPAGE - sizeof(BlkPrefix))
			return false;
		word wcount = word(((newsize ? newsize : 1) + sizeof(BlkPrefix) + LUNIT - 1) >> 8);
		size_t dummy = 0;
		if(wcount == h->GetSize() || lheap.TryRealloc(h, wcount, dummy)) {
			newsize = ((i32)wcount * LUNIT) - sizeof(BlkPrefix);
			LHITCOUNT("Large realloc true");
			return true;
		}
	}

	Mutex::Lock __(mutex);
	if(h->heap == NULL) { // this is big block
		LTIMING("Big realloc");

		DLink *d = (DLink *)h - 1;

		size_t count = (newsize + sizeof(DLink) + sizeof(BlkPrefix) + 4095) >> 12;

		if(HugeTryRealloc(d, count)) {
			big_size -= d->size;
			d->size = newsize = (count << 12) - sizeof(DLink) - sizeof(BlkPrefix);
			big_size += d->size;
			return true;
		}
	}

	// СДЕЛАТЬ: When small block fits, we could still return true

	return false;
}

size_t Heap::LGetBlockSize(uk ptr) {
	BlkPrefix *h = (BlkPrefix *)ptr - 1;

	if(h->heap == NULL) { // huge block
		Mutex::Lock __(mutex);
		DLink *hh = (DLink *)h - 1;
		return hh->size;
	}

	return ((i32)h->GetSize() * LUNIT) - sizeof(BlkPrefix);
}

#endif

}