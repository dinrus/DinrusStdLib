#ifndef __LIBSSH2_PRIV_H
#define __LIBSSH2_PRIV_H
/* Copyright (c) 2004-2008, 2010, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009-2014 by Daniel Stenberg
 * Copyright (c) 2010 Simon Josefsson
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

#define LIBSSH2_LIBRARY
#include "libssh2_config.h"

#ifdef HAVE_WINDOWS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif

#include <stdio.h>
#include <time.h>

/* The following CPP block should really only be in session.c and packet.c.
   However, AIX have #define's for 'events' and 'revents' and we are using
   those names in libssh2.h, so we need to include the AIX headers first, to
   make sure all code is compiled with consistent names of these fields.
   While arguable the best would to change libssh2.h to use other names, that
   would break backwards compatibility.
*/
#ifdef HAVE_POLL
# include <poll.h>
#else
# if defined(HAVE_SELECT) && !defined(WIN32)
# ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
# else
# include <sys/time.h>
# include <sys/types.h>
# endif
# endif
#endif

/* Needed for struct iovec on some platforms */
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "libssh2.h"
#include "libssh2_publickey.h"
#include "libssh2_sftp.h"
#include "misc.h" /* for the linked list stuff */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifdef _MSC_VER
/* "inline" keyword is valid only with C++ engine! */
#define inline __inline
#endif

/* 3DS doesn't seem to have iovec */
#if defined(WIN32) || defined(_3DS)

struct iovec {
    size_t iov_len;
    uk iov_base;
};

#endif

/* Provide iovec / writev on WIN32 platform. */
#ifdef WIN32

static inline i32 writev(i32 sock, struct iovec *iov, i32 nvecs)
{
    DWORD ret;
    if(WSASend(sock, (LPWSABUF)iov, nvecs, &ret, 0, NULL, NULL) == 0) {
        return ret;
    }
    return -1;
}

#endif /* WIN32 */

#ifdef __OS400__
/* Force parameter type. */
#define send(s, b, l, f)    send((s), (u8 *) (b), (l), (f))
#endif

#include "crypto.h"

#ifdef HAVE_WINSOCK2_H

#include <winsock2.h>
#include <ws2tcpip.h>

#endif

#ifndef SIZE_MAX
#if _WIN64
#define SIZE_MAX 0xFFFFFFFFFFFFFFFF
#else
#define SIZE_MAX 0xFFFFFFFF
#endif
#endif

#ifndef UINT_MAX
#define UINT_MAX 0xFFFFFFFF
#endif

/* RFC4253 section 6.1 Maximum Packet Length says:
 *
 * "All implementations MUST be able to process packets with
 * uncompressed payload length of 32768 bytes or less and
 * total packet size of 35000 bytes or less (including length,
 * padding length, payload, padding, and MAC.)."
 */
#define MAX_SSH_PACKET_LEN 35000
#define MAX_SHA_DIGEST_LEN SHA512_DIGEST_LENGTH

#define LIBSSH2_ALLOC(session, count) \
  session->alloc((count), &(session)->abstract)
#define LIBSSH2_CALLOC(session, count) _libssh2_calloc(session, count)
#define LIBSSH2_REALLOC(session, ptr, count) \
 ((ptr) ? session->realloc((ptr), (count), &(session)->abstract) : \
 session->alloc((count), &(session)->abstract))
#define LIBSSH2_FREE(session, ptr) \
 session->free((ptr), &(session)->abstract)
#define LIBSSH2_IGNORE(session, data, datalen) \
 session->ssh_msg_ignore((session), (data), (datalen), &(session)->abstract)
#define LIBSSH2_DEBUG(session, always_display, message, message_len, \
                      language, language_len)    \
    session->ssh_msg_debug((session), (always_display), (message), \
                           (message_len), (language), (language_len), \
                           &(session)->abstract)
#define LIBSSH2_DISCONNECT(session, reason, message, message_len, \
                           language, language_len)                \
    session->ssh_msg_disconnect((session), (reason), (message),   \
                                (message_len), (language), (language_len), \
                                &(session)->abstract)

#define LIBSSH2_MACERROR(session, data, datalen)         \
    session->macerror((session), (data), (datalen), &(session)->abstract)
#define LIBSSH2_X11_OPEN(channel, shost, sport)          \
    channel->session->x11(((channel)->session), (channel), \
                          (shost), (sport), (&(channel)->session->abstract))

#define LIBSSH2_CHANNEL_CLOSE(session, channel)          \
    channel->close_cb((session), &(session)->abstract, \
                      (channel), &(channel)->abstract)

#define LIBSSH2_SEND_FD(session, fd, buffer, length, flags) \
    (session->send)(fd, buffer, length, flags, &session->abstract)
#define LIBSSH2_RECV_FD(session, fd, buffer, length, flags) \
    (session->recv)(fd, buffer, length, flags, &session->abstract)

#define LIBSSH2_SEND(session, buffer, length, flags)  \
    LIBSSH2_SEND_FD(session, session->socket_fd, buffer, length, flags)
#define LIBSSH2_RECV(session, buffer, length, flags)                    \
    LIBSSH2_RECV_FD(session, session->socket_fd, buffer, length, flags)

