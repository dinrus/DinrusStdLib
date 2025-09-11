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
#include <drxtypes.h>   /* size_t */


/* --- simple histogram functions --- */

/*! HIST_count():
 *  Provides the precise count of each u8 within a table 'count'.
 * 'count' is a table of u32, of minimum size (*maxSymbolValPtr+1).
 *  Updates *maxSymbolValPtr with actual largest symbol value detected.
 * @return : count of the most frequent symbol (which isn't identified).
 *           or an error code, which can be tested using HIST_isError().
 *           note : if return == srcSize, there is only one symbol.
 */
size_t HIST_count(u32* count, u32* maxSymbolValPtr,
                  ukk src, size_t srcSize);

u32 HIST_isError(size_t code);  /**< tells if a return value is an error code */


/* --- advanced histogram functions --- */

#define HIST_WKSP_SIZE_U32 1024
#define HIST_WKSP_SIZE    (HIST_WKSP_SIZE_U32 * sizeof(u32))
/** HIST_count_wksp() :
 *  Same as HIST_count(), but using an externally provided scratch buffer.
 *  Benefit is this function will use very little stack space.
 * `workSpace` is a writable buffer which must be 4-bytes aligned,
 * `workSpaceSize` must be >= HIST_WKSP_SIZE
 */
size_t HIST_count_wksp(u32* count, u32* maxSymbolValPtr,
                       ukk src, size_t srcSize,
                       uk workSpace, size_t workSpaceSize);

/** HIST_countFast() :
 *  same as HIST_count(), but blindly trusts that all u8 values within src are <= *maxSymbolValPtr.
 *  This function is unsafe, and will segfault if any value within `src` is `> *maxSymbolValPtr`
 */
size_t HIST_countFast(u32* count, u32* maxSymbolValPtr,
                      ukk src, size_t srcSize);

/** HIST_countFast_wksp() :
 *  Same as HIST_countFast(), but using an externally provided scratch buffer.
 * `workSpace` is a writable buffer which must be 4-bytes aligned,
 * `workSpaceSize` must be >= HIST_WKSP_SIZE
 */
size_t HIST_countFast_wksp(u32* count, u32* maxSymbolValPtr,
                           ukk src, size_t srcSize,
                           uk workSpace, size_t workSpaceSize);

/*! HIST_count_simple() :
 *  Same as HIST_countFast(), this function is unsafe,
 *  and will segfault if any value within `src` is `> *maxSymbolValPtr`.
 *  It is also a bit slower for large inputs.
 *  However, it does not need any additional memory (not even on stack).
 * @return : count of the most frequent symbol.
 *  Note this function doesn't produce any error (i.e. it must succeed).
 */
u32 HIST_count_simple(u32* count, u32* maxSymbolValPtr,
                           ukk src, size_t srcSize);
