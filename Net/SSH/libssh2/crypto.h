#ifndef __LIBSSH2_CRYPTO_H
#define __LIBSSH2_CRYPTO_H
/* Copyright (C) 2009, 2010 Simon Josefsson
 * Copyright (C) 2006, 2007 The Written Word, Inc.  All rights reserved.
 * Copyright (C) 2010-2019 Daniel Stenberg
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

#ifdef LIBSSH2_OPENSSL
#include "openssl.h"
#endif

#ifdef LIBSSH2_LIBGCRYPT
#include "libgcrypt.h"
#endif

#ifdef LIBSSH2_WINCNG
#include "wincng.h"
#endif

#ifdef LIBSSH2_OS400QC3
#include "os400qc3.h"
#endif

#ifdef LIBSSH2_MBEDTLS
#include "mbedtls.h"
#endif

#define LIBSSH2_ED25519_KEY_LEN 32
#define LIBSSH2_ED25519_PRIVATE_KEY_LEN 64
#define LIBSSH2_ED25519_SIG_LEN 64

#if LIBSSH2_RSA
i32 _libssh2_rsa_new(libssh2_rsa_ctx ** rsa,
                     u8k *edata,
                     u64 elen,
                     u8k *ndata,
                     u64 nlen,
                     u8k *ddata,
                     u64 dlen,
                     u8k *pdata,
                     u64 plen,
                     u8k *qdata,
                     u64 qlen,
                     u8k *e1data,
                     u64 e1len,
                     u8k *e2data,
                     u64 e2len,
                     u8k *coeffdata, u64 coefflen);
i32 _libssh2_rsa_new_private(libssh2_rsa_ctx ** rsa,
                             LIBSSH2_SESSION * session,
                             tukk filename,
                             u8k* passphrase);
i32 _libssh2_rsa_sha1_verify(libssh2_rsa_ctx * rsa,
                             u8k *sig,
                             u64 sig_len,
                             u8k *m, u64 m_len);
i32 _libssh2_rsa_sha1_sign(LIBSSH2_SESSION * session,
                           libssh2_rsa_ctx * rsactx,
                           u8k *hash,
                           size_t hash_len,
                           u8 **signature,
                           size_t *signature_len);
i32 _libssh2_rsa_new_private_frommemory(libssh2_rsa_ctx ** rsa,
                                        LIBSSH2_SESSION * session,
                                        tukk filedata,
                                        size_t filedata_len,
                                        u8k* passphrase);
#endif

#if LIBSSH2_DSA
i32 _libssh2_dsa_new(libssh2_dsa_ctx ** dsa,
                     u8k *pdata,
                     u64 plen,
                     u8k *qdata,
                     u64 qlen,
                     u8k *gdata,
                     u64 glen,
                     u8k *ydata,
                     u64 ylen,
                     u8k *x, u64 x_len);
i32 _libssh2_dsa_new_private(libssh2_dsa_ctx ** dsa,
                             LIBSSH2_SESSION * session,
                             tukk filename,
                             u8k* passphrase);
i32 _libssh2_dsa_sha1_verify(libssh2_dsa_ctx * dsactx,
                             u8k *sig,
                             u8k *m, u64 m_len);
i32 _libssh2_dsa_sha1_sign(libssh2_dsa_ctx * dsactx,
                           u8k *hash,
                           u64 hash_len, u8 *sig);
i32 _libssh2_dsa_new_private_frommemory(libssh2_dsa_ctx ** dsa,
                                        LIBSSH2_SESSION * session,
                                        tukk filedata,
                                        size_t filedata_len,
                                        u8k* passphrase);
#endif

#if LIBSSH2_ECDSA
i32
_libssh2_ecdsa_curve_name_with_octal_new(libssh2_ecdsa_ctx ** ecdsactx,
                                         u8k *k,
                                         size_t k_len,
                                         libssh2_curve_type type);
i32
_libssh2_ecdsa_new_private(libssh2_ecdsa_ctx ** ec_ctx,
                           LIBSSH2_SESSION * session,
                           tukk filename,
                           u8k* passphrase);

i32
_libssh2_ecdsa_verify(libssh2_ecdsa_ctx * ctx,
                      u8k *r, size_t r_len,
                      u8k *s, size_t s_len,
                      u8k *m, size_t m_len);

i32
_libssh2_ecdsa_create_key(LIBSSH2_SESSION *session,
                          _libssh2_ec_key **out_private_key,
                          u8 **out_public_key_octal,
                          size_t *out_public_key_octal_len,
                          libssh2_curve_type curve_type);

i32
_libssh2_ecdh_gen_k(_libssh2_bn **k, _libssh2_ec_key *private_key,
                    u8k *server_public_key,
                    size_t server_public_key_len);

i32
_libssh2_ecdsa_sign(LIBSSH2_SESSION *session, libssh2_ecdsa_ctx *ec_ctx,
                    u8k *hash, u64 hash_len,
                    u8 **signature, size_t *signature_len);

i32 _libssh2_ecdsa_new_private_frommemory(libssh2_ecdsa_ctx ** ec_ctx,
                                          LIBSSH2_SESSION * session,
                                          tukk filedata,
                                          size_t filedata_len,
                                          u8k* passphrase);

libssh2_curve_type
_libssh2_ecdsa_get_curve_type(libssh2_ecdsa_ctx *ec_ctx);

i32
_libssh2_ecdsa_curve_type_from_name(tukk name,
                                    libssh2_curve_type *out_type);

#endif /* LIBSSH2_ECDSA */