typedef struct _LIBSSH2_KEX_METHOD LIBSSH2_KEX_METHOD;
typedef struct _LIBSSH2_HOSTKEY_METHOD LIBSSH2_HOSTKEY_METHOD;
typedef struct _LIBSSH2_CRYPT_METHOD LIBSSH2_CRYPT_METHOD;
typedef struct _LIBSSH2_COMP_METHOD LIBSSH2_COMP_METHOD;

typedef struct _LIBSSH2_PACKET LIBSSH2_PACKET;

typedef enum
{
    libssh2_NB_state_idle = 0,
    libssh2_NB_state_allocated,
    libssh2_NB_state_created,
    libssh2_NB_state_sent,
    libssh2_NB_state_sent1,
    libssh2_NB_state_sent2,
    libssh2_NB_state_sent3,
    libssh2_NB_state_sent4,
    libssh2_NB_state_sent5,
    libssh2_NB_state_sent6,
    libssh2_NB_state_sent7,
    libssh2_NB_state_jump1,
    libssh2_NB_state_jump2,
    libssh2_NB_state_jump3,
    libssh2_NB_state_jump4,
    libssh2_NB_state_jump5,
    libssh2_NB_state_end
} libssh2_nonblocking_states;

typedef struct packet_require_state_t
{
    libssh2_nonblocking_states state;
    time_t start;
} packet_require_state_t;

typedef struct packet_requirev_state_t
{
    time_t start;
} packet_requirev_state_t;

typedef struct kmdhgGPshakex_state_t
{
    libssh2_nonblocking_states state;
    u8 *e_packet;
    u8 *s_packet;
    u8 *tmp;
    u8 h_sig_comp[MAX_SHA_DIGEST_LEN];
    u8 c;
    size_t e_packet_len;
    size_t s_packet_len;
    size_t tmp_len;
    _libssh2_bn_ctx *ctx;
    _libssh2_dh_ctx x;
    _libssh2_bn *e;
    _libssh2_bn *f;
    _libssh2_bn *k;
    u8 *f_value;
    u8 *k_value;
    u8 *h_sig;
    size_t f_value_len;
    size_t k_value_len;
    size_t h_sig_len;
    uk exchange_hash;
    packet_require_state_t req_state;
    libssh2_nonblocking_states burn_state;
} kmdhgGPshakex_state_t;

typedef struct key_exchange_state_low_t
{
    libssh2_nonblocking_states state;
    packet_require_state_t req_state;
    kmdhgGPshakex_state_t exchange_state;
    _libssh2_bn *p;             /* SSH2 defined value (p_value) */
    _libssh2_bn *g;             /* SSH2 defined value (2) */
    u8 request[256]; /* Must fit EC_MAX_POINT_LEN + data */
    u8 *data;
    size_t request_len;
    size_t data_len;
    _libssh2_ec_key *private_key;   /* SSH2 ecdh private key */
    u8 *public_key_oct;  /* SSH2 ecdh public key octal value */
    size_t public_key_oct_len;      /* SSH2 ecdh public key octal value
                                       length */
    u8 *curve25519_public_key; /* curve25519 public key, 32
                                             bytes */
    u8 *curve25519_private_key; /* curve25519 private key, 32
                                              bytes */
} key_exchange_state_low_t;

typedef struct key_exchange_state_t
{
    libssh2_nonblocking_states state;
    packet_require_state_t req_state;
    key_exchange_state_low_t key_state_low;
    u8 *data;
    size_t data_len;
    u8 *oldlocal;
    size_t oldlocal_len;
} key_exchange_state_t;

#define FwdNotReq "Forward not requested"

typedef struct packet_queue_listener_state_t
{
    libssh2_nonblocking_states state;
    u8 packet[17 + (sizeof(FwdNotReq) - 1)];
    u8 *host;
    u8 *shost;
    uint32_t sender_channel;
    uint32_t initial_window_size;
    uint32_t packet_size;
    uint32_t port;
    uint32_t sport;
    uint32_t host_len;
    uint32_t shost_len;
    LIBSSH2_CHANNEL *channel;
} packet_queue_listener_state_t;

#define X11FwdUnAvil "X11 Forward Unavailable"

typedef struct packet_x11_open_state_t
{
    libssh2_nonblocking_states state;
    u8 packet[17 + (sizeof(X11FwdUnAvil) - 1)];
    u8 *shost;
    uint32_t sender_channel;
    uint32_t initial_window_size;
    uint32_t packet_size;
    uint32_t sport;
    uint32_t shost_len;
    LIBSSH2_CHANNEL *channel;
} packet_x11_open_state_t;

struct _LIBSSH2_PACKET
{
    struct list_node node; /* linked list header */

    /* the raw unencrypted payload */
    u8 *data;
    size_t data_len;

    /* Where to start reading data from,
     * used for channel data that's been partially consumed */
    size_t data_head;
};

typedef struct _libssh2_channel_data
{
    /* Identifier */
    uint32_t id;

    /* Limits and restrictions */
    uint32_t window_size_initial, window_size, packet_size;

    /* Set to 1 when CHANNEL_CLOSE / CHANNEL_EOF sent/received */
    char close, eof, extended_data_ignore_mode;
} libssh2_channel_data;

