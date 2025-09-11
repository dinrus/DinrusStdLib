/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* This is the public header file for the PCRE library, to be #included by
applications that call the PCRE functions.

           Copyright (c) 1997-2014 University of Cambridge

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

#ifndef _PCRE_H
#define _PCRE_H

/* The current PCRE version information. */

#define PCRE_MAJOR          8
#define PCRE_MINOR          43
#define PCRE_PRERELEASE
#define PCRE_DATE           2019-02-23

/* When an application links to a PCRE DLL in Windows, the symbols that are
imported have to be identified as such. When building PCRE, the appropriate
export setting is defined in pcre_internal.h, which includes this file. So we
don't change existing definitions of PCRE_EXP_DECL and PCRECPP_EXP_DECL. */

#if defined(_WIN32) && !defined(PCRE_STATIC)
#  ifndef PCRE_EXP_DECL
#    define PCRE_EXP_DECL  extern __declspec(dllimport)
#  endif
#  ifdef __cplusplus
#    ifndef PCRECPP_EXP_DECL
#      define PCRECPP_EXP_DECL  extern __declspec(dllimport)
#    endif
#    ifndef PCRECPP_EXP_DEFN
#      define PCRECPP_EXP_DEFN  __declspec(dllimport)
#    endif
#  endif
#endif

/* By default, we use the standard "extern" declarations. */

#ifndef PCRE_EXP_DECL
#  ifdef __cplusplus
#    define PCRE_EXP_DECL  extern "C"
#  else
#    define PCRE_EXP_DECL  extern
#  endif
#endif

