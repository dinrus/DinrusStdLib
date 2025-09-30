void  OutOfMemoryPanic();

uk SysAllocRaw(size_t size, size_t reqsize);
void  SysFreeRaw(uk ptr, size_t size);

tukk asTxt(i32 i);
tukk asTxt(uk ptr);

struct Heap;

// Используется U++ внутренне для управления large (64 КБ) и huge (до 256 МБ) блоками

struct BlkPrefix { // this part is at the start of Blk allocated block, client must not touch it
	word        prev_size;
	word        size;
	bool        free;
	bool        last;
	Heap       *heap; // we need this for 4KB pages and large blocks, NULL for Huge blocks
#ifdef CPU_32
	dword       filler;
#endif

	void  SetSize(word sz)           { size = sz; }
	void  SetPrevSize(word sz)       { prev_size = sz; }
	void  SetFree(bool b)            { free = b; }
	void  SetLast(bool b)            { last = b; }

	word  GetSize()                  { return size; }
	word  GetPrevSize()              { return prev_size; }
	bool  IsFirst()                  { return GetPrevSize() == 0; }
	bool  IsFree()                   { return free; }
	bool  IsLast()                   { return last; }

	BlkPrefix *GetPrevHeader(i32 BlkSize) { return (BlkPrefix *)((u8 *)this - BlkSize * GetPrevSize()); }
	BlkPrefix *GetNextHeader(i32 BlkSize) { return (BlkPrefix *)((u8 *)this + BlkSize * GetSize()); }
};

template <i32 BlkSize>
struct BlkHeader_ : BlkPrefix { // free block record
	BlkHeader_ *prev; // linked list of free blocks
	BlkHeader_ *next; // linked list of free blocks

	BlkHeader_ *GetPrevHeader()      { return (BlkHeader_ *)BlkPrefix::GetPrevHeader(BlkSize); }
	BlkHeader_ *GetNextHeader()      { return (BlkHeader_ *)BlkPrefix::GetNextHeader(BlkSize); }

	void  SetNextPrevSz()            { if(!IsLast()) GetNextHeader()->SetPrevSize(GetSize()); }

	void  UnlinkFree()               { Dbl_Unlink(this); }
};

template <typename Detail, i32 BlkSize>
struct BlkHeap : Detail {
	typedef BlkHeader_<BlkSize> BlkHeader;
	typedef Detail D;

	bool       JoinNext(BlkHeader *h, word needs_count = 0);
	void       Split(BlkHeader *h, word wcount, bool fill = false);
	void       AddChunk(BlkHeader *h, i32 count);
	void      *MakeAlloc(BlkHeader *h, word wcount);
	BlkHeader *Free(BlkHeader *h); // returns final joined block
	bool       TryRealloc(uk ptr, size_t count, size_t& n);
	void       BlkCheck(uk page, i32 size, bool check_heap = false);

	static void  Assert(bool b);
#ifdef HEAPDBG
	static void  DbgFreeFill(uk ptr, size_t size);
	static void  DbgFreeCheck(uk ptr, size_t size);
	static void  FillFree(BlkHeader *h);
	static void  CheckFree(BlkHeader *h);
#else
	static void  DbgFreeFill(uk ptr, size_t size) {}
	static void  DbgFreeCheck(uk ptr, size_t size) {}
	static void  FillFree(BlkHeader *h) {}
	static void  CheckFree(BlkHeader *h) {}
#endif
};

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::Assert(bool b)
{
	if(!b)
		Panic("Куча повреждена!");
}

#ifdef HEAPDBG

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::DbgFreeFill(uk p, size_t size)
{
	size_t count = size >> 2;
	dword *ptr = (dword *)p;
	while(count--)
		*ptr++ = 0x65657246;
}

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::DbgFreeCheck(uk p, size_t size)
{
	size_t count = size >> 2;
	dword *ptr = (dword *)p;
	while(count--)
		if(*ptr++ != 0x65657246)
			Panic("Детектируются записи в освобождённые блоки");
}

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::FillFree(BlkHeader *h)
{
	if(BlkSize == 4096) // it is too slow to check huge blocks
		return;
	DbgFreeFill(h + 1, h->GetSize() * BlkSize - sizeof(BlkHeader));
}

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::CheckFree(BlkHeader *h)
{
	if(BlkSize == 4096) // слишком медленно для проверки блоков huge
		return;
	DbgFreeCheck(h + 1, h->GetSize() * BlkSize - sizeof(BlkHeader));
}

