// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "SysInfo.h"

namespace drx {

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

#if defined(__MINGW32__) && !defined (__MINGW64__)

zu64 __rdtsc() {
/*  #if defined(__MINGW64__)    // Now it is unnecessary
        u32 lo, hi;
        __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
        return zu64(lo) | (zu64(hi) << 32);
    #else*/
        u32 lo, hi;
        __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
        return zu64(lo) | (zu64(hi) << 32);
//  #endif
}

#endif


i32 GetCpuSpeed()
{
    LARGE_INTEGER nFreq, nCtrStop, nCtr;
    if(!QueryPerformanceFrequency(&nFreq))
        return 0;
    QueryPerformanceCounter(&nCtrStop);
    nCtrStop.QuadPart += nFreq.QuadPart;

    unsigned __z64 start = __rdtsc();
    do {
        QueryPerformanceCounter(&nCtr);
    } while (nCtr.QuadPart < nCtrStop.QuadPart);
    unsigned __z64 end = __rdtsc();
    return i32((end - start)/1000000);
}

#endif

#if defined(TARGET_POSIX)

#define __rdtsc(tm) __asm__ __volatile__ (".byte 0x0f; .byte 0x31" :"=a" (tm))
#define COUNT_SEC   (double)tv.tv_sec + (1.e-6)*tv.tv_usec

i32 GetCpuSpeed()
{
    struct timeval tv;
    double cnt1, cnt2;
    u64 start, end;

    __rdtsc(start);
    gettimeofday(&tv, 0);
    cnt1 = COUNT_SEC + 0.01;

    do {
        gettimeofday(&tv, 0);
        cnt2 = COUNT_SEC;
    } while(cnt2 < cnt1);

    __rdtsc(end);

    return i32((end-start)/10000);
}

#endif

}