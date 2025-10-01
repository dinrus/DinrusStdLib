#include "SSL.h"

#define LLOG(x) // DLOG(x)

namespace drx {

#ifdef DRX_HEAP

static z64 DRX_SSL_alloc = 0;

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
static uk SslAlloc(size_t size, tukk file, i32 line)
#else
static uk SslAlloc(size_t size)
#endif
{
	size_t alloc = size + sizeof(z64);
	z64 *aptr = (z64 *)MemoryAllocSz(alloc);
	*aptr++ = (z64)alloc;
	DRX_SSL_alloc += alloc;
	LLOG("DINRUS_SSL_MALLOC(" << (z64)size << ", size " << size
         << ", " << file << " " << line
	     << ") -> " << aptr << ", MemorySize: " << GetMemoryBlockSize(aptr)
	     << ", total = " << DRX_SSL_alloc << ", thread: " << Thread::GetCurrentId());
	return aptr;
}

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
static void SslFree(uk ptr, tukk file, i32 line)
#else
static void SslFree(uk ptr)
#endif
{
	if(!ptr)
		return;
	z64 *aptr = (z64 *)ptr - 1;
	DRX_SSL_alloc -= *aptr;
	LLOG("DINRUS_SSL_FREE(" << ptr << ", size " << *aptr
	                     << ", " << file << " " << line
	                     << "), MemorySize: " << GetMemoryBlockSize(aptr) << ", total = " << DRX_SSL_alloc << ", thread: " << Thread::GetCurrentId());
	if(*aptr !=  GetMemoryBlockSize(aptr))
		Panic("SslFree повреждён");
	MemoryFree(aptr);
}

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
static uk SslRealloc(uk ptr, size_t size, tukk file, i32 line)
#else
static uk SslRealloc(uk ptr, size_t size)
#endif
{
	LLOG("SslRealloc " << ptr << ", " << size);
	if(!ptr) {
		LLOG("DINRUS_SSL_REALLOC by Alloc:");
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		return SslAlloc(size, file, line);
#else
		return SslAlloc(size);
#endif
	}
	z64 *aptr = (z64 *)ptr - 1;
	if((z64)(size + sizeof(z64)) <= *aptr) { // СДЕЛАТЬ: Do we really want this?
		LLOG("DINRUS_SSL_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << *aptr << ") -> keep same block" << ", thread: " << Thread::GetCurrentId());
		return ptr;
	}
	size_t newalloc = size + sizeof(z64);
	z64 *newaptr = (z64 *)MemoryAllocSz(newalloc);
	if(!newaptr) {
		LLOG("DINRUS_SSL_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << newalloc << ") -> fail" << ", thread: " << Thread::GetCurrentId());
		return NULL;
	}
	*newaptr++ = newalloc;
	memcpy(newaptr, ptr, min<i32>((i32)(*aptr - sizeof(z64)), (i32)size));
	DRX_SSL_alloc += newalloc - *aptr;
	LLOG("DINRUS_SSL_REALLOC(" << ptr << ", " << (z64)size << ", alloc " << newalloc
	     << ", " << file << " " << line
	     << ") -> " << newaptr << ", total = " << DRX_SSL_alloc << ", thread: " << Thread::GetCurrentId());
	MemoryFree(aptr);
	return newaptr;
}

#endif

void TcpSocketInit();

INITIALIZER(SSL)
{
	MemoryIgnoreLeaksBlock __;
	LLOG("SslInit");
	TcpSocketInit();
#ifdef DRX_HEAP
	CRYPTO_set_mem_functions(SslAlloc, SslRealloc, SslFree);
#endif
	SSL_library_init();
	SSL_load_error_strings();
}

EXITBLOCK
{
	MemoryIgnoreLeaksBlock __;
	CONF_modules_unload(1);
	EVP_cleanup();
	ENGINE_cleanup();
	CRYPTO_cleanup_all_ex_data();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	STACK_OF(SSL_COMP) *pCOMP = SSL_COMP_get_compression_methods();
	if(pCOMP)
		sk_SSL_COMP_free( pCOMP );
	ERR_remove_state(0);
#else
//	ERR_remove_thread_state(NULL);
	OPENSSL_thread_stop();
#endif
	ERR_free_strings();
}


static thread_local bool sThreadInit;
static thread_local void (*sPrevExit)();

static void sslExitThread()
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	ERR_remove_state(0);
#else
//	ERR_remove_thread_state(NULL);
	OPENSSL_thread_stop();
#endif
	if(sPrevExit)
		(*sPrevExit)();
}

void SslInitThread()
{
	MemoryIgnoreLeaksBlock __;
	if(sThreadInit || Thread::IsMain())
		return;
	sThreadInit = true;
	sPrevExit = Thread::AtExit(sslExitThread);
}

}