struct _LIBSSH2_CHANNEL
{
    struct list_node node;

    u8 *channel_type;
    unsigned channel_type_len;

    /* channel's program exit status */
    i32 exit_status;

    /* channel's program exit signal (without the SIG prefix) */
    char *exit_signal;

    libssh2_channel_data local, remote;
    /* Amount of bytes to be refunded to receive window (but not yet sent) */
    uint32_t adjust_queue;
    /* Data immediately available for reading */
    uint32_t read_avail;

    LIBSSH2_SESSION *session;

    uk abstract;
      LIBSSH2_CHANNEL_CLOSE_FUNC((*close_cb));

    /* State variables used in libssh2_channel_setenv_ex() */
    libssh2_nonblocking_states setenv_state;
    u8 *setenv_packet;
    size_t setenv_packet_len;
    u8 setenv_local_channel[4];
    packet_requirev_state_t setenv_packet_requirev_state;

    /* State variables used in libssh2_channel_request_pty_ex()
       libssh2_channel_request_pty_size_ex() */
    libssh2_nonblocking_states reqPTY_state;
    u8 reqPTY_packet[41 + 256];
    size_t reqPTY_packet_len;
    u8 reqPTY_local_channel[4];
    packet_requirev_state_t reqPTY_packet_requirev_state;

    /* State variables used in libssh2_channel_x11_req_ex() */
    libssh2_nonblocking_states reqX11_state;
    u8 *reqX11_packet;
    size_t reqX11_packet_len;
    u8 reqX11_local_channel[4];
    packet_requirev_state_t reqX11_packet_requirev_state;

    /* State variables used in libssh2_channel_process_startup() */
    libssh2_nonblocking_states process_state;
    u8 *process_packet;
    size_t process_packet_len;
    u8 process_local_channel[4];
    packet_requirev_state_t process_packet_requirev_state;

    /* State variables used in libssh2_channel_flush_ex() */
    libssh2_nonblocking_states flush_state;
    size_t flush_refund_bytes;
    size_t flush_flush_bytes;

    /* State variables used in libssh2_channel_receive_window_adjust() */
    libssh2_nonblocking_states adjust_state;
    u8 adjust_adjust[9];     /* packet_type(1) + channel(4) +
                                           adjustment(4) */

    /* State variables used in libssh2_channel_read_ex() */
    libssh2_nonblocking_states read_state;

    uint32_t read_local_id;

    /* State variables used in libssh2_channel_write_ex() */
    libssh2_nonblocking_states write_state;
    u8 write_packet[13];
    size_t write_packet_len;
    size_t write_bufwrite;

    /* State variables used in libssh2_channel_close() */
    libssh2_nonblocking_states close_state;
    u8 close_packet[5];

    /* State variables used in libssh2_channel_wait_closedeof() */
    libssh2_nonblocking_states wait_eof_state;

    /* State variables used in libssh2_channel_wait_closed() */
    libssh2_nonblocking_states wait_closed_state;

    /* State variables used in libssh2_channel_free() */
    libssh2_nonblocking_states free_state;

    /* State variables used in libssh2_channel_handle_extended_data2() */
    libssh2_nonblocking_states extData2_state;

    /* State variables used in libssh2_channel_request_auth_agent() */
    libssh2_nonblocking_states req_auth_agent_try_state;
    libssh2_nonblocking_states req_auth_agent_state;
    u8 req_auth_agent_packet[36];
    size_t req_auth_agent_packet_len;
    u8 req_auth_agent_local_channel[4];
    packet_requirev_state_t req_auth_agent_requirev_state;
};

struct _LIBSSH2_LISTENER
{
    struct list_node node; /* linked list header */

    LIBSSH2_SESSION *session;

    char *host;
    i32 port;

    /* a list of CHANNELs for this listener */
    struct list_head queue;

    i32 queue_size;
    i32 queue_maxsize;

    /* State variables used in libssh2_channel_forward_cancel() */
    libssh2_nonblocking_states chanFwdCncl_state;
    u8 *chanFwdCncl_data;
    size_t chanFwdCncl_data_len;
};

typedef struct _libssh2_endpoint_data
{
    u8 *banner;

    u8 *kexinit;
    size_t kexinit_len;

    const LIBSSH2_CRYPT_METHOD *crypt;
    uk crypt_abstract;

    const struct _LIBSSH2_MAC_METHOD *mac;
    uint32_t seqno;
    uk mac_abstract;

    const LIBSSH2_COMP_METHOD *comp;
    uk comp_abstract;

    /* Method Preferences -- NULL yields "load order" */
    char *crypt_prefs;
    char *mac_prefs;
    char *comp_prefs;
    char *lang_prefs;
} libssh2_endpoint_data;

#define PACKETBUFSIZE (1024*16)

