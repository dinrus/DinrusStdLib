#ifndef CORE_H
#define CORE_H

#define DRX_VERSION 0x20250130
#include <drxtypes.h>

#define _MULTITHREADED
#define MULTITHREADED
#ifdef flagDLL
    #define flagUSEMALLOC
    #define STD_NEWDELETE
#endif

#ifdef flagDLL
    #define _USRDLL
#endif

#ifdef flagHEAPDBG
    #define HEAPDBG
#endif

#if defined(flagDEBUG)
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #ifndef TESTLEAKS
        #define TESTLEAKS
    #endif
    #ifndef HEAPDBG
        #define HEAPDBG
    #endif
#else
    #ifndef _RELEASE
        #define _RELEASE
    #endif
#endif

#include "config.h"

#if defined(flagSTD_NEWDELETE) && !defined(STD_NEWDELETE)
#define STD_NEWDELETE
#endif

#ifdef _MSC_VER
    #ifndef _CPPRTTI
        #error  RTTI должен быть включен !!!
    #endif  //_CPPRTTI
#endif

#include <drxtypes.h>

#ifdef CPU_X86
#include <immintrin.h>

#ifdef TARGET_WIN32
#include <intrin.h>
#endif

#ifdef COMPILER_GCC
#include <x86intrin.h>
#endif

#endif

#if defined(TARGET_POSIX)
    #ifndef __USE_FILE_OFFSET64
        #define __USE_FILE_OFFSET64
    #endif
    #define DIR_SEP  '/'
    #define DIR_SEPS "/"
    #define TARGET_PATH_HAS_CASE 1

    #include <errno.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <sys/file.h>
    #include <time.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <memory.h>
    #include <dirent.h>
    #include <signal.h>
    #include <syslog.h>
    #include <float.h>
    #include <fenv.h>
    #ifdef TARGET_SOLARIS
        #include <inttypes.h>
    #else
        #include <stdint.h>
    #endif
    #ifdef TARGET_OSX
        #include <dispatch/dispatch.h>
    #endif
#endif //TARGET_POSIX

#ifdef TARGET_POSIX
#define LOFF_T_      off_t
#define LSEEK64_     lseek
#define FTRUNCATE64_ ftruncate
#endif

#ifdef TARGET_LINUX
#undef  LOFF_T_
#define LOFF_T_      loff_t
#undef  LSEEK64_
#define LSEEK64_     lseek64
#undef  FTRUNCATE64_
#define FTRUNCATE64_ ftruncate64
#endif

#ifdef TARGET_WIN32

    #if defined(COMPILER_MSC) && defined(CPU_X86)
        #pragma warning(disable: 4035)
    #else
        #ifndef __NOASSEMBLY__
            #define __NOASSEMBLY__
        #endif
    #endif

    #if defined(COMPILER_MINGW)
        #if !defined(WINVER)
            #define WINVER 0xFFFF
        #endif
        #include <float.h>
    #endif

    #define DIR_SEP  '\\'
    #define DIR_SEPS "\\"
    #define TARGET_PATH_HAS_CASE 0
    #ifndef TARGET_WINCE
        #include <io.h>
    #endif
    #ifndef TARGET_MFC // just mini Windows headers
        #ifdef COMPILER_MSC
            #ifndef CPU_ARM
                #ifndef CPU_AMD64
                    #ifndef _X86_
                        #define _X86_
                    #endif
                #else
                    #ifndef _AMD64_
                        #define _AMD64_
                    #endif
                    #ifndef __NOASSEMBLY__
                        #define __NOASSEMBLY__
                    #endif
                    #ifndef WIN64
                        #define WIN64
                    #endif
                #endif
            #endif
            #ifndef _WINDOWS_
                #define _WINDOWS_
            #endif
            #ifndef _INC_WINDOWS
                #define _INC_WINDOWS
            #endif
            #ifndef _STRUCT_NAME
                #define _STRUCT_NAME(x)
                #define DUMMYSTRUCTNAME
                #define DUMMYSTRUCTNAME2
                #define DUMMYSTRUCTNAME3
            #endif
            #ifndef NO_STRICT
                #ifndef STRICT
                    #define STRICT 1
                #endif
            #endif
            #include <stdarg.h>
            #include <windef.h>
            #include <winbase.h>
            #include <wingdi.h>
            #include <winuser.h>
            #include <Wincon.h>
            #include <float.h>
            #include <mmsystem.h>
        #define u8 win32_byte_ // RpcNdr defines u8 -> class with drx::u8
        #define CY win32_CY_
            #include <objidl.h>
            #include <winnetwk.h>
        #undef u8
        #undef CY
            typedef DWORD LCTYPE;
            #define W_P(w, p) w
            #include <winsock2.h>
            #include <ws2tcpip.h>
            typedef i32 socklen_t;
        #else
            #define W_P(w, p) w
            #if !defined(TARGET_CYGWIN)
            #include <winsock2.h>
            #include <ws2tcpip.h>
            #endif
            typedef i32 socklen_t;
            #define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
            #include <windows.h>
            #include <stdint.h>
        #endif
        #include <process.h>
    #endif

    #ifdef RGBA
        #undef RGBA
    #endif
