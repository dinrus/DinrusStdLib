#include "SSH.h"

namespace drx {
	
#define MLOG(x)  //  LOG(x)

#ifdef DRX_HEAP

static std::atomic<z64> DRX_SSH_alloc;

static uk ssh_malloc(size_t size, uk *abstract)
{
	MLOG("Alloc " << size);
	size_t alloc = size + sizeof(z64);
	z64 *aptr = (z64 *)MemoryAllocSz(alloc);
	*aptr++ = (z64)alloc;
	DRX_SSH_alloc += alloc;
	MLOG("DINRUS_SSH_MALLOC(" << (z64)size << ", alloc " << alloc << ") -> " << FormatIntHex(aptr) << ", total = " << (z64) DRX_SSH_alloc);
	return aptr;
}

static void ssh_free(uk ptr, uk *abstract)
{
	if(!ptr)
		return;
	z64 *aptr = (z64 *)ptr - 1;
	DRX_SSH_alloc -= *aptr;
	MLOG("DINRUS_SSH_FREE(" << ptr << ", alloc " << *aptr << "), total = " << (z64) DRX_SSH_alloc);
	MemoryFree(aptr);
}

static uk ssh_realloc(uk ptr, size_t size, void** abstract)
{
	if(!ptr)
		return NULL;
	z64 *aptr = (z64 *)ptr - 1;
	if((z64)(size + sizeof(z64)) <= *aptr) {
		MLOG("DINRUS_SSH_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << *aptr << ") -> keep same block");
		return ptr;
	}
	size_t newalloc = size + sizeof(z64);
	z64 *newaptr = (z64 *)MemoryAllocSz(newalloc);
	if(!newaptr) {
		MLOG("DINRUS_SSH_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << newalloc << ") -> fail");
		return NULL;
	}
	*newaptr++ = newalloc;
	memcpy(newaptr, ptr, min<i32>((i32)(*aptr - sizeof(z64)), (i32)size));
	DRX_SSH_alloc += newalloc - *aptr;
	MLOG("DINRUS_SSH_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << newalloc << ") -> "
		<< FormatIntHex(newaptr) << ", total = " << (z64) DRX_SSH_alloc);
	MemoryFree(aptr);
	return newaptr;
}

#endif
}