struct transportpacket
{
    /* ------------- for incoming data --------------- */
    u8 buf[PACKETBUFSIZE];
    u8 init[5];  /* first 5 bytes of the incoming data stream,
                               still encrypted */
    size_t writeidx;        /* at what array index we do the next write into
                               the buffer */
    size_t readidx;         /* at what array index we do the next read from
                               the buffer */
    uint32_t packet_length; /* the most recent packet_length as read from the
                               network data */
    uint8_t padding_length; /* the most recent padding_length as read from the
                               network data */
    size_t data_num;        /* How much of the total package that has been read
                               so far. */
    size_t total_num;       /* How much a total package is supposed to be, in
                               number of bytes. A full package is
                               packet_length + padding_length + 4 +
                               mac_length. */
    u8 *payload; /* this is a pointer to a LIBSSH2_ALLOC()
                               area to which we write decrypted data */
    u8 *wptr;    /* write pointer into the payload to where we
                               are currently writing decrypted data */

    /* ------------- for outgoing data --------------- */
    u8 outbuf[MAX_SSH_PACKET_LEN]; /* area for the outgoing data */

    i32 ototal_num;         /* size of outbuf in number of bytes */
    u8k *odata; /* original pointer to the data */
    size_t olen;            /* original size of the data we stored in
                               outbuf */
    size_t osent;           /* number of bytes already sent */
};

struct _LIBSSH2_PUBLICKEY
{
    LIBSSH2_CHANNEL *channel;
    uint32_t version;

    /* State variables used in libssh2_publickey_packet_receive() */
    libssh2_nonblocking_states receive_state;
    u8 *receive_packet;
    size_t receive_packet_len;

    /* State variables used in libssh2_publickey_add_ex() */
    libssh2_nonblocking_states add_state;
    u8 *add_packet;
    u8 *add_s;

    /* State variables used in libssh2_publickey_remove_ex() */
    libssh2_nonblocking_states remove_state;
    u8 *remove_packet;
    u8 *remove_s;

    /* State variables used in libssh2_publickey_list_fetch() */
    libssh2_nonblocking_states listFetch_state;
    u8 *listFetch_s;
    u8 listFetch_buffer[12];
    u8 *listFetch_data;
    size_t listFetch_data_len;
};

#define LIBSSH2_SCP_RESPONSE_BUFLEN     256

struct flags {
    i32 sigpipe;  /* LIBSSH2_FLAG_SIGPIPE */
    i32 compress; /* LIBSSH2_FLAG_COMPRESS */
};

struct _LIBSSH2_SESSION
{
    /* Memory management callbacks */
    uk abstract;
      LIBSSH2_ALLOC_FUNC((*alloc));
      LIBSSH2_REALLOC_FUNC((*realloc));
      LIBSSH2_FREE_FUNC((*free));

    /* Other callbacks */
      LIBSSH2_IGNORE_FUNC((*ssh_msg_ignore));
      LIBSSH2_DEBUG_FUNC((*ssh_msg_debug));
      LIBSSH2_DISCONNECT_FUNC((*ssh_msg_disconnect));
      LIBSSH2_MACERROR_FUNC((*macerror));
      LIBSSH2_X11_OPEN_FUNC((*x11));
      LIBSSH2_SEND_FUNC((*send));
      LIBSSH2_RECV_FUNC((*recv));

    /* Method preferences -- NULL yields "load order" */
    char *kex_prefs;
    char *hostkey_prefs;

    i32 state;

    /* Flag options */
    struct flags flag;

    /* Agreed Key Exchange Method */
    const LIBSSH2_KEX_METHOD *kex;
    u32 burn_optimistic_kexinit:1;

    u8 *session_id;
    uint32_t session_id_len;

    /* this is set to TRUE if a blocking API behavior is requested */
    i32 api_block_mode;

    /* Timeout used when blocking API behavior is active */
    long api_timeout;

    /* Server's public key */
    const LIBSSH2_HOSTKEY_METHOD *hostkey;
    uk server_hostkey_abstract;

    /* Either set with libssh2_session_hostkey() (for server mode)
     * Or read from server in (eg) KEXDH_INIT (for client mode)
     */
    u8 *server_hostkey;
    uint32_t server_hostkey_len;
#if LIBSSH2_MD5
    u8 server_hostkey_md5[MD5_DIGEST_LENGTH];
    i32 server_hostkey_md5_valid;
#endif                          /* ! LIBSSH2_MD5 */
    u8 server_hostkey_sha1[SHA_DIGEST_LENGTH];
    i32 server_hostkey_sha1_valid;

    u8 server_hostkey_sha256[SHA256_DIGEST_LENGTH];
    i32 server_hostkey_sha256_valid;

    /* (remote as source of data -- packet_read ) */
    libssh2_endpoint_data remote;

    /* (local as source of data -- packet_write ) */
    libssh2_endpoint_data local;

    /* Inbound Data linked list -- Sometimes the packet that comes in isn't the
       packet we're ready for */
    struct list_head packets;

    /* Active connection channels */
    struct list_head channels;

    uint32_t next_channel;

    struct list_head listeners; /* list of LIBSSH2_LISTENER structs */

    /* Actual I/O socket */
    libssh2_socket_t socket_fd;
    i32 socket_state;
    i32 socket_block_directions;
    i32 socket_prev_blockstate; /* stores the state of the socket blockiness
                                   when libssh2_session_startup() is called */

