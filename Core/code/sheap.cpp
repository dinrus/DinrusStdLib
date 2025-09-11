#include <drx/Core/Core.h>

#define LTIMING(x) // RTIMING(x)

namespace drx {

#ifdef DRX_HEAP

#include "../HeapImp.h"

#define LLOG(x) //  LOG((uk )this << ' ' << x)

inline void Heap::Page::Format(i32 k)
{
	DbgFreeFill(Begin(), End() - Begin());
	klass = k;
	active = 0;
	i32 sz = Ksz(k);
	u8 *ptr = End() - sz;
	u8 *b = Begin();
	FreeLink *l = NULL;
	while(ptr >= b) {
		((FreeLink *)ptr)->next = l;
		l = (FreeLink *)ptr;
		ptr -= sz;
	}
	freelist = l;
}

Heap::Page *Heap::WorkPage(i32 k) // get a new workpage with empty blocks
{
	LLOG("AllocK - next work page not available " << k << " empty: " << (uk )empty[k]);
	Page *page = empty[k]; // hot empty page of the same klass
	empty[k] = NULL;
	if(!page) { // try to reacquire pages freed remotely
		LLOG("AllocK - trying FreeRemote");
		SmallFreeRemote();
		if(work[k]->freelist) { // partially free page found
			LLOG("AllocK - work available after FreeRemote " << k);
			return work[k];
		}
		page = empty[k]; // hot empty page
		empty[k] = NULL;
	}
	if(!page)
		for(i32 i = 0; i < NKLASS; i++) // Try hot empty page of different klass
			if(empty[i]) {
				LLOG("AllocK - free page available for reformatting " << k);
				page = empty[i];
				empty[i] = NULL;
				page->Format(k); // reformat the page for the required klass
				break;
			}
	if(!page) { // Attempt to find page in global storage of free pages
		Mutex::Lock __(mutex);
		if(this != &aux) { // Try to take the page from the aux
			aux.SmallFreeRemoteRaw();
			if(aux.work[k]->next != aux.work[k]) { // Try page of the same klass first
				page = aux.work[k]->next;
				page->Unlink();
				LLOG("AllocK - adopting aux page " << k << " page: " << (uk )page << ", free " << (uk )page->freelist);
			}
			if(!page && aux.empty[k]) { // Try free page of the same klass (no need to format it)
				page = aux.empty[k];
				aux.empty[k] = page->next;
				free_4KB--;
				ASSERT(free_4KB < max_free_spages);
				LLOG("AllocK - empty aux page available of the same format " << k << " page: " << (uk )page << ", free " << (uk )page->freelist);
			}
			if(!page)
				for(i32 i = 0; i < NKLASS; i++) // Finally try to find free page of different klass
					if(aux.empty[i]) {
						page = aux.empty[i];
						aux.empty[i] = page->next;
						free_4KB--;
						page->Format(k);
						ASSERT(free_4KB < max_free_spages);
						LLOG("AllocK - empty aux page available for reformatting " << k << " page: " << (uk )page << ", free " << (uk )page->freelist);
						break;
					}
		}
		if(!page) { // No free memory was found, ask huge for the new page
			page = (Page *)HugeAlloc(1);
			LLOG("AllocK - allocated new system page " << (uk )page << " " << k);
			page->Format(k);
		}
		page->heap = this;
	}
	page->Link(work[k]);
	ASSERT(page->klass == k);
	return page;
}

uk Heap::AllocK(i32 k)
{
	LLOG("AllocK " << k);
	if(!initialized)
		Init();
	Page *page = work[k]->next;
	for(;;) {
		ASSERT(page->klass == k);
		FreeLink *p = page->freelist;
		if(p) {
			LLOG("AllocK allocating from " << (uk )page << " " << (uk )p);
			page->freelist = p->next;
			++page->active;
			return p;
		}
		LLOG("AllocK - page exhausted " << k << " page: " << (uk )page);
		if(page->next != page) {
			LLOG("Moving " << (uk )page << " to full");
			page->Unlink();
			page->Link(full[k]);
			page = work[k]->next;
		}
		if(page->next == page)
			page = WorkPage(k);
	}
}

force_inline
uk Heap::Allok(i32 k)
{ // Try to alloc from the front-cache first
	LTIMING("Allok");
	FreeLink *ptr = cache[k];
	if(ptr) {
		cachen[k]++;
		cache[k] = ptr->next;
		return DbgFreeCheckK(ptr, k);
	}
	return DbgFreeCheckK(AllocK(k), k);
}

force_inline
uk Heap::AllocSz(size_t& sz)
{
	LTIMING("Alloc");
	LLOG("Alloc " << asTxt(sz));
	Stat(sz);
	if(sz <= 384) {
		if(sz == 0)
			sz = 1;
		i32 k = ((i32)sz - 1) >> 5;
		sz = (k + 1) << 5;
		return Allok(k);
	}
	if(sz <= 992) {
		if(sz <= 448) {
			sz = 448;
			return Allok(12);
		}
		if(sz <= 576) {
			sz = 576;
			return Allok(13);
		}
		if(sz <= 672) {
			sz = 672;
			return Allok(14);
		}
		if(sz <= 800) {
			sz = 800;
			return Allok(15);
		}
		sz = 992;
		return Allok(16);
	}
	return LAlloc(sz);
}

force_inline
void Heap::FreeK(uk ptr, Page *page, i32 k)
{
	if(page->freelist) {
		LLOG("Free next in work page " << k);
		((FreeLink *)ptr)->next = page->freelist;
	}
	else {
		LLOG("Free full to work " << k << " heap: " << (uk )page->heap);
		page->Unlink();
		page->Link(work[k]);
		((FreeLink *)ptr)->next = NULL;
	}
	page->freelist = (FreeLink *)ptr;
	if(--page->active == 0) { // there are no more allocated blocks in this page
		LLOG("Free page is empty " << (uk )page);
		page->Unlink();
		if(this == &aux) {
			LLOG("...is aux " << asTxt(free_4KB));
			Mutex::Lock __(mutex);
			Free4KB(k, page);
		}
		else {
			if(empty[k]) { // Keep one hot empty page per klass in thread, put rest to 'aux' global storage
				Mutex::Lock __(mutex);
				Free4KB(k, empty[k]); // Free current hot page to reserve/huge
			}
			empty[k] = page; // this empty page is now hot
		}
	}
}

void Heap::Free4KB(i32 k, Page *page)
{ // put empty 4KB to aux reserve or back to huge blocks if the reserve is full
	LLOG("Global Free4KB " << k << " " << (uk )empty);
	if(free_4KB < max_free_spages) { // only keep max_free_spages, release if more
		page->heap = &aux;
		page->next = aux.empty[k];
		aux.empty[k] = page;
		free_4KB++;
		LLOG("Reserve 4KB " << asTxt(free_4KB));
	}
	else {
		aux.HugeFree(page);
		LLOG("HugeFree 4KB " << asTxt(free_4KB));
	}
}

force_inline
void Heap::Free(uk ptr, Page *page, i32 k)
{
	LTIMING("Small Free");
	LLOG("Small free page: " << (uk )page << ", k: " << k << ", ksz: " << Ksz(k));
	ASSERT((4096 - ((uintptr_t)ptr & (uintptr_t)4095)) % Ksz(k) == 0);
	if(page->heap != this) { // freeing block allocated in different thread
		RemoteFree(ptr, Ksz(k)); // add to originating heap's list of free pages to be properly freed later
		return;
	}
	DbgFreeFillK(ptr, k);
	if(cachen[k]) {
		cachen[k]--;
		FreeLink *l = (FreeLink *)ptr;
		l->next = cache[k];
		cache[k] = l;
		return;
	}
	FreeK(ptr, page, k);
}

force_inline
void Heap::Free(uk ptr)
{
	if(!ptr) return;
	LLOG("Free " << ptr);
	if(IsSmall(ptr)) {
		Page *page = GetPage(ptr);
		Free(ptr, page, page->klass);
	}
	else
		LFree(ptr);
}

size_t Heap::GetBlockSize(uk ptr)
{
	if(!ptr) return 0;
	LLOG("GetBlockSize " << ptr);
	if(IsSmall(ptr)) {
		Page *page = GetPage(ptr);
		i32 k = page->klass;
		return Ksz(k);
	}
	return LGetBlockSize(ptr);
}

void Heap::SmallFreeDirect(uk ptr)
{ // does not need to check for target heap or small vs large
	LLOG("Free Direct " << ptr);
	Page *page = GetPage(ptr);
	ASSERT(page->heap == this);
	i32 k = page->klass;
	LLOG("Small free page: " << (uk )page << ", k: " << k << ", ksz: " << Ksz(k));
	ASSERT((4096 - ((uintptr_t)ptr & (uintptr_t)4095)) % Ksz(k) == 0);
	DbgFreeFillK(ptr, k);
	FreeK(ptr, page, k);
}

bool Heap::FreeSmallEmpty(i32 size4KB, i32 count)
{ // attempt to release small 4KB pages to gain count4KB space or count of releases
	bool released;
	do {
		released = false;
		for(i32 i = 0; i < NKLASS; i++)
			if(aux.empty[i]) {
				Page *q = aux.empty[i];
				aux.empty[i] = q->next;
				free_4KB--;
				ASSERT(free_4KB < max_free_spages);
				if(aux.HugeFree(q) >= size4KB || --count <= 0) // HugeFree is really static, aux needed just to compile
					return true;
				released = true;
			}
	}
	while(released);
	return false;
}

force_inline
uk Heap::Alloc32()
{
	Stat(32);
	return Allok(KLASS_32);
}

force_inline
void Heap::Free(uk ptr, i32 k)
{
	Free(ptr, GetPage(ptr), k);
}

force_inline
void Heap::Free32(uk ptr)
{
	Free(ptr, KLASS_32);
}

force_inline
uk Heap::Alloc48()
{
	Stat(48);
	return Allok(KLASS_48);
}

force_inline
void Heap::Free48(uk ptr)
{
	Free(ptr, KLASS_48);
}

static thread_local bool heap_closed__;
static thread_local Heap *heap_tls__;

void Heap::Shutdown()
{ // Move all active blocks, "orphans", to global aux heap
	LLOG("**** Shutdown heap " << asTxt(this));
	Mutex::Lock __(mutex);
	heap_closed__ = true;
	heap_tls__ = NULL;
	Init();
	RemoteFlushRaw(); // Move remote blocks to originating heaps
	FreeRemoteRaw(); // Free all remotely freed blocks
	for(i32 i = 0; i < NKLASS; i++) { // move all small pages to aux (some heap will pick them later)
		LLOG("Free cache " << asTxt(i));
		FreeLink *l = cache[i];
		while(l) {
			FreeLink *h = l;
			l = l->next;
			SmallFreeDirect(h);
		}
		while(full[i]->next != full[i]) {
			Page *p = full[i]->next;
			p->Unlink();
			p->heap = &aux;
			p->Link(aux.full[i]);
			LLOG("Orphan full " << (uk )p);
		}
		while(work[i]->next != work[i]) {
			Page *p = work[i]->next;
			p->Unlink();
			p->heap = &aux;
			p->Link(p->freelist ? aux.work[i] : aux.full[i]);
			LLOG("Orphan work " << (uk )p);
		}
		if(empty[i]) {
			ASSERT(empty[i]->freelist);
			ASSERT(empty[i]->active == 0);
			Free4KB(i, empty[i]);
			LLOG("Orphan empty " << (uk )empty[i]);
		}
	}
	MoveLargeTo(&aux); // move all large pages to aux, some heap will pick them later
	memset(this, 0, sizeof(Heap));
	LLOG("++++ Done Shutdown heap " << asTxt(this));
}

never_inline
void EnterHeapMutex()
{
	Heap::mutex.Enter();
}

never_inline
void LeaveHeapMutex()
{
	Heap::mutex.Leave();
}

struct HeapMutexLock {
	HeapMutexLock()  { EnterHeapMutex(); }
	~HeapMutexLock() { LeaveHeapMutex(); }
};

void MemoryFreeThread();

struct HeapExitThreadGuard {
	void Used() {}
	~HeapExitThreadGuard() { MemoryFreeThread(); }
};

Heap *MakeHeap()
{
	if(heap_closed__)
		return &Heap::aux;

	static thread_local HeapExitThreadGuard __;
	__.Used(); // "odr-used", register allocator to be shutdown at thread exit

	static thread_local u8 sHeap__[sizeof(Heap)]; // zero initialization is fine for us
	heap_tls__ = (Heap *)sHeap__;

	return heap_tls__;
}

struct CurrentHeap {
	bool  locked = false;
	Heap *heap;

