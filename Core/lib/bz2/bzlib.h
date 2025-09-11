
/*-------------------------------------------------------------*/
/*--- Public header file for the library.                   ---*/
/*---                                               bzlib.h ---*/
/*-------------------------------------------------------------*/

/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.0.6 of 6 September 2010
   Copyright (C) 1996-2010 Julian Seward <jseward@bzip.org>

   Please read the WARNING, DISCLAIMER and PATENTS sections in the
   README file.

   This program is released under the terms of the license contained
   in the file LICENSE.
   ------------------------------------------------------------------ */


#ifndef _BZLIB_H
#define _BZLIB_H
#include <drxtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BZ_RUN               0
#define BZ_FLUSH             1
#define BZ_FINISH            2

#define BZ_OK                0
#define BZ_RUN_OK            1
#define BZ_FLUSH_OK          2
#define BZ_FINISH_OK         3
#define BZ_STREAM_END        4
#define BZ_SEQUENCE_ERROR    (-1)
#define BZ_PARAM_ERROR       (-2)
#define BZ_MEM_ERROR         (-3)
#define BZ_DATA_ERROR        (-4)
#define BZ_DATA_ERROR_MAGIC  (-5)
#define BZ_IO_ERROR          (-6)
#define BZ_UNEXPECTED_EOF    (-7)
#define BZ_OUTBUFF_FULL      (-8)
#define BZ_CONFIG_ERROR      (-9)

typedef
   struct {
      tuk  next_in;
      u32 avail_in;
      u32 total_in_lo32;
      u32 total_in_hi32;

      tuk  next_out;
      u32 avail_out;
      u32 total_out_lo32;
      u32 total_out_hi32;

      uk state;

      uk (*bzalloc)(uk, i32, i32);
      void (*bzfree)(uk, uk);
      uk opaque;
   }
   bz_stream;


#ifndef BZ_IMPORT
#define BZ_EXPORT
#endif

#ifdef _WIN32
#   include <windows.h>
#   ifdef small
      // windows.h define small to tuk
#      undef small
#   endif
#   ifdef BZ_EXPORT
#   define BZ_API(func) WINAPI func
#   define BZ_EXTERN extern
#   else
   // import windows dll dynamically
#   define BZ_API(func) (WINAPI * func)
#   define BZ_EXTERN
#   endif
#else
#   define BZ_API(func) func
#   define BZ_EXTERN extern
#endif


/*-- Core (low-level) library functions --*/

BZ_EXTERN i32 BZ_API(BZ2_bzCompressInit) (
      bz_stream* strm,
      i32        blockSize100k,
      i32        verbosity,
      i32        workFactor
 );

BZ_EXTERN i32 BZ_API(BZ2_bzCompress) (
      bz_stream* strm,
      i32 action
 );

BZ_EXTERN i32 BZ_API(BZ2_bzCompressEnd) (
      bz_stream* strm
 );

BZ_EXTERN i32 BZ_API(BZ2_bzDecompressInit) (
      bz_stream *strm,
      i32       verbosity,
      i32       small
 );

BZ_EXTERN i32 BZ_API(BZ2_bzDecompress) (
      bz_stream* strm
 );

BZ_EXTERN i32 BZ_API(BZ2_bzDecompressEnd) (
      bz_stream *strm
 );



/*-- High(er) level library functions --*/

#ifndef BZ_NO_STDIO
#define BZ_MAX_UNUSED 5000

typedef void BZFILE;

BZ_EXTERN BZFILE* BZ_API(BZ2_bzReadOpen) (
      i32*  bzerror,
      fuk   f,
      i32   verbosity,
      i32   small,
      uk    unused,
      i32   nUnused
 );

BZ_EXTERN void BZ_API(BZ2_bzReadClose) (
      i32*    bzerror,
      BZFILE* b
 );

BZ_EXTERN void BZ_API(BZ2_bzReadGetUnused) (
      i32*    bzerror,
      BZFILE* b,
      uk*     unused,
      i32*    nUnused
 );

BZ_EXTERN i32 BZ_API(BZ2_bzRead) (
      i32*    bzerror,
      BZFILE* b,
      uk      buf,
      i32     len
 );

BZ_EXTERN BZFILE* BZ_API(BZ2_bzWriteOpen) (
      i32*  bzerror,
      fuk   f,
      i32   blockSize100k,
      i32   verbosity,
      i32   workFactor
 );

BZ_EXTERN void BZ_API(BZ2_bzWrite) (
      i32*    bzerror,
      BZFILE* b,
      uk      buf,
      i32     len
 );

BZ_EXTERN void BZ_API(BZ2_bzWriteClose) (
      i32*          bzerror,
      BZFILE*       b,
      i32           abandon,
      u32*         nbytes_in,
      u32*         nbytes_out
 );

BZ_EXTERN void BZ_API(BZ2_bzWriteClose64) (
      i32*          bzerror,
      BZFILE*       b,
      i32           abandon,
      u32*         nbytes_in_lo32,
      u32*         nbytes_in_hi32,
      u32*         nbytes_out_lo32,
      u32*         nbytes_out_hi32
 );
#endif


/*-- Utility functions --*/

BZ_EXTERN i32 BZ_API(BZ2_bzBuffToBuffCompress) (
      tuk         dest,
      u32*       destLen,
      tuk         source,
      u32        sourceLen,
      i32         blockSize100k,
      i32         verbosity,
      i32         workFactor
 );

BZ_EXTERN i32 BZ_API(BZ2_bzBuffToBuffDecompress) (
      tuk         dest,
      u32*       destLen,
      tuk         source,
      u32        sourceLen,
      i32         small,
      i32         verbosity
 );


/*--
   Code contributed by Yoshioka Tsuneo (tsuneo@rr.iij4u.or.jp)
   to support better zlib compatibility.
   This code is not _officially_ part of libbzip2 (yet);
   I haven't tested it, documented it, or considered the
   threading-safeness of it.
   If this code breaks, please contact both Yoshioka and me.
--*/

BZ_EXTERN tukk  BZ_API(BZ2_bzlibVersion) (
      void
 );

#ifndef BZ_NO_STDIO
BZ_EXTERN BZFILE * BZ_API(BZ2_bzopen) (
      tukk path,
      tukk mode
 );

BZ_EXTERN BZFILE * BZ_API(BZ2_bzdopen) (
      i32        fd,
      tukk       mode
 );

BZ_EXTERN i32 BZ_API(BZ2_bzread) (
      BZFILE* b,
      uk      buf,
      i32     len
 );

BZ_EXTERN i32 BZ_API(BZ2_bzwrite) (
      BZFILE* b,
      uk      buf,
      i32     len
 );

BZ_EXTERN i32 BZ_API(BZ2_bzflush) (
      BZFILE* b
 );

BZ_EXTERN void BZ_API(BZ2_bzclose) (
      BZFILE* b
 );

BZ_EXTERN tukk  BZ_API(BZ2_bzerror) (
      BZFILE *b,
      i32    *errnum
 );
#endif

#ifdef __cplusplus
}
#endif

#endif

/*-------------------------------------------------------------*/
/*--- end                                           bzlib.h ---*/
/*-------------------------------------------------------------*/