    /* Error tracking */
    tukk err_msg;
    i32 err_code;
    i32 err_flags;

    /* struct members for packet-level reading */
    struct transportpacket packet;
#ifdef LIBSSH2DEBUG
    i32 showmask;               /* what debug/trace messages to display */
    libssh2_trace_handler_func tracehandler; /* callback to display trace
                                                messages */
    uk tracehandler_context; /* context for the trace handler */
#endif

    /* State variables used in libssh2_banner_send() */
    libssh2_nonblocking_states banner_TxRx_state;
    char banner_TxRx_banner[256];
    ssize_t banner_TxRx_total_send;

    /* State variables used in libssh2_kexinit() */
    libssh2_nonblocking_states kexinit_state;
    u8 *kexinit_data;
    size_t kexinit_data_len;

    /* State variables used in libssh2_session_startup() */
    libssh2_nonblocking_states startup_state;
    u8 *startup_data;
    size_t startup_data_len;
    u8 startup_service[sizeof("ssh-userauth") + 5 - 1];
    size_t startup_service_length;
    packet_require_state_t startup_req_state;
    key_exchange_state_t startup_key_state;

    /* State variables used in libssh2_session_free() */
    libssh2_nonblocking_states free_state;

    /* State variables used in libssh2_session_disconnect_ex() */
    libssh2_nonblocking_states disconnect_state;
    u8 disconnect_data[256 + 13];
    size_t disconnect_data_len;

    /* State variables used in libssh2_packet_read() */
    libssh2_nonblocking_states readPack_state;
    i32 readPack_encrypted;

    /* State variables used in libssh2_userauth_list() */
    libssh2_nonblocking_states userauth_list_state;
    u8 *userauth_list_data;
    size_t userauth_list_data_len;
    packet_requirev_state_t userauth_list_packet_requirev_state;

    /* State variables used in libssh2_userauth_password_ex() */
    libssh2_nonblocking_states userauth_pswd_state;
    u8 *userauth_pswd_data;
    u8 userauth_pswd_data0;
    size_t userauth_pswd_data_len;
    char *userauth_pswd_newpw;
    i32 userauth_pswd_newpw_len;
    packet_requirev_state_t userauth_pswd_packet_requirev_state;

    /* State variables used in libssh2_userauth_hostbased_fromfile_ex() */
    libssh2_nonblocking_states userauth_host_state;
    u8 *userauth_host_data;
    size_t userauth_host_data_len;
    u8 *userauth_host_packet;
    size_t userauth_host_packet_len;
    u8 *userauth_host_method;
    size_t userauth_host_method_len;
    u8 *userauth_host_s;
    packet_requirev_state_t userauth_host_packet_requirev_state;

    /* State variables used in libssh2_userauth_publickey_fromfile_ex() */
    libssh2_nonblocking_states userauth_pblc_state;
    u8 *userauth_pblc_data;
    size_t userauth_pblc_data_len;
    u8 *userauth_pblc_packet;
    size_t userauth_pblc_packet_len;
    u8 *userauth_pblc_method;
    size_t userauth_pblc_method_len;
    u8 *userauth_pblc_s;
    u8 *userauth_pblc_b;
    packet_requirev_state_t userauth_pblc_packet_requirev_state;

    /* State variables used in libssh2_userauth_keyboard_interactive_ex() */
    libssh2_nonblocking_states userauth_kybd_state;
    u8 *userauth_kybd_data;
    size_t userauth_kybd_data_len;
    u8 *userauth_kybd_packet;
    size_t userauth_kybd_packet_len;
    u32 userauth_kybd_auth_name_len;
    char *userauth_kybd_auth_name;
    unsigned userauth_kybd_auth_instruction_len;
    char *userauth_kybd_auth_instruction;
    u32 userauth_kybd_num_prompts;
    i32 userauth_kybd_auth_failure;
    LIBSSH2_USERAUTH_KBDINT_PROMPT *userauth_kybd_prompts;
    LIBSSH2_USERAUTH_KBDINT_RESPONSE *userauth_kybd_responses;
    packet_requirev_state_t userauth_kybd_packet_requirev_state;

    /* State variables used in libssh2_channel_open_ex() */
    libssh2_nonblocking_states open_state;
    packet_requirev_state_t open_packet_requirev_state;
    LIBSSH2_CHANNEL *open_channel;
    u8 *open_packet;
    size_t open_packet_len;
    u8 *open_data;
    size_t open_data_len;
    uint32_t open_local_channel;

    /* State variables used in libssh2_channel_direct_tcpip_ex() */
    libssh2_nonblocking_states direct_state;
    u8 *direct_message;
    size_t direct_host_len;
    size_t direct_shost_len;
    size_t direct_message_len;

    /* State variables used in libssh2_channel_forward_listen_ex() */
    libssh2_nonblocking_states fwdLstn_state;
    u8 *fwdLstn_packet;
    uint32_t fwdLstn_host_len;
    uint32_t fwdLstn_packet_len;
    packet_requirev_state_t fwdLstn_packet_requirev_state;