#endif

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::BlkCheck(uk page, i32 page_size, bool check_heap)
{
	#define CLOG(x) // LOG(x)
	CLOG("=== " << asTxt(page_size) << " " << AsTxt(page));
	BlkPrefix *h = (BlkPrefix *)page;
	i32 size = 0;
	i32 psize = 0;
	Assert(h->IsFirst());
	for(;;) {
		size += h->GetSize();
		CLOG("h: " << AsTxt(h) << ", GetSize: " << asTxt(h->GetSize())
		     << ", size: " << asTxt(size) << ", islast: " << asTxt(h->IsLast()));
		Assert(h->GetSize());
		Assert(h->GetPrevSize() == psize);
		Assert(!check_heap || h->IsFree() || h->heap);
		if(h->IsFree())
			CheckFree((BlkHeader *)h);
		psize = h->GetSize();
		if(h->IsLast() && size == page_size)
			return;
		Assert(size < page_size);
		h = h->GetNextHeader(BlkSize);
	}
	#undef CLOG
}

template <typename Detail, i32 BlkSize>
force_inline
bool BlkHeap<Detail, BlkSize>::JoinNext(BlkHeader *h, word needs_count)
{ // try to join with next header if it is free, does not link it to free
	if(h->IsLast())
		return false;
	BlkHeader *nh = h->GetNextHeader();
	if(!nh->IsFree() || h->GetSize() + nh->GetSize() < needs_count)
		return false;
	CheckFree(nh);
	h->SetLast(nh->IsLast());
	nh->UnlinkFree();
	word nsz = h->GetSize() + nh->GetSize();
	h->SetSize(nsz);
	h->SetNextPrevSz();
	return true;
}

template <typename Detail, i32 BlkSize>
force_inline
void BlkHeap<Detail, BlkSize>::Split(BlkHeader *h, word wcount, bool fill)
{ // splits the block if bigger, links new block to free
	ASSERT(BlkSize != 4096 || ((dword)(uintptr_t)h & 4095) == 0);
	BlkHeader *h2 = (BlkHeader *)((u8 *)h + BlkSize * (i32)wcount);
	word nsz = h->GetSize() - wcount;
	if(nsz == 0) // nothing to split
		return;

	h2->SetFree(true);
	h2->SetLast(h->IsLast());
	h2->SetSize(nsz);
	h2->SetPrevSize(wcount);
	h2->SetNextPrevSz();
	D::LinkFree(h2);
	if(fill)
		FillFree(h2);

	h->SetSize(wcount);
	h->SetLast(false);
}

template <typename Detail, i32 BlkSize>
void BlkHeap<Detail, BlkSize>::AddChunk(BlkHeader *h, i32 count)
{
	h->SetSize(count);
	h->SetPrevSize(0); // is first
	h->SetLast(true);
	h->SetFree(true);
	D::LinkFree(h);
	FillFree(h);
}

template <typename Detail, i32 BlkSize>
force_inline
uk BlkHeap<Detail, BlkSize>::MakeAlloc(BlkHeader *h, word wcount)
{
	h->UnlinkFree();
	h->SetFree(false);
	Split(h, wcount);
	CheckFree(h);
	return h;
}

template <typename Detail, i32 BlkSize>
bool BlkHeap<Detail, BlkSize>::TryRealloc(uk ptr, size_t count, size_t& n)
{
	ASSERT(count);

	BlkHeader *h = (BlkHeader *)ptr;
	if(h->size == 0)
		return false;

	word sz = h->GetSize();
	if(sz != count) {
		if(!JoinNext(h, (word)count) && count > sz)
			return false;
		Split(h, (word)count, true);
		n = n - sz + count;
	}
	return true;
}

