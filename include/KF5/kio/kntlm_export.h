
#ifndef KNTLM_EXPORT_H
#define KNTLM_EXPORT_H

#ifdef KNTLM_STATIC_DEFINE
#  define KNTLM_EXPORT
#  define KNTLM_NO_EXPORT
#else
#  ifndef KNTLM_EXPORT
#    ifdef KF5KIONTLM_EXPORTS
        /* We are building this library */
#      define KNTLM_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define KNTLM_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef KNTLM_NO_EXPORT
#    define KNTLM_NO_EXPORT 
#  endif
#endif

#ifndef KNTLM_DECL_DEPRECATED
#  define KNTLM_DECL_DEPRECATED __declspec(deprecated)
#endif

#ifndef KNTLM_DECL_DEPRECATED_EXPORT
#  define KNTLM_DECL_DEPRECATED_EXPORT KNTLM_EXPORT KNTLM_DECL_DEPRECATED
#endif

#ifndef KNTLM_DECL_DEPRECATED_NO_EXPORT
#  define KNTLM_DECL_DEPRECATED_NO_EXPORT KNTLM_NO_EXPORT KNTLM_DECL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KNTLM_NO_DEPRECATED
#    define KNTLM_NO_DEPRECATED
#  endif
#endif

#define KNTLM_DECL_DEPRECATED_TEXT(text) __attribute__ ((__deprecated__(text)))

/* Take any defaults from group settings */
#if !defined(KNTLM_NO_DEPRECATED) && !defined(KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  ifdef KF_NO_DEPRECATED
#    define KNTLM_NO_DEPRECATED
#  elif defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#    define KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#  endif
#endif
#if !defined(KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT) && defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  define KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#endif

#if !defined(KNTLM_NO_DEPRECATED_WARNINGS) && !defined(KNTLM_DEPRECATED_WARNINGS_SINCE)
#  ifdef KF_NO_DEPRECATED_WARNINGS
#    define KNTLM_NO_DEPRECATED_WARNINGS
#  elif defined(KF_DEPRECATED_WARNINGS_SINCE)
#    define KNTLM_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#  endif
#endif
#if !defined(KNTLM_DEPRECATED_WARNINGS_SINCE) && defined(KF_DEPRECATED_WARNINGS_SINCE)
#  define KNTLM_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#endif

#if defined(KNTLM_NO_DEPRECATED)
#  undef KNTLM_DEPRECATED
#  define KNTLM_DEPRECATED_EXPORT KNTLM_EXPORT
#  define KNTLM_DEPRECATED_NO_EXPORT KNTLM_NO_EXPORT
#elif defined(KNTLM_NO_DEPRECATED_WARNINGS)
#  define KNTLM_DEPRECATED
#  define KNTLM_DEPRECATED_EXPORT KNTLM_EXPORT
#  define KNTLM_DEPRECATED_NO_EXPORT KNTLM_NO_EXPORT
#else
#  define KNTLM_DEPRECATED KNTLM_DECL_DEPRECATED
#  define KNTLM_DEPRECATED_EXPORT KNTLM_DECL_DEPRECATED_EXPORT
#  define KNTLM_DEPRECATED_NO_EXPORT KNTLM_DECL_DEPRECATED_NO_EXPORT
#endif

/* No deprecated API had been removed from build */
#define KNTLM_EXCLUDE_DEPRECATED_BEFORE_AND_AT 0

#define KNTLM_BUILD_DEPRECATED_SINCE(major, minor) 1

#ifdef KNTLM_NO_DEPRECATED
#  define KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT 0x56e00
#endif
#ifdef KNTLM_NO_DEPRECATED_WARNINGS
#  define KNTLM_DEPRECATED_WARNINGS_SINCE 0
#endif

#ifndef KNTLM_DEPRECATED_WARNINGS_SINCE
#  ifdef KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT
#    define KNTLM_DEPRECATED_WARNINGS_SINCE KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT
#  else
#    define KNTLM_DEPRECATED_WARNINGS_SINCE 0x56e00
#  endif
#endif

#ifndef KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT
#  define KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT 0
#endif

#ifdef KNTLM_DEPRECATED
#  define KNTLM_ENABLE_DEPRECATED_SINCE(major, minor) (((major<<16)|(minor<<8)) > KNTLM_DISABLE_DEPRECATED_BEFORE_AND_AT)
#else
#  define KNTLM_ENABLE_DEPRECATED_SINCE(major, minor) 0
#endif

#if KNTLM_DEPRECATED_WARNINGS_SINCE >= 0x55b00
#  define KNTLM_DEPRECATED_VERSION_5_91(text) KNTLM_DECL_DEPRECATED_TEXT(text)
#else
#  define KNTLM_DEPRECATED_VERSION_5_91(text)
#endif
#define KNTLM_DEPRECATED_VERSION_5(minor, text)      KNTLM_DEPRECATED_VERSION_5_##minor(text)
#define KNTLM_DEPRECATED_VERSION(major, minor, text) KNTLM_DEPRECATED_VERSION_##major(minor, "Since "#major"."#minor". " text)
#define KNTLM_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) KNTLM_DEPRECATED_VERSION_##major(minor, "Since "#textmajor"."#textminor". " text)
#if defined(__cpp_enumerator_attributes) && __cpp_enumerator_attributes >= 201411
#  define KNTLM_ENUMERATOR_DEPRECATED_VERSION(major, minor, text) KNTLM_DEPRECATED_VERSION(major, minor, text)
#  define KNTLM_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) KNTLM_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#else
#  define KNTLM_ENUMERATOR_DEPRECATED_VERSION(major, minor, text)
#  define KNTLM_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#endif

#endif /* KNTLM_EXPORT_H */