    /* State variables used in libssh2_publickey_init() */
    libssh2_nonblocking_states pkeyInit_state;
    LIBSSH2_PUBLICKEY *pkeyInit_pkey;
    LIBSSH2_CHANNEL *pkeyInit_channel;
    u8 *pkeyInit_data;
    size_t pkeyInit_data_len;
    /* 19 = packet_len(4) + version_len(4) + "version"(7) + version_num(4) */
    u8 pkeyInit_buffer[19];
    size_t pkeyInit_buffer_sent; /* how much of buffer that has been sent */

    /* State variables used in libssh2_packet_add() */
    libssh2_nonblocking_states packAdd_state;
    LIBSSH2_CHANNEL *packAdd_channelp; /* keeper of the channel during EAGAIN
                                          states */
    packet_queue_listener_state_t packAdd_Qlstn_state;
    packet_x11_open_state_t packAdd_x11open_state;

    /* State variables used in fullpacket() */
    libssh2_nonblocking_states fullpacket_state;
    i32 fullpacket_macstate;
    size_t fullpacket_payload_len;
    i32 fullpacket_packet_type;

    /* State variables used in libssh2_sftp_init() */
    libssh2_nonblocking_states sftpInit_state;
    LIBSSH2_SFTP *sftpInit_sftp;
    LIBSSH2_CHANNEL *sftpInit_channel;
    u8 sftpInit_buffer[9];   /* sftp_header(5){excludes request_id}
                                           + version_id(4) */
    i32 sftpInit_sent; /* number of bytes from the buffer that have been
                          sent */

    /* State variables used in libssh2_scp_recv() / libssh_scp_recv2() */
    libssh2_nonblocking_states scpRecv_state;
    u8 *scpRecv_command;
    size_t scpRecv_command_len;
    u8 scpRecv_response[LIBSSH2_SCP_RESPONSE_BUFLEN];
    size_t scpRecv_response_len;
    long scpRecv_mode;
#if defined(HAVE_LONGLONG) && defined(HAVE_STRTOLL)
    /* we have the type and we can parse such numbers */
    z64 scpRecv_size;
#define scpsize_strtol strtoll
#elif defined(HAVE_STRTOI64)
    __int64 scpRecv_size;
#define scpsize_strtol _strtoi64
#else
    long scpRecv_size;
#define scpsize_strtol strtol
#endif
    long scpRecv_mtime;
    long scpRecv_atime;
    LIBSSH2_CHANNEL *scpRecv_channel;

    /* State variables used in libssh2_scp_send_ex() */
    libssh2_nonblocking_states scpSend_state;
    u8 *scpSend_command;
    size_t scpSend_command_len;
    u8 scpSend_response[LIBSSH2_SCP_RESPONSE_BUFLEN];
    size_t scpSend_response_len;
    LIBSSH2_CHANNEL *scpSend_channel;

    /* Keepalive variables used by keepalive.c. */
    i32 keepalive_interval;
    i32 keepalive_want_reply;
    time_t keepalive_last_sent;
};

/* session.state bits */
#define LIBSSH2_STATE_EXCHANGING_KEYS   0x00000001
#define LIBSSH2_STATE_NEWKEYS           0x00000002
#define LIBSSH2_STATE_AUTHENTICATED     0x00000004
#define LIBSSH2_STATE_KEX_ACTIVE        0x00000008

/* session.flag helpers */
#ifdef MSG_NOSIGNAL
#define LIBSSH2_SOCKET_SEND_FLAGS(session)              \
    (((session)->flag.sigpipe) ? 0 : MSG_NOSIGNAL)
#define LIBSSH2_SOCKET_RECV_FLAGS(session)              \
    (((session)->flag.sigpipe) ? 0 : MSG_NOSIGNAL)
#else
/* If MSG_NOSIGNAL isn't defined we're SOL on blocking SIGPIPE */
#define LIBSSH2_SOCKET_SEND_FLAGS(session)      0
#define LIBSSH2_SOCKET_RECV_FLAGS(session)      0
#endif

/* --------- */

/* libssh2 extensible ssh api, ultimately I'd like to allow loading additional
   methods via .so/.dll */

struct _LIBSSH2_KEX_METHOD
{
    tukk name;

    /* Key exchange, populates session->* and returns 0 on success, non-0 on
       error */
    i32 (*exchange_keys) (LIBSSH2_SESSION * session,
                          key_exchange_state_low_t * key_state);

    long flags;
};

struct _LIBSSH2_HOSTKEY_METHOD
{
    tukk name;
    u64 hash_len;

    i32 (*init) (LIBSSH2_SESSION * session, u8k *hostkey_data,
                 size_t hostkey_data_len, uk *abstract);
    i32 (*initPEM) (LIBSSH2_SESSION * session, tukk privkeyfile,
                    u8k* passphrase, uk *abstract);
    i32 (*initPEMFromMemory) (LIBSSH2_SESSION * session,
                              tukk privkeyfiledata,
                              size_t privkeyfiledata_len,
                              u8k* passphrase,
                              uk *abstract);
    i32 (*sig_verify) (LIBSSH2_SESSION * session, u8k *sig,
                       size_t sig_len, u8k *m,
                       size_t m_len, uk *abstract);
    i32 (*signv) (LIBSSH2_SESSION * session, u8 **signature,
                  size_t *signature_len, i32 veccount,
                  const struct iovec datavec[], uk *abstract);
    i32 (*encrypt) (LIBSSH2_SESSION * session, u8 **dst,
                    size_t *dst_len, u8k *src,
                    size_t src_len, uk *abstract);
    i32 (*dtor) (LIBSSH2_SESSION * session, uk *abstract);
};

