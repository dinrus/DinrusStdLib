/**
 * Copyright (c) 2016 Tino Reichardt
 * All rights reserved.
 *
 * You can contact the author at:
 * - zstdmt source repository: https://github.com/mcmilk/zstdmt
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

/**
 * This file will hold wrapper for systems, which do not support pthreads
 */

#include "threading.h"

/* create fake symbol to avoid empty translation unit warning */
i32 g_ZSTD_threading_useless_symbol;

#if defined(ZSTD_MULTITHREAD) && defined(_WIN32)

/**
 * Windows minimalist Pthread Wrapper, based on :
 * http://www.cse.wustl.edu/~schmidt/win32-cv-1.html
 */


/* ===  Dependencies  === */
#include <process.h>
#include <errno.h>


/* ===  Implementation  === */

static u32 __stdcall worker(uk arg)
{
    ZSTD_pthread_t* const thread = (ZSTD_pthread_t*) arg;
    thread->arg = thread->start_routine(thread->arg);
    return 0;
}

i32 ZSTD_pthread_create(ZSTD_pthread_t* thread, ukk unused,
            uk (*start_routine) (uk), uk arg)
{
    (void)unused;
    thread->arg = arg;
    thread->start_routine = start_routine;
    thread->handle = (HANDLE) _beginthreadex(NULL, 0, worker, thread, 0, NULL);

    if (!thread->handle)
        return errno;
    else
        return 0;
}

i32 ZSTD_pthread_join(ZSTD_pthread_t thread, uk *value_ptr)
{
    DWORD result;

    if (!thread.handle) return 0;

    result = WaitForSingleObject(thread.handle, INFINITE);
    switch (result) {
    case WAIT_OBJECT_0:
        if (value_ptr) *value_ptr = thread.arg;
        return 0;
    case WAIT_ABANDONED:
        return EINVAL;
    default:
        return GetLastError();
    }
}

#endif   /* ZSTD_MULTITHREAD */

#if defined(ZSTD_MULTITHREAD) && DEBUGLEVEL >= 1 && !defined(_WIN32)

#include <stdlib.h>

i32 ZSTD_pthread_mutex_init(ZSTD_pthread_mutex_t* mutex, pthread_mutexattr_t const* attr)
{
    *mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!*mutex)
        return 1;
    return pthread_mutex_init(*mutex, attr);
}

i32 ZSTD_pthread_mutex_destroy(ZSTD_pthread_mutex_t* mutex)
{
    if (!*mutex)
        return 0;
    {
        i32 const ret = pthread_mutex_destroy(*mutex);
        free(*mutex);
        return ret;
    }
}

i32 ZSTD_pthread_cond_init(ZSTD_pthread_cond_t* cond, pthread_condattr_t const* attr)
{
    *cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    if (!*cond)
        return 1;
    return pthread_cond_init(*cond, attr);
}

i32 ZSTD_pthread_cond_destroy(ZSTD_pthread_cond_t* cond)
{
    if (!*cond)
        return 0;
    {
        i32 const ret = pthread_cond_destroy(*cond);
        free(*cond);
        return ret;
    }
}

#endif
