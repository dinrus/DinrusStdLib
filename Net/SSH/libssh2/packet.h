#ifndef __LIBSSH2_PACKET_H
#define __LIBSSH2_PACKET_H
/*
 * Copyright (C) 2010 by Daniel Stenberg
 * Author: Daniel Stenberg <daniel@haxx.se>
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
 *
 */

i32 _libssh2_packet_read(LIBSSH2_SESSION * session);

i32 _libssh2_packet_ask(LIBSSH2_SESSION * session, u8 packet_type,
                        u8 **data, size_t *data_len,
                        i32 match_ofs,
                        u8k *match_buf,
                        size_t match_len);

i32 _libssh2_packet_askv(LIBSSH2_SESSION * session,
                         u8k *packet_types,
                         u8 **data, size_t *data_len,
                         i32 match_ofs,
                         u8k *match_buf,
                         size_t match_len);
i32 _libssh2_packet_require(LIBSSH2_SESSION * session,
                            u8 packet_type, u8 **data,
                            size_t *data_len, i32 match_ofs,
                            u8k *match_buf,
                            size_t match_len,
                            packet_require_state_t * state);
i32 _libssh2_packet_requirev(LIBSSH2_SESSION *session,
                             u8k *packet_types,
                             u8 **data, size_t *data_len,
                             i32 match_ofs,
                             u8k *match_buf,
                             size_t match_len,
                             packet_requirev_state_t * state);
i32 _libssh2_packet_burn(LIBSSH2_SESSION * session,
                         libssh2_nonblocking_states * state);
i32 _libssh2_packet_write(LIBSSH2_SESSION * session, u8 *data,
                          u64 data_len);
i32 _libssh2_packet_add(LIBSSH2_SESSION * session, u8 *data,
                        size_t datalen, i32 macstate);

#endif /* __LIBSSH2_PACKET_H */
