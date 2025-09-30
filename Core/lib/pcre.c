#define SUPPORT_UTF
#define SUPPORT_UTF8	//is needed anymore?
#define SUPPORT_UCP
#define HAVE_CONFIG_H

#define PCRE_STATIC

#include "pcre/pcre_byte_order.c"
#include "pcre/pcre_chartables.c"
#include "pcre/pcre_compile.c"
#undef PSSTART
#undef PSEND
#undef NLBLOCK
#include "pcre/pcre_config.c"
#include "pcre/pcre_dfa_exec.c"
#include "pcre/pcre_exec.c"
#include "pcre/pcre_fullinfo.c"
#include "pcre/pcre_get.c"
#include "pcre/pcre_globals.c"
#include "pcre/pcre_jit_compile.c"
#include "pcre/pcre_maketables.c"
#include "pcre/pcre_newline.c"
#include "pcre/pcre_ord2utf8.c"
#include "pcre/pcre_refcount.c"
#include "pcre/pcre_string_utils.c"
#include "pcre/pcre_study.c"
#include "pcre/pcre_tables.c"
#include "pcre/pcre_ucd.c"
#include "pcre/pcre_valid_utf8.c"
#include "pcre/pcre_version.c"
#include "pcre/pcre_xclass.c"
