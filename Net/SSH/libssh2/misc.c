/* Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2019 by Daniel Stenberg
 * Copyright (c) 2010  Simon Josefsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   Redistributions of source code must retain the above
 *   copyright notice, this list of conditions and the
 *   following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names
 *   of any other contributors may be used to endorse or
 *   promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include "libssh2_priv.h"
#include "misc.h"
#include "blf.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if defined(HAVE_DECL_SECUREZEROMEMORY) && HAVE_DECL_SECUREZEROMEMORY
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#endif

#include <stdio.h>
#include <errno.h>

i32 _libssh2_error_flags(LIBSSH2_SESSION* session, i32 errcode,
                         tukk errmsg, i32 errflags)
{
    if(session->err_flags & LIBSSH2_ERR_FLAG_DUP)
        LIBSSH2_FREE(session, (char *)session->err_msg);

    session->err_code = errcode;
    session->err_flags = 0;

    if((errmsg != NULL) && ((errflags & LIBSSH2_ERR_FLAG_DUP) != 0)) {
        size_t len = strlen(errmsg);
        char *copy = LIBSSH2_ALLOC(session, len + 1);
        if(copy) {
            memcpy(copy, errmsg, len + 1);
            session->err_flags = LIBSSH2_ERR_FLAG_DUP;
            session->err_msg = copy;
        }
        else
            /* Out of memory: this code path is very unlikely */
            session->err_msg = "former error forgotten (OOM)";
    }
    else
        session->err_msg = errmsg;

#ifdef LIBSSH2DEBUG
    if((errcode == LIBSSH2_ERROR_EAGAIN) && !session->api_block_mode)
        /* if this is EAGAIN and we're in non-blocking mode, don't generate
           a debug output for this */
        return errcode;
    _libssh2_debug(session, LIBSSH2_TRACE_ERROR, "%d - %s", session->err_code,
                   session->err_msg);
#endif

    return errcode;
}

i32 _libssh2_error(LIBSSH2_SESSION* session, i32 errcode, tukk errmsg)
{
    return _libssh2_error_flags(session, errcode, errmsg, 0);
}

#ifdef WIN32
static i32 wsa2errno(void)
{
    switch(WSAGetLastError()) {
    case WSAEWOULDBLOCK:
        return EAGAIN;

    case WSAENOTSOCK:
        return EBADF;

    case WSAEINTR:
        return EINTR;

    default:
        /* It is most important to ensure errno does not stay at EAGAIN
         * when a different error occurs so just set errno to a generic
         * error */
        return EIO;
    }
}
#endif

/* _libssh2_recv
 *
 * Replacement for the standard recv, return -errno on failure.
 */
ssize_t
_libssh2_recv(libssh2_socket_t sock, uk buffer, size_t length,
              i32 flags, uk *abstract)
{
    ssize_t rc;

    (void) abstract;

    rc = recv(sock, buffer, length, flags);
#ifdef WIN32
    if(rc < 0)
        return -wsa2errno();
#else
    if(rc < 0) {
        /* Sometimes the first recv() function call sets errno to ENOENT on
           Solaris and HP-UX */
        if(errno == ENOENT)
            return -EAGAIN;
#ifdef EWOULDBLOCK /* For VMS and other special unixes */
        else if(errno == EWOULDBLOCK)
          return -EAGAIN;
#endif
        else
            return -errno;
    }
#endif
    return rc;
}

/* _libssh2_send
 *
 * Replacement for the standard send, return -errno on failure.
 */
ssize_t
_libssh2_send(libssh2_socket_t sock, ukk buffer, size_t length,
              i32 flags, uk *abstract)
{
    ssize_t rc;

    (void) abstract;

    rc = send(sock, buffer, length, flags);
#ifdef WIN32
    if(rc < 0)
        return -wsa2errno();
#else
    if(rc < 0) {
#ifdef EWOULDBLOCK /* For VMS and other special unixes */
      if(errno == EWOULDBLOCK)
        return -EAGAIN;
#endif
      return -errno;
    }
#endif
    return rc;
}

/* libssh2_ntohu32
 */
u32
_libssh2_ntohu32(u8k *buf)
{
    return (((u32)buf[0] << 24)
           | ((u32)buf[1] << 16)
           | ((u32)buf[2] << 8)
           | ((u32)buf[3]));
}