struct _LIBSSH2_CRYPT_METHOD
{
    tukk name;
    tukk pem_annotation;

    i32 blocksize;

    /* iv and key sizes (-1 for variable length) */
    i32 iv_len;
    i32 secret_len;

    long flags;

    i32 (*init) (LIBSSH2_SESSION * session,
                 const LIBSSH2_CRYPT_METHOD * method, u8 *iv,
                 i32 *free_iv, u8 *secret, i32 *free_secret,
                 i32 encrypt, uk *abstract);
    i32 (*crypt) (LIBSSH2_SESSION * session, u8 *block,
                  size_t blocksize, uk *abstract);
    i32 (*dtor) (LIBSSH2_SESSION * session, uk *abstract);

      _libssh2_cipher_type(algo);
};

struct _LIBSSH2_COMP_METHOD
{
    tukk name;
    i32 compress; /* 1 if it does compress, 0 if it doesn't */
    i32 use_in_auth; /* 1 if compression should be used in userauth */
    i32 (*init) (LIBSSH2_SESSION *session, i32 compress, uk *abstract);
    i32 (*comp) (LIBSSH2_SESSION *session,
                 u8 *dest,
                 size_t *dest_len,
                 u8k *src,
                 size_t src_len,
                 uk *abstract);
    i32 (*decomp) (LIBSSH2_SESSION *session,
                   u8 **dest,
                   size_t *dest_len,
                   size_t payload_limit,
                   u8k *src,
                   size_t src_len,
                   uk *abstract);
    i32 (*dtor) (LIBSSH2_SESSION * session, i32 compress, uk *abstract);
};

#ifdef LIBSSH2DEBUG
void _libssh2_debug(LIBSSH2_SESSION * session, i32 context, tukk format,
                    ...);
#else
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) ||     \
    defined(__GNUC__)
/* C99 supported and also by older GCC */
#define _libssh2_debug(x,y,z,...) do {} while (0)
#else
/* no gcc and not C99, do static and hopefully inline */
static inline void
_libssh2_debug(LIBSSH2_SESSION * session, i32 context, tukk format, ...)
{
    (void)session;
    (void)context;
    (void)format;
}
#endif
#endif

#define LIBSSH2_SOCKET_UNKNOWN                   1
#define LIBSSH2_SOCKET_CONNECTED                 0
#define LIBSSH2_SOCKET_DISCONNECTED             -1

/* Initial packet state, prior to MAC check */
#define LIBSSH2_MAC_UNCONFIRMED                  1
/* When MAC type is "none" (proto initiation phase) all packets are deemed
   "confirmed" */
#define LIBSSH2_MAC_CONFIRMED                    0
/* Something very bad is going on */
#define LIBSSH2_MAC_INVALID                     -1

/* Flags for _libssh2_error_flags */
/* Error message is allocated on the heap */
#define LIBSSH2_ERR_FLAG_DUP                     1

/* SSH Packet Types -- Defined by internet draft */
/* Transport Layer */
#define SSH_MSG_DISCONNECT                          1
#define SSH_MSG_IGNORE                              2
#define SSH_MSG_UNIMPLEMENTED                       3
#define SSH_MSG_DEBUG                               4
#define SSH_MSG_SERVICE_REQUEST                     5
#define SSH_MSG_SERVICE_ACCEPT                      6

#define SSH_MSG_KEXINIT                             20
#define SSH_MSG_NEWKEYS                             21

/* diffie-hellman-group1-sha1 */
#define SSH_MSG_KEXDH_INIT                          30
#define SSH_MSG_KEXDH_REPLY                         31

/* diffie-hellman-group-exchange-sha1 and
   diffie-hellman-group-exchange-sha256 */
#define SSH_MSG_KEX_DH_GEX_REQUEST_OLD              30
#define SSH_MSG_KEX_DH_GEX_REQUEST                  34
#define SSH_MSG_KEX_DH_GEX_GROUP                    31
#define SSH_MSG_KEX_DH_GEX_INIT                     32
#define SSH_MSG_KEX_DH_GEX_REPLY                    33

/* ecdh */
#define SSH2_MSG_KEX_ECDH_INIT                      30
#define SSH2_MSG_KEX_ECDH_REPLY                     31

/* User Authentication */
#define SSH_MSG_USERAUTH_REQUEST                    50
#define SSH_MSG_USERAUTH_FAILURE                    51
#define SSH_MSG_USERAUTH_SUCCESS                    52
#define SSH_MSG_USERAUTH_BANNER                     53

