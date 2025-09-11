/* ******************************************************************
 * hist : Histogram functions
 * part of Finite State Entropy project
 * Copyright (c) 2013-2020, Yann Collet, Facebook, Inc.
 *
 *  You can contact the author at :
 *  - FSE source repository : https://github.com/Cyan4973/FiniteStateEntropy
 *  - Public forum : https://groups.google.com/forum/#!forum/lz4c
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
****************************************************************** */

/* --- dependencies --- */
#include "mem.h"             /* U32, BYTE, etc. */
#include "debug.h"           /* assert, DEBUGLOG */
#include "error_private.h"   /* ERROR */
#include "hist.h"


/* --- Error management --- */
u32 HIST_isError(size_t code) { return ERR_isError(code); }

/*-**************************************************************
 *  Histogram functions
 ****************************************************************/
u32 HIST_count_simple(u32* count, u32* maxSymbolValPtr,
                           ukk src, size_t srcSize)
{
    const BYTE* ip = (const BYTE*)src;
    const BYTE* const end = ip + srcSize;
    u32 maxSymbolVal = *maxSymbolValPtr;
    u32 largestCount=0;

    memset(count, 0, (maxSymbolVal+1) * sizeof(*count));
    if (srcSize==0) { *maxSymbolValPtr = 0; return 0; }

    while (ip<end) {
        assert(*ip <= maxSymbolVal);
        count[*ip++]++;
    }

    while (!count[maxSymbolVal]) maxSymbolVal--;
    *maxSymbolValPtr = maxSymbolVal;

    {   U32 s;
        for (s=0; s<=maxSymbolVal; s++)
            if (count[s] > largestCount) largestCount = count[s];
    }

    return largestCount;
}

typedef enum { trustInput, checkMaxSymbolVal } HIST_checkInput_e;

/* HIST_count_parallel_wksp() :
 * store histogram into 4 intermediate tables, recombined at the end.
 * this design makes better use of OoO cpus,
 * and is noticeably faster when some values are heavily repeated.
 * But it needs some additional workspace for intermediate tables.
 * `workSpace` size must be a table of size >= HIST_WKSP_SIZE_U32.
 * @return : largest histogram frequency,
 *           or an error code (notably when histogram would be larger than *maxSymbolValPtr). */
static size_t HIST_count_parallel_wksp(
                                u32* count, u32* maxSymbolValPtr,
                                ukk source, size_t sourceSize,
                                HIST_checkInput_e check,
                                U32* const workSpace)
{
    const BYTE* ip = (const BYTE*)source;
    const BYTE* const iend = ip+sourceSize;
    u32 maxSymbolVal = *maxSymbolValPtr;
    u32 max=0;
    U32* const Counting1 = workSpace;
    U32* const Counting2 = Counting1 + 256;
    U32* const Counting3 = Counting2 + 256;
    U32* const Counting4 = Counting3 + 256;

    memset(workSpace, 0, 4*256*sizeof(u32));

    /* safety checks */
    if (!sourceSize) {
        memset(count, 0, maxSymbolVal + 1);
        *maxSymbolValPtr = 0;
        return 0;
    }
    if (!maxSymbolVal) maxSymbolVal = 255;            /* 0 == default */

    /* by stripes of 16 bytes */
    {   U32 cached = MEM_read32(ip); ip += 4;
        while (ip < iend-15) {
            U32 c = cached; cached = MEM_read32(ip); ip += 4;
            Counting1[(BYTE) c     ]++;
            Counting2[(BYTE)(c>>8) ]++;
            Counting3[(BYTE)(c>>16)]++;
            Counting4[       c>>24 ]++;
            c = cached; cached = MEM_read32(ip); ip += 4;
            Counting1[(BYTE) c     ]++;
            Counting2[(BYTE)(c>>8) ]++;
            Counting3[(BYTE)(c>>16)]++;
            Counting4[       c>>24 ]++;
            c = cached; cached = MEM_read32(ip); ip += 4;
            Counting1[(BYTE) c     ]++;
            Counting2[(BYTE)(c>>8) ]++;
            Counting3[(BYTE)(c>>16)]++;
            Counting4[       c>>24 ]++;
            c = cached; cached = MEM_read32(ip); ip += 4;
            Counting1[(BYTE) c     ]++;
            Counting2[(BYTE)(c>>8) ]++;
            Counting3[(BYTE)(c>>16)]++;
            Counting4[       c>>24 ]++;
        }
        ip-=4;
    }

    /* finish last symbols */
    while (ip<iend) Counting1[*ip++]++;

    if (check) {   /* verify stats will fit into destination table */
        U32 s; for (s=255; s>maxSymbolVal; s--) {
            Counting1[s] += Counting2[s] + Counting3[s] + Counting4[s];
            if (Counting1[s]) return ERROR(maxSymbolVal_tooSmall);
    }   }

    {   U32 s;
        if (maxSymbolVal > 255) maxSymbolVal = 255;
        for (s=0; s<=maxSymbolVal; s++) {
            count[s] = Counting1[s] + Counting2[s] + Counting3[s] + Counting4[s];
            if (count[s] > max) max = count[s];
    }   }

    while (!count[maxSymbolVal]) maxSymbolVal--;
    *maxSymbolValPtr = maxSymbolVal;
    return (size_t)max;
}