#endif

#ifdef TARGET_POSIX

#define W_P(w, p) p
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>


enum
{
    INVALID_SOCKET = -1,
#ifndef TCP_NODELAY // macos defines it as macro, with the same value 1
    TCP_NODELAY    = 1,
#endif
    SD_RECEIVE     = 0,
    SD_SEND        = 1,
    SD_BOTH        = 2,
};
typedef i32 SOCKET;
#endif

#include <functional>
#include <algorithm>
#include <string>
#include <complex>
#include <type_traits>
#include <atomic>
#include <chrono>
#include <utility>



// fix MSC8 beta problem....
#ifdef COMPILER_MSC
#ifndef TARGET_WINCE

namespace std {
    inline void __cdecl _Debug_message(const wchar_t *, const wchar_t *, u32 line) {}
};
#endif
#endif

// deprecated, use 'namespace' directly instead of macros
#define NAMESPACE_UPP     namespace drx {
#define END_DRX_NAMESPACE }
#define DRX               drx

namespace drx {

#ifndef flagNODEPRECATED
#ifndef DEPRECATED
#define DEPRECATED
#endif
#endif

#include "Defs.h"

class XmlIO;
class JsonIO;

#include "Ops.h"
#include "Fn.h"

#ifdef flagNOSIMD
    #ifdef CPU_SSE2
    #undef CPU_SSE2
    #endif
    #ifdef CPU_NEON
    #undef CPU_NEON
    #endif
#endif

#if defined(CPU_SSE2) && defined(CPU_32) && defined(TARGET_POSIX)
#undef CPU_SSE2
#endif

#ifdef CPU_SSE2
#include "SIMD_SSE2.h"
#define CPU_SIMD 1
#endif

#ifdef CPU_NEON
#include "SIMD_NEON.h"
#define CPU_SIMD 1
#endif

#include "Mem.h"
#include "Atomic.h"
#include "Topt.h"
#include "Mt.h"
#include "Txt.h"

#include "TimeDate.h"
#include "Stream.h"
#include "Diag.h"

#include "Vcont.h"
#include "Range.h"
#include "BiCont.h"
#include "Index.h"
#include "Map.h"
#include "Algo.h"
#include "Sorted.h"
#include "Sort.h"
#include "Obsolete.h"
#include "FixedMap.h"
#include "InVector.h"

#include "CharSet.h"

#include "Path.h"

#include "SplitMerge.h"

#include "Other.h"

#include "Lang.h"

#include "Val.h"
#include "ValUtil.h"

#include "Tuple.h"

#include "Uuid.h"
#include "Ptr.h"

#include "Function.h"

#include "Callback.h"

#include "Color.h"
#include "Complex.h"

#include "Hash.h"

#include "Util.h"

#include "Profile.h"

#include "FilterStream.h"

#include "Format.h"
#include "Convert.h"

#include "Parser.h"
#include "JSON.h"
#include "XML.h"
#include "Xmlize.h"

#include "Gtypes.h"
#include "i18n.h"
#include "Topic.h"

#include "App.h"

#include "CoWork.h"

#include "CoAlgo.h"
#include "CoSort.h"

#include "LocalProcess.h"

#include "Inet.h"

#include "Win32Util.h"

#include "Vcont.hpp"
#include "Index.hpp"
#include "Map.hpp"
#include "InVector.hpp"
#include "InMap.hpp"
#include "Utf.hpp"

#include "Huge.h"

#include "ValCache.h"

#ifdef CPU_SIMD
Txt AsTxt(const f32x4& x);
Txt AsTxt(const i32x4& x);
Txt AsTxt(const i16x8& x);
Txt AsTxt(const i8x16& x);
#endif

#ifdef TARGET_WIN32
NTL_MOVEABLE(POINT)
NTL_MOVEABLE(SIZE)
NTL_MOVEABLE(RECT)
#endif

}

#if (defined(TESTLEAKS) || defined(HEAPDBG)) && defined(COMPILER_GCC) && defined(DRX_HEAP)

//Place it to the begining of each file to be the first function called in whole executable...
//This is now backup to init_priority attribute in heapdbg.cpp
struct MemDiagCls {
    MemDiagCls();
    ~MemDiagCls();
};
static const MemDiagCls sMemDiagHelper__upp__;


#endif

//some global definitions

void      RegisterTopic__(tukk topicfile, tukk topic, tukk title, u8k *data, i32 len);

#ifdef TARGET_WIN32
typedef HMODULE DLLHANDLE;
#else
typedef void   *DLLHANDLE;
#endif

DLLHANDLE LoadDll__(DRX::Txt& fn, tukk const *names, uk const *procs);
void      FreeDll__(DLLHANDLE dllhandle);

#include <drx/Core/Compress.h>
#include <drx/Core/Pcre.h>
#endif //CORE_H