#if LIBSSH2_ED25519

i32
_libssh2_curve25519_new(LIBSSH2_SESSION *session, uint8_t **out_public_key,
                        uint8_t **out_private_key);

i32
_libssh2_curve25519_gen_k(_libssh2_bn **k,
                          uint8_t private_key[LIBSSH2_ED25519_KEY_LEN],
                          uint8_t server_public_key[LIBSSH2_ED25519_KEY_LEN]);

i32
_libssh2_ed25519_verify(libssh2_ed25519_ctx *ctx, const uint8_t *s,
                        size_t s_len, const uint8_t *m, size_t m_len);

i32
_libssh2_ed25519_new_private(libssh2_ed25519_ctx **ed_ctx,
                            LIBSSH2_SESSION *session,
                            tukk filename, const uint8_t *passphrase);

i32
_libssh2_ed25519_new_public(libssh2_ed25519_ctx **ed_ctx,
                            LIBSSH2_SESSION *session,
                            u8k *raw_pub_key,
                            const uint8_t key_len);

i32
_libssh2_ed25519_sign(libssh2_ed25519_ctx *ctx, LIBSSH2_SESSION *session,
                      uint8_t **out_sig, size_t *out_sig_len,
                      const uint8_t *message, size_t message_len);

i32
_libssh2_ed25519_new_private_frommemory(libssh2_ed25519_ctx **ed_ctx,
                                        LIBSSH2_SESSION *session,
                                        tukk filedata,
                                        size_t filedata_len,
                                        u8k* passphrase);

#endif /* LIBSSH2_ED25519 */


i32 _libssh2_cipher_init(_libssh2_cipher_ctx * h,
                         _libssh2_cipher_type(algo),
                         u8 *iv,
                         u8 *secret, i32 encrypt);

i32 _libssh2_cipher_crypt(_libssh2_cipher_ctx * ctx,
                          _libssh2_cipher_type(algo),
                          i32 encrypt, u8 *block, size_t blocksize);

i32 _libssh2_pub_priv_keyfile(LIBSSH2_SESSION *session,
                              u8 **method,
                              size_t *method_len,
                              u8 **pubkeydata,
                              size_t *pubkeydata_len,
                              tukk privatekey,
                              tukk passphrase);

i32 _libssh2_pub_priv_keyfilememory(LIBSSH2_SESSION *session,
                                    u8 **method,
                                    size_t *method_len,
                                    u8 **pubkeydata,
                                    size_t *pubkeydata_len,
                                    tukk privatekeydata,
                                    size_t privatekeydata_len,
                                    tukk passphrase);

#endif /* __LIBSSH2_CRYPTO_H */
