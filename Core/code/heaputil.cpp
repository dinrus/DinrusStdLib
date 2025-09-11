#include "../Core.h"

#ifdef DRX_HEAP

#ifdef TARGET_POSIX
#include <sys/mman.h>
#endif

namespace drx {

#include "../HeapImp.h"

void OutOfMemoryPanic(size_t size)
{
	char h[200];
	sprintf(h, "Нехватка памяти!\nРазмещённая U++ память: %d KB", MemoryUsedKb());
	Panic(h);
}

size_t Heap::huge_4KB_count;
i32    Heap::free_4KB;
size_t Heap::big_size;
size_t Heap::big_count;
size_t Heap::sys_size;
size_t Heap::sys_count;
size_t Heap::huge_chunks;
size_t Heap::huge_4KB_count_max;

i32 MemoryUsedKb()
{
	return i32(4 * (Heap::huge_4KB_count - Heap::free_4KB));
}

i32 MemoryUsedKbMax()
{
	return i32(4 * Heap::huge_4KB_count_max);
}

uk SysAllocRaw(size_t size, size_t reqsize)
{
	uk ptr = NULL;
#ifdef TARGET_WIN32
	ptr = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#elif TARGET_LINUX
	ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED)
		ptr = NULL;
#else
	ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
	if(ptr == MAP_FAILED)
		ptr = NULL;
#endif
	if(!ptr)
		OutOfMemoryPanic(size);
	return ptr;
}

void  SysFreeRaw(uk ptr, size_t size)
{
#ifdef TARGET_WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#else
	munmap(ptr, size);
#endif
}

uk MemoryAllocPermanent(size_t size)
{
	Mutex::Lock __(Heap::mutex);
	if(size > 10000)
		return SysAllocRaw(size, size);
	static u8 *ptr = NULL;
	static u8 *limit = NULL;
	ASSERT(size < INT_MAX);
	if(ptr + size >= limit) {
		ptr = (u8 *)SysAllocRaw(16384, 16384);
		limit = ptr + 16384;
	}
	uk p = ptr;
	ptr += size;
	return p;
}

void HeapPanic(tukk text, uk pos, i32 size)
{
	RLOG("\n\n" << text << "\n");
	HexDump(VppLog(), pos, size, 1024);
	Panic(text);
}

#ifdef HEAPDBG

uk Heap::DbgFreeCheckK(uk p, i32 k)
{
	Page *page = GetPage(p);
	ASSERT((u8 *)page + sizeof(Page) <= (u8 *)p && (u8 *)p < (u8 *)page + 4096);
	ASSERT((4096 - ((uintptr_t)p & (uintptr_t)4095)) % Ksz(k) == 0);
	ASSERT(page->klass == k);
	DbgFreeCheck((FreeLink *)p + 1, Ksz(k) - sizeof(FreeLink));
	return p;
}

void Heap::DbgFreeFillK(uk p, i32 k)
{
	DbgFreeFill((FreeLink *)p + 1, Ksz(k) - sizeof(FreeLink));
}

#endif


void Heap::Make(MemoryProfile& f)
{
	Mutex::Lock __(mutex);
	memset((uk )&f, 0, sizeof(MemoryProfile));
	for(i32 i = 0; i < NKLASS; i++) {
		i32 qq = Ksz(i) / 4;
		Page *p = work[i]->next;
		while(p != work[i]) {
			f.allocated[qq] += p->active;
			f.fragments[qq] += p->Count() - p->active;
			p = p->next;
		}
		p = full[i]->next;
		while(p != full[i]) {
			f.allocated[qq] += p->active;
			p = p->next;
		}
		if(empty[i])
			f.freepages++;
		p = aux.empty[i];
		while(p) {
			f.freepages++;
			p = p->next;
		}
	}
	DLink *m = large->next;
	while(m != large) {
		LargeHeap::BlkHeader *h = m->GetFirst();
		for(;;) {
			if(h->IsFree()) {
				f.large_fragments_count++;
				i32 sz = LUNIT * h->GetSize();
				f.large_fragments_total += sz;
				if(h->size < 2048)
					f.large_fragments[sz >> 8]++;
			}
			else {
				f.large_count++;
				f.large_total += LUNIT * h->size;
			}
			if(h->IsLast())
				break;
			h = h->GetNextHeader();
		}
		m = m->next;
	}

	f.sys_count = (i32)sys_count;
	f.sys_total = sys_size;

	f.huge_count = i32(big_count - sys_count);
	f.huge_total = big_size - sys_size; // this is not 100% correct, but approximate

	f.master_chunks = (i32)huge_chunks;

	HugePage *pg = huge_pages;
	while(pg) {
		BlkPrefix *h = (BlkPrefix *)pg->page;
		for(;;) {
			if(h->IsFree()) {
				word sz = h->GetSize();
				f.huge_fragments[sz]++;
				f.huge_fragments_count++;
				f.huge_fragments_total += sz;
			}
			if(h->IsLast())
				break;
			h = h->GetNextHeader(4096);
		}
		pg = pg->next;
	}
}