/* _libssh2_ntohu64
 */
libssh2_uint64_t
_libssh2_ntohu64(u8k *buf)
{
    u64 msl, lsl;

    msl = ((libssh2_uint64_t)buf[0] << 24) | ((libssh2_uint64_t)buf[1] << 16)
        | ((libssh2_uint64_t)buf[2] << 8) | (libssh2_uint64_t)buf[3];
    lsl = ((libssh2_uint64_t)buf[4] << 24) | ((libssh2_uint64_t)buf[5] << 16)
        | ((libssh2_uint64_t)buf[6] << 8) | (libssh2_uint64_t)buf[7];

    return ((libssh2_uint64_t)msl <<32) | lsl;
}

/* _libssh2_htonu32
 */
void
_libssh2_htonu32(u8 *buf, uint32_t value)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
}

/* _libssh2_store_u32
 */
void _libssh2_store_u32(u8 **buf, uint32_t value)
{
    _libssh2_htonu32(*buf, value);
    *buf += sizeof(uint32_t);
}

/* _libssh2_store_str
 */
void _libssh2_store_str(u8 **buf, tukk str, size_t len)
{
    _libssh2_store_u32(buf, (uint32_t)len);
    if(len) {
        memcpy(*buf, str, len);
        *buf += len;
    }
}

/* Base64 Conversion */

static const short base64_reverse_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/* libssh2_base64_decode
 *
 * Decode a base64 chunk and store it into a newly alloc'd buffer
 */
LIBSSH2_API i32
libssh2_base64_decode(LIBSSH2_SESSION *session, char **data,
                      u32 *datalen, tukk src,
                      u32 src_len)
{
    u8 *s, *d;
    short v;
    i32 i = 0, len = 0;

    *data = LIBSSH2_ALLOC(session, (3 * src_len / 4) + 1);
    d = (u8 *) *data;
    if(!d) {
        return _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for base64 decoding");
    }

    for(s = (u8 *) src; ((char *) s) < (src + src_len); s++) {
        v = base64_reverse_table[*s];
        if(v < 0)
            continue;
        switch(i % 4) {
        case 0:
            d[len] = (u8)(v << 2);
            break;
        case 1:
            d[len++] |= v >> 4;
            d[len] = (u8)(v << 4);
            break;
        case 2:
            d[len++] |= v >> 2;
            d[len] = (u8)(v << 6);
            break;
        case 3:
            d[len++] |= v;
            break;
        }
        i++;
    }
    if((i % 4) == 1) {
        /* Invalid -- We have a u8 which belongs exclusively to a partial
           octet */
        LIBSSH2_FREE(session, *data);
        *data = NULL;
        return _libssh2_error(session, LIBSSH2_ERROR_INVAL, "Неверное base64");
    }

    *datalen = len;
    return 0;
}

/* ---- Base64 Encoding/Decoding Table --- */
static const char table64[]=
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * _libssh2_base64_encode()
 *
 * Returns the length of the newly created base64 string. The third argument
 * is a pointer to an allocated area holding the base64 data. If something
 * went wrong, 0 is returned.
 *
 */
size_t _libssh2_base64_encode(LIBSSH2_SESSION *session,
                              tukk inp, size_t insize, char **outptr)
{
    u8 ibuf[3];
    u8 obuf[4];
    i32 i;
    i32 inputparts;
    char *output;
    char *base64data;
    tukk indata = inp;

    *outptr = NULL; /* set to NULL in case of failure before we reach the
                       end */

    if(0 == insize)
        insize = strlen(indata);

    base64data = output = LIBSSH2_ALLOC(session, insize * 4 / 3 + 4);
    if(NULL == output)
        return 0;

    while(insize > 0) {
        for(i = inputparts = 0; i < 3; i++) {
            if(insize > 0) {
                inputparts++;
                ibuf[i] = *indata;
                indata++;
                insize--;
            }
            else
                ibuf[i] = 0;
        }

        obuf[0] = (u8)  ((ibuf[0] & 0xFC) >> 2);
        obuf[1] = (u8) (((ibuf[0] & 0x03) << 4) | \
                                   ((ibuf[1] & 0xF0) >> 4));
        obuf[2] = (u8) (((ibuf[1] & 0x0F) << 2) | \
                                   ((ibuf[2] & 0xC0) >> 6));
        obuf[3] = (u8)   (ibuf[2] & 0x3F);

        switch(inputparts) {
        case 1: /* only one u8 read */
            snprintf(output, 5, "%c%c==",
                     table64[obuf[0]],
                     table64[obuf[1]]);
            break;
        case 2: /* two bytes read */
            snprintf(output, 5, "%c%c%c=",
                     table64[obuf[0]],
                     table64[obuf[1]],
                     table64[obuf[2]]);
            break;
        default:
            snprintf(output, 5, "%c%c%c%c",
                     table64[obuf[0]],
                     table64[obuf[1]],
                     table64[obuf[2]],
                     table64[obuf[3]]);
            break;
        }
        output += 4;
    }
    *output = 0;
    *outptr = base64data; /* make it return the actual data memory */

    return strlen(base64data); /* return the length of the new data */
}
/* ---- End of Base64 Encoding ---- */