	Heap *operator->() { return heap; }

	CurrentHeap() {
		heap = heap_tls__;
		if(!heap) {
			EnterHeapMutex();
			locked = true;
			heap = MakeHeap();
		}
	}
	~CurrentHeap() {
		if(locked)
			LeaveHeapMutex();
	}
};

void MemoryFreek__(i32 klass, uk ptr)
{
	Heap *heap = heap_tls__;
	if(heap)
		heap->Free((uk )ptr, klass);
	else {
		HeapMutexLock __;
		MakeHeap()->Free((uk )ptr, klass);
	}
}

uk MemoryAllok__(i32 klass)
{
	Heap *heap = heap_tls__;
	if(heap)
		return heap->Allok(klass);
	else {
		HeapMutexLock __;
		return MakeHeap()->Allok(klass);
	}
}

#if defined(HEAPDBG)

uk MemoryAllocSz_(size_t& sz)
{
	return CurrentHeap()->AllocSz(sz);
}

void  MemoryFree_(uk ptr)
{
	return CurrentHeap()->Free(ptr);
}

bool  MemoryTryRealloc_(uk ptr, size_t& size)
{
	return CurrentHeap()->TryRealloc(ptr, size);
}

size_t GetMemoryBlockSize_(uk ptr)
{
	return CurrentHeap()->GetBlockSize(ptr);
}

#else

#ifdef flagHEAPLOG

#undef AllocSz

StaticMutex sHeapLogLock;

static FILE *sLog = fopen(GetExeDirFile("heap.log"), "w");

void LogFree(uk ptr)
{
	if(sLog) {
		Mutex::Lock __(sHeapLogLock);
		fprintf(sLog, "-%x %p\n", (i32)Thread::GetCurrentId(), ptr);
	}
}

uk LogAlloc(uk ptr, size_t sz)
{
	if(sLog) {
		Mutex::Lock __(sHeapLogLock);
		fprintf(sLog, "%x %zx %p\n", (i32)Thread::GetCurrentId(), sz, ptr);
	}
	return ptr;
}

#else

inline void LogFree(uk ptr) {}

inline uk LogAlloc(uk ptr, size_t sz) { return ptr; }

#endif

// xxx2 routines in the following code are to make things explicit for optimiser

never_inline
uk MemoryAlloc2(size_t& sz)
{
	HeapMutexLock __;
	return LogAlloc(MakeHeap()->AllocSz(sz), sz);
}

uk MemoryAlloc(size_t sz)
{
	LTIMING("MemoryAlloc");
	Heap *heap = heap_tls__;
	if(heap)
		return LogAlloc(heap->AllocSz(sz), sz);
	return MemoryAlloc2(sz);
}

uk MemoryAllocSz(size_t& sz)
{
	LTIMING("MemoryAllocSz");
	Heap *heap = heap_tls__;
	if(heap)
		return LogAlloc(heap->AllocSz(sz), sz);
	return MemoryAlloc2(sz);
}

never_inline
void MemoryFree2(uk ptr)
{
	HeapMutexLock __;
	MakeHeap()->Free(ptr);
}

void  MemoryFree(uk ptr)
{
	LTIMING("MemoryFree");
	LogFree(ptr);

	Heap *heap = heap_tls__;
	if(heap)
		heap->Free(ptr);
	else
		MemoryFree2(ptr);
}

size_t GetMemoryBlockSize(uk ptr)
{
	return CurrentHeap()->GetBlockSize(ptr);
}

bool MemoryTryRealloc__(uk ptr, size_t& size)
{
	Heap *heap = heap_tls__;
	if(heap)
		return heap->TryRealloc(ptr, size);
	else {
		HeapMutexLock __;
		return MakeHeap()->TryRealloc(ptr, size);
	}
}

never_inline
uk MemoryAlloc32_2()
{
	HeapMutexLock __;
	return LogAlloc(MakeHeap()->Alloc32(), 32);
}

force_inline
uk MemoryAlloc32_i()
{
	LTIMING("MemoryAlloc32");
	Heap *heap = heap_tls__;
	if(heap)
		return LogAlloc(heap->Alloc32(), 32);
	return MemoryAlloc32_2();
}

uk MemoryAlloc32() { return MemoryAlloc32_i(); }

never_inline
void  MemoryFree32_2(uk ptr)
{
	HeapMutexLock __;
	MakeHeap()->Free32(ptr);
}

force_inline
void  MemoryFree32_i(uk ptr)
{
	LTIMING("MemoryFree32");
	LogFree(ptr);
	Heap *heap = heap_tls__;
	if(heap)
		heap->Free32(ptr);
	else
		MemoryFree32_2(ptr);
}

void  MemoryFree32(uk ptr) { MemoryFree32_i(ptr); }

#endif

void MemoryFreeThread()
{
	if(heap_closed__)
		return;
	CurrentHeap()->Shutdown();
}

void MemoryCheck()
{
	CurrentHeap()->Check();
}

MemoryProfile::MemoryProfile()
{
	CurrentHeap()->Make(*this);
}

void MemoryDumpLarge()
{
	CurrentHeap()->DumpLarge();;
}

void MemoryDumpHuge()
{
	CurrentHeap()->DumpHuge();
}

#ifdef HEAPDBG
force_inline uk MemoryAlloc32_i() { return MemoryAlloc32(); }
force_inline void  MemoryFree32_i(uk ptr) { return MemoryFree32(ptr); }
#endif

#include "../StringMem.i"

#endif

}