/* HIST_countFast_wksp() :
 * Same as HIST_countFast(), but using an externally provided scratch buffer.
 * `workSpace` is a writable buffer which must be 4-bytes aligned,
 * `workSpaceSize` must be >= HIST_WKSP_SIZE
 */
size_t HIST_countFast_wksp(u32* count, u32* maxSymbolValPtr,
                          ukk source, size_t sourceSize,
                          uk workSpace, size_t workSpaceSize)
{
    if (sourceSize < 1500) /* heuristic threshold */
        return HIST_count_simple(count, maxSymbolValPtr, source, sourceSize);
    if ((size_t)workSpace & 3) return ERROR(GENERIC);  /* must be aligned on 4-bytes boundaries */
    if (workSpaceSize < HIST_WKSP_SIZE) return ERROR(workSpace_tooSmall);
    return HIST_count_parallel_wksp(count, maxSymbolValPtr, source, sourceSize, trustInput, (U32*)workSpace);
}

/* fast variant (unsafe : won't check if src contains values beyond count[] limit) */
size_t HIST_countFast(u32* count, u32* maxSymbolValPtr,
                     ukk source, size_t sourceSize)
{
    u32 tmpCounters[HIST_WKSP_SIZE_U32];
    return HIST_countFast_wksp(count, maxSymbolValPtr, source, sourceSize, tmpCounters, sizeof(tmpCounters));
}

/* HIST_count_wksp() :
 * Same as HIST_count(), but using an externally provided scratch buffer.
 * `workSpace` size must be table of >= HIST_WKSP_SIZE_U32 u32 */
size_t HIST_count_wksp(u32* count, u32* maxSymbolValPtr,
                       ukk source, size_t sourceSize,
                       uk workSpace, size_t workSpaceSize)
{
    if ((size_t)workSpace & 3) return ERROR(GENERIC);  /* must be aligned on 4-bytes boundaries */
    if (workSpaceSize < HIST_WKSP_SIZE) return ERROR(workSpace_tooSmall);
    if (*maxSymbolValPtr < 255)
        return HIST_count_parallel_wksp(count, maxSymbolValPtr, source, sourceSize, checkMaxSymbolVal, (U32*)workSpace);
    *maxSymbolValPtr = 255;
    return HIST_countFast_wksp(count, maxSymbolValPtr, source, sourceSize, workSpace, workSpaceSize);
}

size_t HIST_count(u32* count, u32* maxSymbolValPtr,
                 ukk src, size_t srcSize)
{
    u32 tmpCounters[HIST_WKSP_SIZE_U32];
    return HIST_count_wksp(count, maxSymbolValPtr, src, srcSize, tmpCounters, sizeof(tmpCounters));
}