LIBSSH2_API void
libssh2_free(LIBSSH2_SESSION *session, uk ptr)
{
    LIBSSH2_FREE(session, ptr);
}

#ifdef LIBSSH2DEBUG
#include <stdarg.h>

LIBSSH2_API i32
libssh2_trace(LIBSSH2_SESSION * session, i32 bitmask)
{
    session->showmask = bitmask;
    return 0;
}

LIBSSH2_API i32
libssh2_trace_sethandler(LIBSSH2_SESSION *session, uk handler_context,
                         libssh2_trace_handler_func callback)
{
    session->tracehandler = callback;
    session->tracehandler_context = handler_context;
    return 0;
}

void
_libssh2_debug(LIBSSH2_SESSION * session, i32 context, tukk format, ...)
{
    char buffer[1536];
    i32 len, msglen, buflen = sizeof(buffer);
    va_list vargs;
    struct timeval now;
    static i32 firstsec;
    static tukk const contexts[] = {
        "Unknown",
        "Transport",
        "Key Ex",
        "Userauth",
        "Conn",
        "SCP",
        "SFTP",
        "Failure Event",
        "Publickey",
        "Socket",
    };
    tukk contexttext = contexts[0];
    u32 contextindex;

    if(!(session->showmask & context)) {
        /* no such output asked for */
        return;
    }

    /* Find the first matching context string for this message */
    for(contextindex = 0; contextindex < ARRAY_SIZE(contexts);
         contextindex++) {
        if((context & (1 << contextindex)) != 0) {
            contexttext = contexts[contextindex];
            break;
        }
    }

    _libssh2_gettimeofday(&now, NULL);
    if(!firstsec) {
        firstsec = now.tv_sec;
    }
    now.tv_sec -= firstsec;

    len = snprintf(buffer, buflen, "[libssh2] %d.%06d %s: ",
                   (i32)now.tv_sec, (i32)now.tv_usec, contexttext);

    if(len >= buflen)
        msglen = buflen - 1;
    else {
        buflen -= len;
        msglen = len;
        va_start(vargs, format);
        len = vsnprintf(buffer + msglen, buflen, format, vargs);
        va_end(vargs);
        msglen += len < buflen ? len : buflen - 1;
    }

    if(session->tracehandler)
        (session->tracehandler)(session, session->tracehandler_context, buffer,
                                msglen);
    else
        fprintf(stderr, "%s\n", buffer);
}

#else
LIBSSH2_API i32
libssh2_trace(LIBSSH2_SESSION * session, i32 bitmask)
{
    (void) session;
    (void) bitmask;
    return 0;
}

LIBSSH2_API i32
libssh2_trace_sethandler(LIBSSH2_SESSION *session, uk handler_context,
                         libssh2_trace_handler_func callback)
{
    (void) session;
    (void) handler_context;
    (void) callback;
    return 0;
}
#endif

/* init the list head */
void _libssh2_list_init(struct list_head *head)
{
    head->first = head->last = NULL;
}

/* add a node to the list */
void _libssh2_list_add(struct list_head *head,
                       struct list_node *entry)
{
    /* store a pointer to the head */
    entry->head = head;

    /* we add this entry at the "top" so it has no next */
    entry->next = NULL;

    /* make our prev point to what the head thinks is last */
    entry->prev = head->last;

    /* and make head's last be us now */
    head->last = entry;