/* "public key" method */
#define SSH_MSG_USERAUTH_PK_OK                      60
/* "password" method */
#define SSH_MSG_USERAUTH_PASSWD_CHANGEREQ           60
/* "keyboard-interactive" method */
#define SSH_MSG_USERAUTH_INFO_REQUEST               60
#define SSH_MSG_USERAUTH_INFO_RESPONSE              61

/* Channels */
#define SSH_MSG_GLOBAL_REQUEST                      80
#define SSH_MSG_REQUEST_SUCCESS                     81
#define SSH_MSG_REQUEST_FAILURE                     82

#define SSH_MSG_CHANNEL_OPEN                        90
#define SSH_MSG_CHANNEL_OPEN_CONFIRMATION           91
#define SSH_MSG_CHANNEL_OPEN_FAILURE                92
#define SSH_MSG_CHANNEL_WINDOW_ADJUST               93
#define SSH_MSG_CHANNEL_DATA                        94
#define SSH_MSG_CHANNEL_EXTENDED_DATA               95
#define SSH_MSG_CHANNEL_EOF                         96
#define SSH_MSG_CHANNEL_CLOSE                       97
#define SSH_MSG_CHANNEL_REQUEST                     98
#define SSH_MSG_CHANNEL_SUCCESS                     99
#define SSH_MSG_CHANNEL_FAILURE                     100

/* Error codes returned in SSH_MSG_CHANNEL_OPEN_FAILURE message
   (see RFC4254) */
#define SSH_OPEN_ADMINISTRATIVELY_PROHIBITED 1
#define SSH_OPEN_CONNECT_FAILED              2
#define SSH_OPEN_UNKNOWN_CHANNELTYPE         3
#define SSH_OPEN_RESOURCE_SHORTAGE           4

ssize_t _libssh2_recv(libssh2_socket_t socket, uk buffer,
                      size_t length, i32 flags, uk *abstract);
ssize_t _libssh2_send(libssh2_socket_t socket, ukk buffer,
                      size_t length, i32 flags, uk *abstract);

#define LIBSSH2_READ_TIMEOUT 60 /* generic timeout in seconds used when
                                   waiting for more data to arrive */


i32 _libssh2_kex_exchange(LIBSSH2_SESSION * session, i32 reexchange,
                          key_exchange_state_t * state);

/* Let crypt.c/hostkey.c expose their method structs */
const LIBSSH2_CRYPT_METHOD **libssh2_crypt_methods(void);
const LIBSSH2_HOSTKEY_METHOD **libssh2_hostkey_methods(void);

/* misc.c */
i32 _libssh2_bcrypt_pbkdf(tukk pass,
                          size_t passlen,
                          const uint8_t *salt,
                          size_t saltlen,
                          uint8_t *key,
                          size_t keylen,
                          u32 rounds);

/* pem.c */
i32 _libssh2_pem_parse(LIBSSH2_SESSION * session,
                       tukk headerbegin,
                       tukk headerend,
                       u8k *passphrase,
                       FILE * fp, u8 **data, u32 *datalen);
i32 _libssh2_pem_parse_memory(LIBSSH2_SESSION * session,
                              tukk headerbegin,
                              tukk headerend,
                              tukk filedata, size_t filedata_len,
                              u8 **data, u32 *datalen);
 /* OpenSSL keys */
i32
_libssh2_openssh_pem_parse(LIBSSH2_SESSION * session,
                           u8k *passphrase,
                           FILE * fp, struct string_buf **decrypted_buf);
i32
_libssh2_openssh_pem_parse_memory(LIBSSH2_SESSION * session,
                                  u8k *passphrase,
                                  tukk filedata, size_t filedata_len,
                                  struct string_buf **decrypted_buf);

i32 _libssh2_pem_decode_sequence(u8 **data, u32 *datalen);
i32 _libssh2_pem_decode_integer(u8 **data, u32 *datalen,
                                u8 **i, u32 *ilen);

/* global.c */
void _libssh2_init_if_needed(void);


#define ARRAY_SIZE(a) (sizeof ((a)) / sizeof ((a)[0]))

/* define to output the libssh2_int64_t type in a *printf() */
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__MINGW32__)
#define LIBSSH2_INT64_T_FORMAT "I64d"
#else
#define LIBSSH2_INT64_T_FORMAT "lld"
#endif

/* In Windows the default file mode is text but an application can override it.
Therefore we specify it explicitly. https://github.com/curl/curl/pull/258
*/
#if defined(WIN32) || defined(MSDOS)
#define FOPEN_READTEXT "rt"
#define FOPEN_WRITETEXT "wt"
#define FOPEN_APPENDTEXT "at"
#elif defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when WIN32 is not defined.
https://cygwin.com/cygwin-ug-net/using-textbinary.html
For write we want our output to have line endings of LF and be compatible with
other Cygwin utilities. For read we want to handle input that may have line
endings either CRLF or LF so 't' is appropriate.
*/
#define FOPEN_READTEXT "rt"
#define FOPEN_WRITETEXT "w"
#define FOPEN_APPENDTEXT "a"
#else
#define FOPEN_READTEXT "r"
#define FOPEN_WRITETEXT "w"
#define FOPEN_APPENDTEXT "a"
#endif

#endif /* __LIBSSH2_PRIV_H */