template <typename Detail, i32 BlkSize>
force_inline
typename BlkHeap<Detail, BlkSize>::BlkHeader *BlkHeap<Detail, BlkSize>::Free(BlkHeader *h)
{
	ASSERT(BlkSize != 4096 || ((dword)(uintptr_t)h & 4095) == 0);
	JoinNext(h);
	if(!h->IsFirst()) { // try to join with previous header if it is free
		BlkHeader *ph = h->GetPrevHeader();
		if(ph->IsFree()) {
			ph->UnlinkFree(); // remove because size will change, might end in another bucket then
			word nsz = ph->GetSize() + h->GetSize();
			ph->SetSize(nsz);
			ph->SetLast(h->IsLast());
			ph->SetNextPrevSz();
			h = ph;
		}
	}
	h->SetFree(true);
	D::LinkFree(h); // was not joined with previous header
	FillFree(h);
	return h;
}

struct HugeHeapDetail {
	static BlkHeader_<4096> freelist[20][1];

	static i32  Cv(i32 n)                         { return n < 16 ? 0 : SignificantBits(n - 16) + 1; }
	static void LinkFree(BlkHeader_<4096> *h)     { Dbl_LinkAfter(h, freelist[Cv(h->GetSize())]); }
	static void NewFreeSize(BlkHeader_<4096> *h)  {}
};

struct Heap : BlkHeap<HugeHeapDetail, 4096> {
	enum {
		LUNIT = 256, // granularity of large blocks (size always a multiple of this)
		LPAGE = 255, // number of LUNITs in large page (need to fix freelist and lclass if changing)
		LOFFSET = 64, // offset from 64KB start to the first block header

		NKLASS = 23, // number of small size classes

		REMOTE_OUT_SZ = 2000, // maximum size of remote frees to be buffered to flush at once
	};

	// allocator options:
	static word HPAGE; // size of master page, in 4KB units
	static i32  max_free_hpages; // maximum free master pages kept in reserve (if more, they are returned to the system)
	static i32  max_free_lpages; // maximum free large pages kept in reserve (if more, they are returned to huge system)
	static i32  max_free_spages; // maximum free small pages kept in reserve (but HugeAlloc also converts them)
	static word sys_block_limit; // > this (in 4KB) blocks are managed directly by system

	uk HugeAlloc(size_t count); // count in 4KB, client needs to not touch HugePrefix
	i32   HugeFree(uk ptr);
	bool  HugeTryRealloc(uk ptr, size_t count);

	static i32 Ksz(i32 k) { // small block size classes
		static i32 sz[] = {
		//  0   1   2   3    4    5    6    7    8    9    10   11
			32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384,
			448, 576, 672, 800, 992, 8, 16, 24, 40, 48, 56
		//  12   13   14   15   16  17  18  19  20  21  22
		//  8 - 56 sizes are only available with TinyAlloc
		};
		static_assert(__countof(sz) == 23, "NKLASS mismatch");
		return sz[k];
	}

	struct FreeLink {
		FreeLink *next;
	};

	struct Page : BlkPrefix { // small block Page
		u8         klass;    // size class
		word         active;   // number of used (active) blocks in this page
		FreeLink    *freelist; // single linked list of free blocks in Page
		Page        *next;     // Pages are in work/full/empty lists
		Page        *prev;

		void         LinkSelf()            { Dbl_Self(this); }
		void         Unlink()              { Dbl_Unlink(this); }
		void         Link(Page *lnk)       { Dbl_LinkAfter(this, lnk);  }

		void         Format(i32 k);

		u8 *Begin()                      { return (u8 *)this + sizeof(Page); }
		u8 *End()                        { return (u8 *)this + 4096; }
		i32   Count()                      { return (i32)(uintptr_t)(End() - Begin()) / Ksz(klass); }
	};

	struct LargeHeapDetail {
		BlkHeader_<LUNIT> freelist[25][1];
		void LinkFree(BlkHeader_<LUNIT> *h);
	};

