/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2012 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/


/* This module contains the external function pcre_config(). */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Keep the original link size. */
static i32 real_link_size = LINK_SIZE;

#include "pcre_internal.h"


/*************************************************
* Return info about what features are configured *
*************************************************/

/* This function has an extensible interface so that additional items can be
added compatibly.

Arguments:
  what             what information is required
  where            where to put the information

Returns:           0 if data returned, negative on error
*/

#if defined COMPILE_PCRE8
PCRE_EXP_DEFN i32 PCRE_CALL_CONVENTION
pcre_config(i32 what, uk where)
#elif defined COMPILE_PCRE16
PCRE_EXP_DEFN i32 PCRE_CALL_CONVENTION
pcre16_config(i32 what, uk where)
#elif defined COMPILE_PCRE32
PCRE_EXP_DEFN i32 PCRE_CALL_CONVENTION
pcre32_config(i32 what, uk where)
#endif
{
switch (what)
  {
  case PCRE_CONFIG_UTF8:
#if defined COMPILE_PCRE16 || defined COMPILE_PCRE32
  *((i32*)where) = 0;
  return PCRE_ERROR_BADOPTION;
#else
#if defined SUPPORT_UTF
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;
#endif

  case PCRE_CONFIG_UTF16:
#if defined COMPILE_PCRE8 || defined COMPILE_PCRE32
  *((i32*)where) = 0;
  return PCRE_ERROR_BADOPTION;
#else
#if defined SUPPORT_UTF
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;
#endif

  case PCRE_CONFIG_UTF32:
#if defined COMPILE_PCRE8 || defined COMPILE_PCRE16
  *((i32*)where) = 0;
  return PCRE_ERROR_BADOPTION;
#else
#if defined SUPPORT_UTF
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;
#endif

  case PCRE_CONFIG_UNICODE_PROPERTIES:
#ifdef SUPPORT_UCP
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;

  case PCRE_CONFIG_JIT:
#ifdef SUPPORT_JIT
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;

  case PCRE_CONFIG_JITTARGET:
#ifdef SUPPORT_JIT
  *((tukk*)where) = PRIV(jit_get_target)();
#else
  *((tukk*)where) = NULL;
#endif
  break;

  case PCRE_CONFIG_NEWLINE:
  *((i32*)where) = NEWLINE;
  break;

  case PCRE_CONFIG_BSR:
#ifdef BSR_ANYCRLF
  *((i32*)where) = 1;
#else
  *((i32*)where) = 0;
#endif
  break;

  case PCRE_CONFIG_LINK_SIZE:
  *((i32*)where) = real_link_size;
  break;

  case PCRE_CONFIG_POSIX_MALLOC_THRESHOLD:
  *((i32*)where) = POSIX_MALLOC_THRESHOLD;
  break;

  case PCRE_CONFIG_PARENS_LIMIT:
  *((u64*)where) = PARENS_NEST_LIMIT;
  break;

  case PCRE_CONFIG_MATCH_LIMIT:
  *((u64*)where) = MATCH_LIMIT;
  break;

  case PCRE_CONFIG_MATCH_LIMIT_RECURSION:
  *((u64*)where) = MATCH_LIMIT_RECURSION;
  break;

  case PCRE_CONFIG_STACKRECURSE:
#ifdef NO_RECURSE
  *((i32*)where) = 0;
#else
  *((i32*)where) = 1;
#endif
  break;

  default: return PCRE_ERROR_BADOPTION;
  }

return 0;
}

/* End of pcre_config.c */
