// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Functions4U_bsdiff_h_
#define _Functions4U_bsdiff_h_

#define ftello  ftell
#define fseeko  fseek

namespace Upp {

extern String errMsg;
//String BsGetLastError();
bool Err(String str);

#if defined(_WINDOWS_) || defined(__MINGW32__)
    #include <io.h>
    #include <share.h>
    #include <sys/stat.h>

    #define lseek   _lseek
    #define read    _read
    #define write   _write
    #define close   _close
    typedef unsigned char u_char;

    #define _SH_DENYNO 0x40
    #if !defined(__MINGW32__)
        typedef long pid_t;
        typedef signed int ssize_t;
    #endif
#endif

}

#endif