    /* make sure our 'prev' node points to us next */
    if(entry->prev)
        entry->prev->next = entry;
    else
        head->first = entry;
}

/* return the "first" node in the list this head points to */
uk _libssh2_list_first(struct list_head *head)
{
    return head->first;
}

/* return the next node in the list */
uk _libssh2_list_next(struct list_node *node)
{
    return node->next;
}

/* return the prev node in the list */
uk _libssh2_list_prev(struct list_node *node)
{
    return node->prev;
}

/* remove this node from the list */
void _libssh2_list_remove(struct list_node *entry)
{
    if(entry->prev)
        entry->prev->next = entry->next;
    else
        entry->head->first = entry->next;

    if(entry->next)
        entry->next->prev = entry->prev;
    else
        entry->head->last = entry->prev;
}

#if 0
/* insert a node before the given 'after' entry */
void _libssh2_list_insert(struct list_node *after, /* insert before this */
                          struct list_node *entry)
{
    /* 'after' is next to 'entry' */
    bentry->next = after;

    /* entry's prev is then made to be the prev after current has */
    entry->prev = after->prev;

    /* the node that is now before 'entry' was previously before 'after'
       and must be made to point to 'entry' correctly */
    if(entry->prev)
        entry->prev->next = entry;
    else
      /* there was no node before this, so we make sure we point the head
         pointer to this node */
      after->head->first = entry;

    /* after's prev entry points back to entry */
    after->prev = entry;

    /* after's next entry is still the same as before */

    /* entry's head is the same as after's */
    entry->head = after->head;
}

#endif

/* this define is defined in misc.h for the correct platforms */
#ifdef LIBSSH2_GETTIMEOFDAY_WIN32
/*
 * gettimeofday
 * Implementation according to:
 * The Open Group Base Specifications Issue 6
 * IEEE Std 1003.1, 2004 Edition
 */

/*
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Contributed by:
 *  Danny Smith <dannysmith@users.sourceforge.net>
 */

/* Offset between 1/1/1601 and 1/1/1970 in 100 nanosec units */
#define _W32_FT_OFFSET (116444736000000000)

i32 __cdecl _libssh2_gettimeofday(struct timeval *tp, uk tzp)
{
    union {
        unsigned __int64 ns100; /*time since 1 Jan 1601 in 100ns units */
        FILETIME ft;
    } _now;
    (void)tzp;
    if(tp) {
        GetSystemTimeAsFileTime(&_now.ft);
        tp->tv_usec = (long)((_now.ns100 / 10) % 1000000);
        tp->tv_sec = (long)((_now.ns100 - _W32_FT_OFFSET) / 10000000);
    }
    /* Always return 0 as per Open Group Base Specifications Issue 6.
       Do not set errno on error.  */
    return 0;
}


#endif

uk _libssh2_calloc(LIBSSH2_SESSION* session, size_t size)
{
    uk p = LIBSSH2_ALLOC(session, size);
    if(p) {
        memset(p, 0, size);
    }
    return p;
}

/* XOR operation on buffers input1 and input2, result in output.
   It is safe to use an input buffer as the output buffer. */
void _libssh2_xor_data(u8 *output,
                       u8k *input1,
                       u8k *input2,
                       size_t length)
{
    size_t i;

    for(i = 0; i < length; i++)
        *output++ = *input1++ ^ *input2++;
}

/* Increments an AES CTR buffer to prepare it for use with the
   next AES block. */
void _libssh2_aes_ctr_increment(u8 *ctr,
                                size_t length)
{
    u8 *pc;
    u32 val, carry;

    pc = ctr + length - 1;
    carry = 1;

    while(pc >= ctr) {
        val = (u32)*pc + carry;
        *pc-- = val & 0xFF;
        carry = val >> 8;
    }
}

#ifdef WIN32
static uk  (__cdecl * const volatile memset_libssh)(uk , i32, size_t) =
    memset;
#else
static uk  (* const volatile memset_libssh)(uk , i32, size_t) = memset;
#endif

void _libssh2_explicit_zero(uk buf, size_t size)
{
#if defined(HAVE_DECL_SECUREZEROMEMORY) && HAVE_DECL_SECUREZEROMEMORY
    SecureZeroMemory(buf, size);
    (void)memset_libssh; /* Silence unused variable warning */
#elif defined(HAVE_MEMSET_S)
    (void)memset_s(buf, size, 0, size);
    (void)memset_libssh; /* Silence unused variable warning */
#else
    memset_libssh(buf, 0, size);
#endif
}