#ifdef __cplusplus
#  ifndef PCRECPP_EXP_DECL
#    define PCRECPP_EXP_DECL  extern
#  endif
#  ifndef PCRECPP_EXP_DEFN
#    define PCRECPP_EXP_DEFN
#  endif
#endif

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
extern "C" {
#endif

/* Public options. Some are compile-time only, some are run-time only, and some
are both. Most of the compile-time options are saved with the compiled regex so
that they can be inspected during studying (and therefore JIT compiling). Note
that pcre_study() has its own set of options. Originally, all the options
defined here used distinct bits. However, almost all the bits in a 32-bit word
are now used, so in order to conserve them, option bits that were previously
only recognized at matching time (i.e. by pcre_exec() or pcre_dfa_exec()) may
also be used for compile-time options that affect only compiling and are not
relevant for studying or JIT compiling.

Some options for pcre_compile() change its behaviour but do not affect the
behaviour of the execution functions. Other options are passed through to the
execution functions and affect their behaviour, with or without affecting the
behaviour of pcre_compile().

Options that can be passed to pcre_compile() are tagged Cx below, with these
variants:

C1   Affects compile only
C2   Does not affect compile; affects exec, dfa_exec
C3   Affects compile, exec, dfa_exec
C4   Affects compile, exec, dfa_exec, study
C5   Affects compile, exec, study

Options that can be set for pcre_exec() and/or pcre_dfa_exec() are flagged with
E and D, respectively. They take precedence over C3, C4, and C5 settings passed
from pcre_compile(). Those that are compatible with JIT execution are flagged
with J. */

#define PCRE_CASELESS           0x00000001  /* C1       */
#define PCRE_MULTILINE          0x00000002  /* C1       */
#define PCRE_DOTALL             0x00000004  /* C1       */
#define PCRE_EXTENDED           0x00000008  /* C1       */
#define PCRE_ANCHORED           0x00000010  /* C4 E D   */
#define PCRE_DOLLAR_ENDONLY     0x00000020  /* C2       */
#define PCRE_EXTRA              0x00000040  /* C1       */
#define PCRE_NOTBOL             0x00000080  /*    E D J */
#define PCRE_NOTEOL             0x00000100  /*    E D J */
#define PCRE_UNGREEDY           0x00000200  /* C1       */
#define PCRE_NOTEMPTY           0x00000400  /*    E D J */
#define PCRE_UTF8               0x00000800  /* C4      )          */
#define PCRE_UTF16              0x00000800  /* C4      ) Synonyms */
#define PCRE_UTF32              0x00000800  /* C4      )          */
#define PCRE_NO_AUTO_CAPTURE    0x00001000  /* C1       */
#define PCRE_NO_UTF8_CHECK      0x00002000  /* C1 E D J)          */
#define PCRE_NO_UTF16_CHECK     0x00002000  /* C1 E D J) Synonyms */
#define PCRE_NO_UTF32_CHECK     0x00002000  /* C1 E D J)          */
#define PCRE_AUTO_CALLOUT       0x00004000  /* C1       */
#define PCRE_PARTIAL_SOFT       0x00008000  /*    E D J) Synonyms */
#define PCRE_PARTIAL            0x00008000  /*    E D J)          */

/* This pair use the same bit. */
#define PCRE_NEVER_UTF          0x00010000  /* C1      ) Overlaid */
#define PCRE_DFA_SHORTEST       0x00010000  /*      D  ) Overlaid */

/* This pair use the same bit. */
#define PCRE_NO_AUTO_POSSESS    0x00020000  /* C1      ) Overlaid */
#define PCRE_DFA_RESTART        0x00020000  /*      D  ) Overlaid */

#define PCRE_FIRSTLINE          0x00040000  /* C3       */
#define PCRE_DUPNAMES           0x00080000  /* C1       */
#define PCRE_NEWLINE_CR         0x00100000  /* C3 E D   */
#define PCRE_NEWLINE_LF         0x00200000  /* C3 E D   */
#define PCRE_NEWLINE_CRLF       0x00300000  /* C3 E D   */
#define PCRE_NEWLINE_ANY        0x00400000  /* C3 E D   */
#define PCRE_NEWLINE_ANYCRLF    0x00500000  /* C3 E D   */
#define PCRE_BSR_ANYCRLF        0x00800000  /* C3 E D   */
#define PCRE_BSR_UNICODE        0x01000000  /* C3 E D   */
#define PCRE_JAVASCRIPT_COMPAT  0x02000000  /* C5       */
#define PCRE_NO_START_OPTIMIZE  0x04000000  /* C2 E D  ) Synonyms */
#define PCRE_NO_START_OPTIMISE  0x04000000  /* C2 E D  )          */
#define PCRE_PARTIAL_HARD       0x08000000  /*    E D J */
#define PCRE_NOTEMPTY_ATSTART   0x10000000  /*    E D J */
#define PCRE_UCP                0x20000000  /* C3       */

/* Exec-time and get/set-time error codes */

#define PCRE_ERROR_NOMATCH          (-1)
#define PCRE_ERROR_NULL             (-2)
#define PCRE_ERROR_BADOPTION        (-3)
#define PCRE_ERROR_BADMAGIC         (-4)
#define PCRE_ERROR_UNKNOWN_OPCODE   (-5)
#define PCRE_ERROR_UNKNOWN_NODE     (-5)  /* For backward compatibility */
#define PCRE_ERROR_NOMEMORY         (-6)
#define PCRE_ERROR_NOSUBTXT      (-7)
#define PCRE_ERROR_MATCHLIMIT       (-8)
#define PCRE_ERROR_CALLOUT          (-9)  /* Never used by PCRE itself */
#define PCRE_ERROR_BADUTF8         (-10)  /* Same for 8/16/32 */
#define PCRE_ERROR_BADUTF16        (-10)  /* Same for 8/16/32 */
#define PCRE_ERROR_BADUTF32        (-10)  /* Same for 8/16/32 */
#define PCRE_ERROR_BADUTF8_OFFSET  (-11)  /* Same for 8/16 */
#define PCRE_ERROR_BADUTF16_OFFSET (-11)  /* Same for 8/16 */
#define PCRE_ERROR_PARTIAL         (-12)
#define PCRE_ERROR_BADPARTIAL      (-13)
#define PCRE_ERROR_INTERNAL        (-14)
#define PCRE_ERROR_BADCOUNT        (-15)
#define PCRE_ERROR_DFA_UITEM       (-16)
#define PCRE_ERROR_DFA_UCOND       (-17)
#define PCRE_ERROR_DFA_UMLIMIT     (-18)
#define PCRE_ERROR_DFA_WSSIZE      (-19)
#define PCRE_ERROR_DFA_RECURSE     (-20)
#define PCRE_ERROR_RECURSIONLIMIT  (-21)
#define PCRE_ERROR_NULLWSLIMIT     (-22)  /* No longer actually used */
#define PCRE_ERROR_BADNEWLINE      (-23)
#define PCRE_ERROR_BADOFFSET       (-24)
#define PCRE_ERROR_SHORTUTF8       (-25)
#define PCRE_ERROR_SHORTUTF16      (-25)  /* Same for 8/16 */
#define PCRE_ERROR_RECURSELOOP     (-26)
#define PCRE_ERROR_JIT_STACKLIMIT  (-27)
#define PCRE_ERROR_BADMODE         (-28)
#define PCRE_ERROR_BADENDIANNESS   (-29)
#define PCRE_ERROR_DFA_BADRESTART  (-30)
#define PCRE_ERROR_JIT_BADOPTION   (-31)
#define PCRE_ERROR_BADLENGTH       (-32)
#define PCRE_ERROR_UNSET           (-33)

/* Specific error codes for UTF-8 validity checks */

#define PCRE_UTF8_ERR0               0
#define PCRE_UTF8_ERR1               1
#define PCRE_UTF8_ERR2               2
#define PCRE_UTF8_ERR3               3
#define PCRE_UTF8_ERR4               4
#define PCRE_UTF8_ERR5               5
#define PCRE_UTF8_ERR6               6
#define PCRE_UTF8_ERR7               7
#define PCRE_UTF8_ERR8               8
#define PCRE_UTF8_ERR9               9
#define PCRE_UTF8_ERR10             10
#define PCRE_UTF8_ERR11             11
#define PCRE_UTF8_ERR12             12
#define PCRE_UTF8_ERR13             13
#define PCRE_UTF8_ERR14             14
#define PCRE_UTF8_ERR15             15
#define PCRE_UTF8_ERR16             16
#define PCRE_UTF8_ERR17             17
#define PCRE_UTF8_ERR18             18
#define PCRE_UTF8_ERR19             19
#define PCRE_UTF8_ERR20             20
#define PCRE_UTF8_ERR21             21
#define PCRE_UTF8_ERR22             22  /* Unused (was non-character) */

/* Specific error codes for UTF-16 validity checks */

#define PCRE_UTF16_ERR0              0
#define PCRE_UTF16_ERR1              1
#define PCRE_UTF16_ERR2              2
#define PCRE_UTF16_ERR3              3
#define PCRE_UTF16_ERR4              4  /* Unused (was non-character) */

/* Specific error codes for UTF-32 validity checks */

#define PCRE_UTF32_ERR0              0
#define PCRE_UTF32_ERR1              1
#define PCRE_UTF32_ERR2              2  /* Unused (was non-character) */
#define PCRE_UTF32_ERR3              3

/* Request types for pcre_fullinfo() */

#define PCRE_INFO_OPTIONS            0
#define PCRE_INFO_SIZE               1
#define PCRE_INFO_CAPTURECOUNT       2
#define PCRE_INFO_BACKREFMAX         3
#define PCRE_INFO_FIRSTBYTE          4
#define PCRE_INFO_FIRSTCHAR          4  /* For backwards compatibility */
#define PCRE_INFO_FIRSTTABLE         5
#define PCRE_INFO_LASTLITERAL        6
#define PCRE_INFO_NAMEENTRYSIZE      7
#define PCRE_INFO_NAMECOUNT          8
#define PCRE_INFO_NAMETABLE          9
#define PCRE_INFO_STUDYSIZE         10
#define PCRE_INFO_DEFAULT_TABLES    11
#define PCRE_INFO_OKPARTIAL         12
#define PCRE_INFO_JCHANGED          13
#define PCRE_INFO_HASCRORLF         14
#define PCRE_INFO_MINLENGTH         15
#define PCRE_INFO_JIT               16
#define PCRE_INFO_JITSIZE           17
#define PCRE_INFO_MAXLOOKBEHIND     18
#define PCRE_INFO_FIRSTCHARACTER    19
#define PCRE_INFO_FIRSTCHARACTERFLAGS 20
#define PCRE_INFO_REQUIREDCHAR      21
#define PCRE_INFO_REQUIREDCHARFLAGS 22
#define PCRE_INFO_MATCHLIMIT        23
#define PCRE_INFO_RECURSIONLIMIT    24
#define PCRE_INFO_MATCH_EMPTY       25

/* Request types for pcre_config(). Do not re-arrange, in order to remain
compatible. */

#define PCRE_CONFIG_UTF8                    0
#define PCRE_CONFIG_NEWLINE                 1
#define PCRE_CONFIG_LINK_SIZE               2
#define PCRE_CONFIG_POSIX_MALLOC_THRESHOLD  3
#define PCRE_CONFIG_MATCH_LIMIT             4
#define PCRE_CONFIG_STACKRECURSE            5
#define PCRE_CONFIG_UNICODE_PROPERTIES      6
#define PCRE_CONFIG_MATCH_LIMIT_RECURSION   7
#define PCRE_CONFIG_BSR                     8
#define PCRE_CONFIG_JIT                     9
#define PCRE_CONFIG_UTF16                  10
#define PCRE_CONFIG_JITTARGET              11
#define PCRE_CONFIG_UTF32                  12
#define PCRE_CONFIG_PARENS_LIMIT           13

/* Request types for pcre_study(). Do not re-arrange, in order to remain
compatible. */

#define PCRE_STUDY_JIT_COMPILE                0x0001
#define PCRE_STUDY_JIT_PARTIAL_SOFT_COMPILE   0x0002
#define PCRE_STUDY_JIT_PARTIAL_HARD_COMPILE   0x0004
#define PCRE_STUDY_EXTRA_NEEDED               0x0008

/* Bit flags for the pcre[16|32]_extra structure. Do not re-arrange or redefine
these bits, just add new ones on the end, in order to remain compatible. */

#define PCRE_EXTRA_STUDY_DATA             0x0001
#define PCRE_EXTRA_MATCH_LIMIT            0x0002
#define PCRE_EXTRA_CALLOUT_DATA           0x0004
#define PCRE_EXTRA_TABLES                 0x0008
#define PCRE_EXTRA_MATCH_LIMIT_RECURSION  0x0010
#define PCRE_EXTRA_MARK                   0x0020
#define PCRE_EXTRA_EXECUTABLE_JIT         0x0040

/* Types */

struct real_pcre8_or_16;          /* declaration; the definition is private  */
typedef struct real_pcre8_or_16 pcre;

struct real_pcre8_or_16;          /* declaration; the definition is private  */
typedef struct real_pcre8_or_16 pcre16;

struct real_pcre32;               /* declaration; the definition is private  */
typedef struct real_pcre32 pcre32;

struct real_pcre_jit_stack;       /* declaration; the definition is private  */
typedef struct real_pcre_jit_stack pcre_jit_stack;

struct real_pcre16_jit_stack;     /* declaration; the definition is private  */
typedef struct real_pcre16_jit_stack pcre16_jit_stack;

struct real_pcre32_jit_stack;     /* declaration; the definition is private  */
typedef struct real_pcre32_jit_stack pcre32_jit_stack;

/* If PCRE is compiled with 16 bit character support, PCRE_UCHAR16 must contain
a 16 bit wide signed data type. Otherwise it can be a dummy data type since
pcre16 functions are not implemented. There is a check for this in pcre_internal.h. */
#ifndef PCRE_UCHAR16
#define PCRE_UCHAR16 u16
#endif

#ifndef PCRE_SPTR16
#define PCRE_SPTR16 const PCRE_UCHAR16 *
#endif

/* If PCRE is compiled with 32 bit character support, PCRE_UCHAR32 must contain
a 32 bit wide signed data type. Otherwise it can be a dummy data type since
pcre32 functions are not implemented. There is a check for this in pcre_internal.h. */
#ifndef PCRE_UCHAR32
#define PCRE_UCHAR32 u32
#endif

#ifndef PCRE_SPTR32
#define PCRE_SPTR32 const PCRE_UCHAR32 *
#endif

/* When PCRE is compiled as a C++ library, the subject pointer type can be
replaced with a custom type. For conventional use, the public interface is a
tukk . */

#ifndef PCRE_SPTR
#define PCRE_SPTR tukk
#endif

/* The structure for passing additional data to pcre_exec(). This is defined in
such as way as to be extensible. Always add new fields at the end, in order to
remain compatible. */

typedef struct pcre_extra {
  u64 flags;        /* Bits for which fields are set */
  uk study_data;               /* Opaque data from pcre_study() */
  u64 match_limit;  /* Maximum number of calls to match() */
  uk callout_data;             /* Data passed back in callouts */
  u8k* tables;    /* Pointer to character tables */
  u64 match_limit_recursion; /* Max recursive calls to match() */
  u8** mark;           /* For passing back a mark pointer */
  uk executable_jit;           /* Contains a pointer to a compiled jit code */
} pcre_extra;

/* Same structure as above, but with 16 bit char pointers. */

typedef struct pcre16_extra {
  u64 flags;        /* Bits for which fields are set */
  uk study_data;               /* Opaque data from pcre_study() */
  u64 match_limit;  /* Maximum number of calls to match() */
  uk callout_data;             /* Data passed back in callouts */
  u8k* tables;    /* Pointer to character tables */
  u64 match_limit_recursion; /* Max recursive calls to match() */
  PCRE_UCHAR16 **mark;            /* For passing back a mark pointer */
  uk executable_jit;           /* Contains a pointer to a compiled jit code */
} pcre16_extra;

/* Same structure as above, but with 32 bit char pointers. */

typedef struct pcre32_extra {
  u64 flags;        /* Bits for which fields are set */
  uk study_data;               /* Opaque data from pcre_study() */
  u64 match_limit;  /* Maximum number of calls to match() */
  uk callout_data;             /* Data passed back in callouts */
  u8k* tables;    /* Pointer to character tables */
  u64 match_limit_recursion; /* Max recursive calls to match() */
  PCRE_UCHAR32 **mark;            /* For passing back a mark pointer */
  uk executable_jit;           /* Contains a pointer to a compiled jit code */
} pcre32_extra;

/* The structure for passing out data via the pcre_callout_function. We use a
structure so that new fields can be added on the end in future versions,
without changing the API of the function, thereby allowing old clients to work
without modification. */

typedef struct pcre_callout_block {
  i32          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  i32          callout_number;    /* Number compiled into pattern */
  i32         *offset_vector;     /* The offset vector */
  PCRE_SPTR    subject;           /* The subject being matched */
  i32          subject_length;    /* The length of the subject */
  i32          start_match;       /* Offset to start of this match attempt */
  i32          current_position;  /* Where we currently are in the subject */
  i32          capture_top;       /* Max current capture */
  i32          capture_last;      /* Most recently closed capture */
  void        *callout_data;      /* Data passed in with the call */
  /* ------------------- Added for Version 1 -------------------------- */
  i32          pattern_position;  /* Offset to next item in the pattern */
  i32          next_item_length;  /* Length of next item in the pattern */
  /* ------------------- Added for Version 2 -------------------------- */
  u8k* mark;      /* Pointer to current mark or NULL    */
  /* ------------------------------------------------------------------ */
} pcre_callout_block;

/* Same structure as above, but with 16 bit char pointers. */

typedef struct pcre16_callout_block {
  i32          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  i32          callout_number;    /* Number compiled into pattern */
  i32         *offset_vector;     /* The offset vector */
  PCRE_SPTR16  subject;           /* The subject being matched */
  i32          subject_length;    /* The length of the subject */
  i32          start_match;       /* Offset to start of this match attempt */
  i32          current_position;  /* Where we currently are in the subject */
  i32          capture_top;       /* Max current capture */
  i32          capture_last;      /* Most recently closed capture */
  void        *callout_data;      /* Data passed in with the call */
  /* ------------------- Added for Version 1 -------------------------- */
  i32          pattern_position;  /* Offset to next item in the pattern */
  i32          next_item_length;  /* Length of next item in the pattern */
  /* ------------------- Added for Version 2 -------------------------- */
  const PCRE_UCHAR16 *mark;       /* Pointer to current mark or NULL    */
  /* ------------------------------------------------------------------ */
} pcre16_callout_block;

/* Same structure as above, but with 32 bit char pointers. */

typedef struct pcre32_callout_block {
  i32          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  i32          callout_number;    /* Number compiled into pattern */
  i32         *offset_vector;     /* The offset vector */
  PCRE_SPTR32  subject;           /* The subject being matched */
  i32          subject_length;    /* The length of the subject */
  i32          start_match;       /* Offset to start of this match attempt */
  i32          current_position;  /* Where we currently are in the subject */
  i32          capture_top;       /* Max current capture */
  i32          capture_last;      /* Most recently closed capture */
  void        *callout_data;      /* Data passed in with the call */
  /* ------------------- Added for Version 1 -------------------------- */
  i32          pattern_position;  /* Offset to next item in the pattern */
  i32          next_item_length;  /* Length of next item in the pattern */
  /* ------------------- Added for Version 2 -------------------------- */
  const PCRE_UCHAR32 *mark;       /* Pointer to current mark or NULL    */
  /* ------------------------------------------------------------------ */
} pcre32_callout_block;

/* Indirection for store get and free functions. These can be set to
alternative malloc/free functions if required. Special ones are used in the
non-recursive case for "frames". There is also an optional callout function
that is triggered by the (?) regex item. For Virtual Pascal, these definitions
have to take another form. */

#ifndef VPCOMPAT
PCRE_EXP_DECL uk (*pcre_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre_free)(uk);
PCRE_EXP_DECL uk (*pcre_stack_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre_stack_free)(uk);
PCRE_EXP_DECL i32   (*pcre_callout)(pcre_callout_block*);
PCRE_EXP_DECL i32   (*pcre_stack_guard)(void);

PCRE_EXP_DECL uk (*pcre16_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre16_free)(uk);
PCRE_EXP_DECL uk (*pcre16_stack_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre16_stack_free)(uk);
PCRE_EXP_DECL i32   (*pcre16_callout)(pcre16_callout_block*);
PCRE_EXP_DECL i32   (*pcre16_stack_guard)(void);

PCRE_EXP_DECL uk (*pcre32_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre32_free)(uk);
PCRE_EXP_DECL uk (*pcre32_stack_malloc)(size_t);
PCRE_EXP_DECL void  (*pcre32_stack_free)(uk);
PCRE_EXP_DECL i32   (*pcre32_callout)(pcre32_callout_block*);
PCRE_EXP_DECL i32   (*pcre32_stack_guard)(void);
#else   /* VPCOMPAT */
PCRE_EXP_DECL uk pcre_malloc(size_t);
PCRE_EXP_DECL void  pcre_free(uk);
PCRE_EXP_DECL uk pcre_stack_malloc(size_t);
PCRE_EXP_DECL void  pcre_stack_free(uk);
PCRE_EXP_DECL i32   pcre_callout(pcre_callout_block*);
PCRE_EXP_DECL i32   pcre_stack_guard(void);

PCRE_EXP_DECL uk pcre16_malloc(size_t);
PCRE_EXP_DECL void  pcre16_free(uk);
PCRE_EXP_DECL uk pcre16_stack_malloc(size_t);
PCRE_EXP_DECL void  pcre16_stack_free(uk);
PCRE_EXP_DECL i32   pcre16_callout(pcre16_callout_block*);
PCRE_EXP_DECL i32   pcre16_stack_guard(void);

PCRE_EXP_DECL uk pcre32_malloc(size_t);
PCRE_EXP_DECL void  pcre32_free(uk);
PCRE_EXP_DECL uk pcre32_stack_malloc(size_t);
PCRE_EXP_DECL void  pcre32_stack_free(uk);
PCRE_EXP_DECL i32   pcre32_callout(pcre32_callout_block*);
PCRE_EXP_DECL i32   pcre32_stack_guard(void);
#endif  /* VPCOMPAT */

/* User defined callback which provides a stack just before the match starts. */

typedef pcre_jit_stack *(*pcre_jit_callback)(uk);
typedef pcre16_jit_stack *(*pcre16_jit_callback)(uk);
typedef pcre32_jit_stack *(*pcre32_jit_callback)(uk);

/* Exported PCRE functions */

PCRE_EXP_DECL pcre *pcre_compile(tukk , i32, tukk *, i32 *,
                  u8k*);
PCRE_EXP_DECL pcre16 *pcre16_compile(PCRE_SPTR16, i32, tukk *, i32 *,
                  u8k*);
PCRE_EXP_DECL pcre32 *pcre32_compile(PCRE_SPTR32, i32, tukk *, i32 *,
                  u8k*);
PCRE_EXP_DECL pcre *pcre_compile2(tukk , i32, i32 *, tukk *,
                  i32*, u8k*);
PCRE_EXP_DECL pcre16 *pcre16_compile2(PCRE_SPTR16, i32, i32 *, tukk *,
                  i32*, u8k*);
PCRE_EXP_DECL pcre32 *pcre32_compile2(PCRE_SPTR32, i32, i32 *, tukk *,
                  i32*, u8k*);
PCRE_EXP_DECL i32  pcre_config(i32, uk );
PCRE_EXP_DECL i32  pcre16_config(i32, uk );
PCRE_EXP_DECL i32  pcre32_config(i32, uk );
PCRE_EXP_DECL i32  pcre_copy_named_substring(const pcre *, tukk ,
                  i32*, i32, tukk , tuk , i32);
PCRE_EXP_DECL i32  pcre16_copy_named_substring(const pcre16 *, PCRE_SPTR16,
                  i32*, i32, PCRE_SPTR16, PCRE_UCHAR16 *, i32);
PCRE_EXP_DECL i32  pcre32_copy_named_substring(const pcre32 *, PCRE_SPTR32,
                  i32 *, i32, PCRE_SPTR32, PCRE_UCHAR32 *, i32);
PCRE_EXP_DECL i32  pcre_copy_substring(tukk , i32 *, i32, i32,
                  tuk , i32);
PCRE_EXP_DECL i32  pcre16_copy_substring(PCRE_SPTR16, i32 *, i32, i32,
                  PCRE_UCHAR16 *, i32);
PCRE_EXP_DECL i32  pcre32_copy_substring(PCRE_SPTR32, i32 *, i32, i32,
                  PCRE_UCHAR32 *, i32);
PCRE_EXP_DECL i32  pcre_dfa_exec(const pcre *, const pcre_extra *,
                  tukk , i32, i32, i32, i32 *, i32 , i32 *, i32);
PCRE_EXP_DECL i32  pcre16_dfa_exec(const pcre16 *, const pcre16_extra *,
                  PCRE_SPTR16, i32, i32, i32, i32 *, i32 , i32 *, i32);
PCRE_EXP_DECL i32  pcre32_dfa_exec(const pcre32 *, const pcre32_extra *,
                  PCRE_SPTR32, i32, i32, i32, i32 *, i32 , i32 *, i32);
PCRE_EXP_DECL i32  pcre_exec(const pcre *, const pcre_extra *, PCRE_SPTR,
                   i32, i32, i32, i32 *, i32);
PCRE_EXP_DECL i32  pcre16_exec(const pcre16 *, const pcre16_extra *,
                   PCRE_SPTR16, i32, i32, i32, i32 *, i32);
PCRE_EXP_DECL i32  pcre32_exec(const pcre32 *, const pcre32_extra *,
                   PCRE_SPTR32, i32, i32, i32, i32 *, i32);
PCRE_EXP_DECL i32  pcre_jit_exec(const pcre *, const pcre_extra *,
                   PCRE_SPTR, i32, i32, i32, i32 *, i32,
                   pcre_jit_stack*);
PCRE_EXP_DECL i32  pcre16_jit_exec(const pcre16 *, const pcre16_extra *,
                   PCRE_SPTR16, i32, i32, i32, i32 *, i32,
                   pcre16_jit_stack*);
PCRE_EXP_DECL i32  pcre32_jit_exec(const pcre32 *, const pcre32_extra *,
                   PCRE_SPTR32, i32, i32, i32, i32 *, i32,
                   pcre32_jit_stack*);
PCRE_EXP_DECL void pcre_free_substring(tukk);
PCRE_EXP_DECL void pcre16_free_substring(PCRE_SPTR16);
PCRE_EXP_DECL void pcre32_free_substring(PCRE_SPTR32);
PCRE_EXP_DECL void pcre_free_substring_list(tukk*);
PCRE_EXP_DECL void pcre16_free_substring_list(PCRE_SPTR16*);
PCRE_EXP_DECL void pcre32_free_substring_list(PCRE_SPTR32*);
PCRE_EXP_DECL i32  pcre_fullinfo(const pcre *, const pcre_extra *, i32,
                  uk );
PCRE_EXP_DECL i32  pcre16_fullinfo(const pcre16 *, const pcre16_extra *, i32,
                  uk );
PCRE_EXP_DECL i32  pcre32_fullinfo(const pcre32 *, const pcre32_extra *, i32,
                  uk );
PCRE_EXP_DECL i32  pcre_get_named_substring(const pcre *, tukk ,
                  i32 *, i32, tukk , tukk*);
PCRE_EXP_DECL i32  pcre16_get_named_substring(const pcre16 *, PCRE_SPTR16,
                  i32 *, i32, PCRE_SPTR16, PCRE_SPTR16*);
PCRE_EXP_DECL i32  pcre32_get_named_substring(const pcre32 *, PCRE_SPTR32,
                  i32 *, i32, PCRE_SPTR32, PCRE_SPTR32*);
PCRE_EXP_DECL i32  pcre_get_stringnumber(const pcre *, tukk );
PCRE_EXP_DECL i32  pcre16_get_stringnumber(const pcre16 *, PCRE_SPTR16);
PCRE_EXP_DECL i32  pcre32_get_stringnumber(const pcre32 *, PCRE_SPTR32);
PCRE_EXP_DECL i32  pcre_get_stringtable_entries(const pcre *, tukk ,
                  tuk* , tuk*);
PCRE_EXP_DECL i32  pcre16_get_stringtable_entries(const pcre16 *, PCRE_SPTR16,
                  PCRE_UCHAR16 **, PCRE_UCHAR16 **);
PCRE_EXP_DECL i32  pcre32_get_stringtable_entries(const pcre32 *, PCRE_SPTR32,
                  PCRE_UCHAR32 **, PCRE_UCHAR32 **);
PCRE_EXP_DECL i32  pcre_get_substring(tukk , i32 *, i32, i32,
                  tukk*);
PCRE_EXP_DECL i32  pcre16_get_substring(PCRE_SPTR16, i32 *, i32, i32,
                  PCRE_SPTR16*);
PCRE_EXP_DECL i32  pcre32_get_substring(PCRE_SPTR32, i32 *, i32, i32,
                  PCRE_SPTR32*);
PCRE_EXP_DECL i32  pcre_get_substring_list(tukk , i32 *, i32,
                  tukk **);
PCRE_EXP_DECL i32  pcre16_get_substring_list(PCRE_SPTR16, i32 *, i32,
                  PCRE_SPTR16 **);
PCRE_EXP_DECL i32  pcre32_get_substring_list(PCRE_SPTR32, i32 *, i32,
                  PCRE_SPTR32 **);
PCRE_EXP_DECL u8k* pcre_maketables(void);
PCRE_EXP_DECL u8k* pcre16_maketables(void);
PCRE_EXP_DECL u8k* pcre32_maketables(void);
PCRE_EXP_DECL i32  pcre_refcount(pcre *, i32);
PCRE_EXP_DECL i32  pcre16_refcount(pcre16 *, i32);
PCRE_EXP_DECL i32  pcre32_refcount(pcre32 *, i32);
PCRE_EXP_DECL pcre_extra *pcre_study(const pcre *, i32, tukk*);
PCRE_EXP_DECL pcre16_extra *pcre16_study(const pcre16 *, i32, tukk*);
PCRE_EXP_DECL pcre32_extra *pcre32_study(const pcre32 *, i32, tukk*);
PCRE_EXP_DECL void pcre_free_study(pcre_extra*);
PCRE_EXP_DECL void pcre16_free_study(pcre16_extra*);
PCRE_EXP_DECL void pcre32_free_study(pcre32_extra*);
PCRE_EXP_DECL tukk pcre_version(void);
PCRE_EXP_DECL tukk pcre16_version(void);
PCRE_EXP_DECL tukk pcre32_version(void);

/* Utility functions for u8 order swaps. */
PCRE_EXP_DECL i32  pcre_pattern_to_host_byte_order(pcre *, pcre_extra *,
                  u8k*);
PCRE_EXP_DECL i32  pcre16_pattern_to_host_byte_order(pcre16 *, pcre16_extra *,
                  u8k*);
PCRE_EXP_DECL i32  pcre32_pattern_to_host_byte_order(pcre32 *, pcre32_extra *,
                  u8k*);
PCRE_EXP_DECL i32  pcre16_utf16_to_host_byte_order(PCRE_UCHAR16 *,
                  PCRE_SPTR16, i32, i32 *, i32);
PCRE_EXP_DECL i32  pcre32_utf32_to_host_byte_order(PCRE_UCHAR32 *,
                  PCRE_SPTR32, i32, i32 *, i32);

/* JIT compiler related functions. */

PCRE_EXP_DECL pcre_jit_stack *pcre_jit_stack_alloc(i32, i32);
PCRE_EXP_DECL pcre16_jit_stack *pcre16_jit_stack_alloc(i32, i32);
PCRE_EXP_DECL pcre32_jit_stack *pcre32_jit_stack_alloc(i32, i32);
PCRE_EXP_DECL void pcre_jit_stack_free(pcre_jit_stack*);
PCRE_EXP_DECL void pcre16_jit_stack_free(pcre16_jit_stack*);
PCRE_EXP_DECL void pcre32_jit_stack_free(pcre32_jit_stack*);
PCRE_EXP_DECL void pcre_assign_jit_stack(pcre_extra *,
                  pcre_jit_callback, uk );
PCRE_EXP_DECL void pcre16_assign_jit_stack(pcre16_extra *,
                  pcre16_jit_callback, uk );
PCRE_EXP_DECL void pcre32_assign_jit_stack(pcre32_extra *,
                  pcre32_jit_callback, uk );
PCRE_EXP_DECL void pcre_jit_free_unused_memory(void);
PCRE_EXP_DECL void pcre16_jit_free_unused_memory(void);
PCRE_EXP_DECL void pcre32_jit_free_unused_memory(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* End of pcre.h */