void Heap::DumpLarge()
{
	Mutex::Lock __(mutex);
	DLink *m = large->next;
	auto& out = VppLog();
	while(m != large) {
		LargeHeap::BlkHeader *h = m->GetFirst();
		out << h << ": ";
		for(;;) {
			if(h->IsFree())
				out << "#";
			out << h->GetSize() * 0.25 << ' ';
			if(h->IsLast())
				break;
			h = h->GetNextHeader();
		}
		out << "\r\n";
		m = m->next;
	}
}

void Heap::DumpHuge()
{
	Mutex::Lock __(mutex);
	HugePage *pg = huge_pages;
	auto& out = VppLog();
	while(pg) {
		BlkPrefix *h = (BlkPrefix *)pg->page;
		out << h << ": ";
		for(;;) {
			if(h->IsFree())
				out << "#";
			out << 4 * h->GetSize() << ' ';
			if(h->IsLast())
				break;
			h = h->GetNextHeader(4096);
		}
		out << "\r\n";
		pg = pg->next;
	}
}

Txt AsTxt(const MemoryProfile& mem)
{
	Txt text;
#ifdef DRX_HEAP
	i32 acount = 0;
	size_t asize = 0;
	i32 fcount = 0;
	size_t fsize = 0;
	text << "Пик памяти: " << MemoryUsedKbMax() << " КБ, текущая: " << MemoryUsedKb() << "КБ \n";
	for(i32 i = 0; i < 1024; i++)
		if(mem.allocated[i]) {
			i32 sz = 4 * i;
			text << Format("%4d Б, %7d размещено (%6d KB), %6d фрагментов (%6d КБ)\n",
			              sz, mem.allocated[i], (mem.allocated[i] * sz) >> 10,
			              mem.fragments[i], (mem.fragments[i] * sz) >> 10);
			acount += mem.allocated[i];
			asize += mem.allocated[i] * sz;
			fcount += mem.fragments[i];
			fsize += mem.fragments[i] * sz;
		}
	text << Format(" ВСЕГО, %7d размещено (%6d КБ), %6d фрагментов (%6d КБ)\n",
	              acount, i32(asize >> 10), fcount, i32(fsize >> 10));
	text << "Пустые 4КБ страницы " << mem.freepages << " (" << mem.freepages * 4 << " КБ)\n";
	text << "Счёт блоков Large " << mem.large_count
	     << ", общий размер " << (mem.large_total >> 10) << " КБ\n";
	text << "Счёт фрагментов Large " << mem.large_fragments_count
	     << ", общий размер " << (mem.large_fragments_total >> 10) << " КБ\n";
	text << "Счёт блоков Huge " << mem.huge_count
	     << ", общий размер " << i32(mem.huge_total >> 10) << " КБ\n";
	text << "Счёт фрагментов Huge " << mem.huge_fragments_count
	     << ", общий размер " << 4 * mem.huge_fragments_total << " КБ\n";
	text << "Счёт блоков Syst " << mem.sys_count
	     << ", общий размер " << i32(mem.sys_total >> 10) << " КБ\n";
	text << Heap::HPAGE * 4 / 1024 << "МБ мастер-блоки " << mem.master_chunks << "\n";
	text << "\nФрагменты Large:\n";
	for(i32 i = 0; i < 2048; i++)
		if(mem.large_fragments[i])
			text << 256.0 * i / 1024 << " КБ: " << mem.large_fragments[i] << "\n";
	text << "\nФрагменты Huge:\n";
	for(i32 i = 0; i < 65535; i++)
		if(mem.huge_fragments[i])
			text << i * 4 << " КБ: " << mem.huge_fragments[i] << "\n";
#endif
	return text;
}

#ifdef flagHEAPSTAT
i32 stat[65536];
i32 bigstat;

void Heap::Stat(size_t sz)
{
	if(sz < 65536)
		stat[sz]++;
	else
		bigstat++;
}

EXITBLOCK {
	i32 sum = 0;
	for(i32 i = 0; i < 65536; i++)
		sum += stat[i];
	sum += bigstat;
	i32 total = 0;
	VppLog() << Sprintf("Статистика размещений: (всего размещений: %d)\n", sum);
	for(i32 i = 0; i < 65536; i++)
		if(stat[i]) {
			total += stat[i];
			VppLog() << Sprintf("%5d %8dx %2d%%, всего %8dx %2d%%\n",
			                    i, stat[i], 100 * stat[i] / sum, total, 100 * total / sum);
		}
	if(bigstat) {
		total += bigstat;
		VppLog() << Sprintf(">64KB %8dx %2d%%, всего %8dx %2d%%\n",
		                    bigstat, 100 * bigstat / sum, total, 100 * total / sum);
	}
}
#endif

}

#endif