	struct LargeHeap : BlkHeap<LargeHeapDetail, LUNIT> {};

	typedef LargeHeap::BlkHeader LBlkHeader;

	struct DLink : BlkPrefix { // Large Page header / big block header
		DLink       *next;
		DLink       *prev;
		size_t       size; // only used for big header
	#ifdef CPU_64
		dword        filler[6]; // sizeof need to be 64 because of alignment
	#else
		dword        filler[9];
	#endif

		void         LinkSelf()            { Dbl_Self(this); }
		void         Unlink()              { Dbl_Unlink(this); }
		void         Link(DLink *lnk)      { Dbl_LinkAfter(this, lnk);  }

		LargeHeap::BlkHeader *GetFirst()   { return (LargeHeap::BlkHeader *)((u8 *)this + LOFFSET); } // pointer to data area
	};

	static i32 lclass[];
	static i32 free_lclass[LPAGE + 1];
	static i32 alloc_lclass[LPAGE + 1];

	static_assert(sizeof(BlkPrefix) == 16, "Wrong sizeof(BlkPrefix)");
	static_assert(sizeof(DLink) == 64, "Wrong sizeof(DLink)");

	static StaticMutex mutex;

	Page      work[NKLASS][1];   // circular list of pages that contain some empty blocks
	Page      full[NKLASS][1];   // circular list of pages that contain NO empty blocks
	Page     *empty[NKLASS];     // last fully freed page per klass (hot reserve) / shared global list of empty pages in aux
	FreeLink *cache[NKLASS];     // hot frontend cache of small blocks
	i32       cachen[NKLASS];    // counter of small blocks that are allowed to be stored in cache

	bool      initialized;

	LargeHeap lheap;
	DLink     large[1]; // all large 64KB chunks that belong to this heap
	i32       free_lpages; // empty large pages (in reserve)

	void     *out[REMOTE_OUT_SZ / 8 + 1];
	void    **out_ptr;
	i32       out_size;

	u8      filler1[64]; // make sure the next variable is in distinct cacheline

	FreeLink *small_remote_list; // list of remotely freed small blocks for lazy reclamation
	FreeLink *large_remote_list; // list of remotely freed large blocks for lazy reclamation

	struct HugePage { // to store the list of all huge pages in permanent storage
		void     *page;
		HugePage *next;
	};

	static HugePage *huge_pages;

	static DLink  big[1]; // List of all big blocks
	static Heap   aux;    // Single global auxiliary heap to store orphans and global list of free pages

	static size_t huge_4KB_count; // total number of 4KB pages in small/large/huge blocks
	static i32    free_4KB; // number of empty 4KB pages (linked in aux.empty)
	static size_t big_size; // blocks >~64KB
	static size_t big_count;
	static size_t sys_size;  // blocks allocated directly from system (included in big too)
	static size_t sys_count;
	static size_t huge_chunks; // 32MB master pages
	static size_t huge_4KB_count_max; // peak huge memory allocated
	static HugePage *free_huge_pages; // list of records of freed hpages (to be reused)
	static i32       free_hpages; // empty huge pages (in reserve)

#ifdef HEAPDBG
	static void  DbgFreeFillK(uk ptr, i32 k);
	static uk DbgFreeCheckK(uk p, i32 k);
#else
	static void  DbgFreeFillK(uk ptr, i32 k) {}
	static uk DbgFreeCheckK(uk p, i32 k) { return p; }
#endif

#ifdef flagHEAPSTAT
	static void  Stat(size_t sz);
#else
	static void  Stat(size_t sz) {}
#endif

	void  Init();

	static i32   CheckFree(FreeLink *l, i32 k, bool page = true);
	void  Check();
	static void  DblCheck(Page *p);
	static void  AssertLeaks(bool b);

	static bool  IsSmall(uk ptr) { return (((dword)(uintptr_t)ptr) & 16) == 0; }
	static Page *GetPage(uk ptr) { return (Page *)((uintptr_t)ptr & ~(uintptr_t)4095); }