/* Txt buffer */

struct string_buf* _libssh2_string_buf_new(LIBSSH2_SESSION *session)
{
    struct string_buf *ret;

    ret = _libssh2_calloc(session, sizeof(*ret));
    if(ret == NULL)
        return NULL;

    return ret;
}

void _libssh2_string_buf_free(LIBSSH2_SESSION *session, struct string_buf *buf)
{
    if(buf == NULL)
        return;

    if(buf->data != NULL)
        LIBSSH2_FREE(session, buf->data);

    LIBSSH2_FREE(session, buf);
    buf = NULL;
}

i32 _libssh2_get_u32(struct string_buf *buf, uint32_t *out)
{
    if(!_libssh2_check_length(buf, 4)) {
        return -1;
    }

    *out = _libssh2_ntohu32(buf->dataptr);
    buf->dataptr += 4;
    return 0;
}

i32 _libssh2_get_u64(struct string_buf *buf, libssh2_uint64_t *out)
{
    if(!_libssh2_check_length(buf, 8)) {
        return -1;
    }

    *out = _libssh2_ntohu64(buf->dataptr);
    buf->dataptr += 8;
    return 0;
}

i32 _libssh2_match_string(struct string_buf *buf, tukk match)
{
    u8 *out;
    size_t len = 0;
    if(_libssh2_get_string(buf, &out, &len) || len != strlen(match) ||
        strncmp((char *)out, match, strlen(match)) != 0) {
        return -1;
    }
    return 0;
}

i32 _libssh2_get_string(struct string_buf *buf, u8 **outbuf,
                        size_t *outlen)
{
    uint32_t data_len;
    if(_libssh2_get_u32(buf, &data_len) != 0) {
        return -1;
    }
    if(!_libssh2_check_length(buf, data_len)) {
        return -1;
    }
    *outbuf = buf->dataptr;
    buf->dataptr += data_len;

    if(outlen)
        *outlen = (size_t)data_len;

    return 0;
}

i32 _libssh2_copy_string(LIBSSH2_SESSION *session, struct string_buf *buf,
                         u8 **outbuf, size_t *outlen)
{
    size_t str_len;
    u8 *str;

    if(_libssh2_get_string(buf, &str, &str_len)) {
        return -1;
    }

    *outbuf = LIBSSH2_ALLOC(session, str_len);
    if(*outbuf) {
        memcpy(*outbuf, str, str_len);
    }
    else {
        return -1;
    }

    if(outlen)
        *outlen = str_len;

    return 0;
}

i32 _libssh2_get_bignum_bytes(struct string_buf *buf, u8 **outbuf,
                              size_t *outlen)
{
    uint32_t data_len;
    uint32_t bn_len;
    u8 *bnptr;

    if(_libssh2_get_u32(buf, &data_len)) {
        return -1;
    }
    if(!_libssh2_check_length(buf, data_len)) {
        return -1;
    }

    bn_len = data_len;
    bnptr = buf->dataptr;

    /* trim leading zeros */
    while(bn_len > 0 && *bnptr == 0x00) {
        bn_len--;
        bnptr++;
    }

    *outbuf = bnptr;
    buf->dataptr += data_len;

    if(outlen)
        *outlen = (size_t)bn_len;

    return 0;
}

/* Given the current location in buf, _libssh2_check_length ensures
   callers can read the next len number of bytes out of the buffer
   before reading the buffer content */

i32 _libssh2_check_length(struct string_buf *buf, size_t len)
{
    u8 *endp = &buf->data[buf->len];
    size_t left = endp - buf->dataptr;
    return ((len <= left) && (left <= buf->len));
}

/* Wrappers */

i32 _libssh2_bcrypt_pbkdf(tukk pass,
                          size_t passlen,
                          const uint8_t *salt,
                          size_t saltlen,
                          uint8_t *key,
                          size_t keylen,
                          u32 rounds)
{
    /* defined in bcrypt_pbkdf.c */
    return bcrypt_pbkdf(pass,
                        passlen,
                        salt,
                        saltlen,
                        key,
                        keylen,
                        rounds);
}
