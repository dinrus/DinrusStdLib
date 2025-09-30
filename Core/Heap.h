struct MemoryOptions { // sizes are in KB
	i32 master_block; // master block size
	i32 sys_block_limit; // > that this: allocate directly from the system
	i32 master_reserve; // free master blocks kept in reserve
	i32 large_reserve; // free large blocks kept in reserve
	i32 small_reserve; // free formatted small block pages kept in reserve
	
	MemoryOptions(); // loads default options
	~MemoryOptions(); // sets options
};

enum {
	DRX_HEAP_ALIGNMENT = 16,
	DRX_HEAP_MINBLOCK = 32,
};

#ifdef DRX_HEAP

uk MemoryAllocPermanent(size_t size);

uk    MemoryAllocSz(size_t& size);
uk    MemoryAlloc(size_t size);
void  MemoryFree(uk ptr);
uk    MemoryAlloc32();
void  MemoryFree32(uk ptr);
void  MemoryCheck();
void  MemoryDumpLarge();
void  MemoryDumpHuge();
i32   MemoryUsedKb();
i32   MemoryUsedKbMax();
void  MemoryLimitKb(i32 kb);

size_t GetMemoryBlockSize(uk ptr);

bool MemoryTryRealloc__(uk ptr, size_t& newsize);

#ifdef _DEBUG
inline // in DEBUG test for small block is moved inside, because debug adds diagnostics header
bool  MemoryTryRealloc(uk ptr, size_t& newsize) {
	return MemoryTryRealloc__(ptr, newsize);
}
#else
inline
bool  MemoryTryRealloc(uk ptr, size_t& newsize) {
	return (((dword)(uintptr_t)ptr) & 16) && MemoryTryRealloc__(ptr, newsize);
}
#endif

void  MemoryBreakpoint(dword serial);

void  MemoryInitDiagnostics();
void  MemoryDumpLeaks();

#ifdef HEAPDBG
void  MemoryIgnoreLeaksBegin();
void  MemoryIgnoreLeaksEnd();

void  MemoryCheckDebug();
#else
inline void  MemoryIgnoreLeaksBegin() {}
inline void  MemoryIgnoreLeaksEnd() {}

inline void  MemoryCheckDebug() {}
#endif

struct MemoryProfile {
	i32    allocated[1024]; // active small blocks (index is size in bytes)
	i32    fragments[1024]; // unallocated small blocks (index is size in bytes)
	i32    freepages; // empty 4KB pages (can be recycled)
	i32    large_count; // count of large (~ 1 - 64KB) active blocks
	size_t large_total; // ^ total size
	i32    large_fragments_count; // count of unused large blocks
	size_t large_fragments_total; // ^ total size
	i32    large_fragments[2048]; // * 256
	i32    huge_count; // bigger blocks managed by U++ heap (<= 32MB)
	size_t huge_total; // ^ total size
	i32    huge_fragments_count; // count of unused large blocks
	size_t huge_fragments_total; // ^ total size
	i32    huge_fragments[65536]; // * 256
	i32    sys_count; // blocks directly allocated from the system (>32MB
	size_t sys_total; // ^total size
	i32    master_chunks; // master blocks

	MemoryProfile();
};

MemoryProfile *PeakMemoryProfile();

enum {
	KLASS_8 = 17,
	KLASS_16 = 18,
	KLASS_24 = 19,
	KLASS_32 = 0,
	KLASS_40 = 20,
	KLASS_48 = 21,
	KLASS_56 = 22,
};

force_inline
i32 TinyKlass__(i32 sz) { // we suppose that this gets resolved at compile time....
	if(sz <= 8) return KLASS_8;
	if(sz <= 16) return KLASS_16;
	if(sz <= 24) return KLASS_24;
	if(sz <= 32) return KLASS_32;
	if(sz <= 40) return KLASS_40;
	if(sz <= 48) return KLASS_48;
	if(sz <= 56) return KLASS_56;
	return -1;
}

uk MemoryAllok__(i32 klass);
void  MemoryFreek__(i32 klass, uk ptr);

inline
uk TinyAlloc(i32 size) {
	i32 k = TinyKlass__(size);
	if(k < 0) return MemoryAlloc(size);
	return MemoryAllok__(k);
}

inline
void TinyFree(i32 size, uk ptr)
{
	i32 k = TinyKlass__(size);
	if(k < 0)
		MemoryFree(ptr);
	else
		MemoryFreek__(k, ptr);
}

#else

inline MemoryOptions::MemoryOptions() {}
inline MemoryOptions::~MemoryOptions() {}

#ifndef flagHEAPOVERRIDE

inline void  *MemoryAllocPermanent(size_t size)                { return malloc(size); }
inline void  *MemoryAlloc(size_t size)     { return new u8[size]; }
inline void  *MemoryAllocSz(size_t &size)  { return new u8[size]; }
inline void   MemoryFree(uk p)          { delete[] (u8 *) p; }
inline void  *MemoryAlloc32()              { return new u8[32]; }
inline void  *MemoryAlloc48()              { return new u8[48]; }
inline void   MemoryFree32(uk ptr)      { delete[] (u8 *)ptr; }
inline void   MemoryFree48(uk ptr)      { delete[] (u8 *)ptr; }
inline void   MemoryInitDiagnostics()      {}
inline void   MemoryCheck() {}
inline void   MemoryCheckDebug() {}
inline i32    MemoryUsedKb() { return 0; }
inline i32    MemoryUsedKbMax() { return 0; }

inline void   MemoryIgnoreLeaksBegin() {}
inline void   MemoryIgnoreLeaksEnd() {}

inline size_t GetMemoryBlockSize(uk ptr) { return 0; }

inline bool   MemoryTryRealloc(uk ptr, size_t& newsize) { return false; }

struct MemoryProfile {
	i32 empty__;
};

inline MemoryProfile *PeakMemoryProfile() { return NULL; }

inline uk TinyAlloc(i32 size) { return MemoryAlloc(size); }

inline void TinyFree(i32, uk ptr) { return MemoryFree(ptr); }

#endif

#endif

dword MemoryGetCurrentSerial();

void  MemoryIgnoreNonMainLeaks();
void  MemoryIgnoreNonUppThreadsLeaks();

struct MemoryIgnoreLeaksBlock {
	MemoryIgnoreLeaksBlock()  { MemoryIgnoreLeaksBegin(); }
	~MemoryIgnoreLeaksBlock() { MemoryIgnoreLeaksEnd(); }
};

template <class T, class... Args>
T *tiny_new(Args... args)
{
	return new(TinyAlloc(sizeof(T))) T(args...);
}

template <class T>
void tiny_delete(T *ptr)
{
	ptr->~T();
	TinyFree(sizeof(T), ptr);
}
