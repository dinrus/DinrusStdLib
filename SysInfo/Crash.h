// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _CrashHandler_Crash_h_
#define _CrashHandler_Crash_h_

#include <drx/Core/Core.h>
#include "SysInfo_in.h"

namespace drx {

class CrashHandler {
public:
    CrashHandler();
    virtual ~CrashHandler() {};

private:
#if defined(TARGET_WIN32)
    static LONG WINAPI UnhandledHandler(EXCEPTION_POINTERS *p);
    static void __cdecl SEHHandler(unsigned u, EXCEPTION_POINTERS* p);
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression,
        const wchar_t* function, const wchar_t* file, u32 line, uintptr_t pReserved);

    static void __cdecl NewHandler();
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    static void SigabrtHandler(i32);
    static void SigfpeHandler(i32);
    static void SigintHandler(i32);
    static void SigillHandler(i32);
    static void SigsegvHandler(i32);
    static void SigtermHandler(i32);
};

}

#endif