	Page *WorkPage(i32 k);
	uk AllocK(i32 k);
	void  FreeK(uk ptr, Page *page, i32 k);
	uk Allok(i32 k);
	void  Free(uk ptr, Page *page, i32 k);
	void  Free(uk ptr, i32 k);
	void  Free4KB(i32 k, Page *page);

	static bool FreeSmallEmpty(i32 size4KB, i32 count = INT_MAX);

	void   LInit();
	void  *TryLAlloc(i32 i0, word wcount);
	void  *LAlloc(size_t& size);
	void   FreeLargePage(DLink *l);
	void   LFree(uk ptr);
	bool   LTryRealloc(uk ptr, size_t& newsize);
	size_t LGetBlockSize(uk ptr);

	void   Make(MemoryProfile& f);
	void   DumpLarge();
	void   DumpHuge();

	static void Shrink();

	void SmallFreeDirect(uk ptr);

	void RemoteFlushRaw();
	void RemoteFlush();
	void RemoteFree(uk ptr, i32 size);
	void SmallFreeRemoteRaw(FreeLink *list);
	void SmallFreeRemoteRaw() { SmallFreeRemoteRaw(small_remote_list); small_remote_list = NULL; }
	void SmallFreeRemote();
	void LargeFreeRemoteRaw(FreeLink *list);
	void LargeFreeRemoteRaw() { LargeFreeRemoteRaw(large_remote_list); large_remote_list = NULL; }
	void LargeFreeRemote();
	void FreeRemoteRaw();
	static void MoveLargeTo(DLink *ml, Heap *to_heap);
	void MoveLargeTo(Heap *to_heap);

	void Shutdown();
	static void AuxFinalCheck();

	void  *AllocSz(size_t& sz);
	void   Free(uk ptr);
	size_t GetBlockSize(uk ptr);
	void  *Alloc32();
	void   Free32(uk ptr);
	void  *Alloc48();
	void   Free48(uk ptr);

	bool   TryRealloc(uk ptr, size_t& newsize);
};

force_inline
void Heap::RemoteFlushRaw()
{ // transfer all buffered freed remote blocks to target heaps, no locking
	if(!initialized)
		Init();
	for(uk *o = out; o < out_ptr; o++) {
		FreeLink *f = (FreeLink *)*o;
		Heap *heap = GetPage(f)->heap;
		f->next = heap->small_remote_list;
		heap->small_remote_list = f;
	}
	out_ptr = out;
	out_size = 0;
}

force_inline
void Heap::RemoteFree(uk ptr, i32 size)
{ // buffer freed remote block until REMOTE_OUT_SZ is reached
	if(!initialized)
		Init();
	ASSERT(out_ptr <= out + REMOTE_OUT_SZ / 8 + 1);
	*out_ptr++ = ptr;
	out_size += size;
	if(out_size >= REMOTE_OUT_SZ) {
		Mutex::Lock __(mutex);
		RemoteFlushRaw();
	}
}

force_inline
void Heap::SmallFreeRemoteRaw(FreeLink *list)
{
	while(list) {
		FreeLink *f = list;
		list = list->next;
		SmallFreeDirect(f);
	}
}

force_inline
void Heap::SmallFreeRemote()
{
	while(small_remote_list) { // avoid mutex if likely nothing to free
		FreeLink *list;
		{ // only pick values in mutex, resolve later
			Mutex::Lock __(mutex);
			list = small_remote_list;
			small_remote_list = NULL;
		}
		SmallFreeRemoteRaw(list);
	}
}

force_inline
void Heap::LargeFreeRemoteRaw(FreeLink *list)
{
	while(list) {
		FreeLink *f = list;
		list = list->next;
		LFree(f);
	}
}

force_inline
void Heap::LargeFreeRemote()
{
	while(large_remote_list) { // avoid mutex if likely nothing to free
		FreeLink *list;
		{ // only pick values in mutex, resolve later
			Mutex::Lock __(mutex);
			list = large_remote_list;
			large_remote_list = NULL;
		}
		LargeFreeRemoteRaw(list);
	}
}