
#ifndef KAUTH_EXPORT_H
#define KAUTH_EXPORT_H

#ifdef KAUTH_STATIC_DEFINE
#  define KAUTH_EXPORT
#  define KAUTH_NO_EXPORT
#else
#  ifndef KAUTH_EXPORT
#    ifdef KF5AuthWidgets_EXPORTS
        /* We are building this library */
#      define KAUTH_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define KAUTH_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef KAUTH_NO_EXPORT
#    define KAUTH_NO_EXPORT 
#  endif
#endif

#ifndef KAUTH_DECL_DEPRECATED
#  define KAUTH_DECL_DEPRECATED __declspec(deprecated)
#endif

#ifndef KAUTH_DECL_DEPRECATED_EXPORT
#  define KAUTH_DECL_DEPRECATED_EXPORT KAUTH_EXPORT KAUTH_DECL_DEPRECATED
#endif

#ifndef KAUTH_DECL_DEPRECATED_NO_EXPORT
#  define KAUTH_DECL_DEPRECATED_NO_EXPORT KAUTH_NO_EXPORT KAUTH_DECL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KAUTH_NO_DEPRECATED
#    define KAUTH_NO_DEPRECATED
#  endif
#endif

#define KAUTH_DECL_DEPRECATED_TEXT(text) __attribute__ ((__deprecated__(text)))

/* Take any defaults from group settings */
#if !defined(KAUTH_NO_DEPRECATED) && !defined(KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  ifdef KF_NO_DEPRECATED
#    define KAUTH_NO_DEPRECATED
#  elif defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#    define KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#  endif
#endif
#if !defined(KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT) && defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  define KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#endif

#if !defined(KAUTH_NO_DEPRECATED_WARNINGS) && !defined(KAUTH_DEPRECATED_WARNINGS_SINCE)
#  ifdef KF_NO_DEPRECATED_WARNINGS
#    define KAUTH_NO_DEPRECATED_WARNINGS
#  elif defined(KF_DEPRECATED_WARNINGS_SINCE)
#    define KAUTH_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#  endif
#endif
#if !defined(KAUTH_DEPRECATED_WARNINGS_SINCE) && defined(KF_DEPRECATED_WARNINGS_SINCE)
#  define KAUTH_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#endif

#if defined(KAUTH_NO_DEPRECATED)
#  undef KAUTH_DEPRECATED
#  define KAUTH_DEPRECATED_EXPORT KAUTH_EXPORT
#  define KAUTH_DEPRECATED_NO_EXPORT KAUTH_NO_EXPORT
#elif defined(KAUTH_NO_DEPRECATED_WARNINGS)
#  define KAUTH_DEPRECATED
#  define KAUTH_DEPRECATED_EXPORT KAUTH_EXPORT
#  define KAUTH_DEPRECATED_NO_EXPORT KAUTH_NO_EXPORT
#else
#  define KAUTH_DEPRECATED KAUTH_DECL_DEPRECATED
#  define KAUTH_DEPRECATED_EXPORT KAUTH_DECL_DEPRECATED_EXPORT
#  define KAUTH_DEPRECATED_NO_EXPORT KAUTH_DECL_DEPRECATED_NO_EXPORT
#endif

/* No deprecated API had been removed from build */
#define KAUTH_EXCLUDE_DEPRECATED_BEFORE_AND_AT 0

#define KAUTH_BUILD_DEPRECATED_SINCE(major, minor) 1

#ifdef KAUTH_NO_DEPRECATED
#  define KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT 0x56e00
#endif
#ifdef KAUTH_NO_DEPRECATED_WARNINGS
#  define KAUTH_DEPRECATED_WARNINGS_SINCE 0
#endif

#ifndef KAUTH_DEPRECATED_WARNINGS_SINCE
#  ifdef KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT
#    define KAUTH_DEPRECATED_WARNINGS_SINCE KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT
#  else
#    define KAUTH_DEPRECATED_WARNINGS_SINCE 0x56e00
#  endif
#endif

#ifndef KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT
#  define KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT 0
#endif

#ifdef KAUTH_DEPRECATED
#  define KAUTH_ENABLE_DEPRECATED_SINCE(major, minor) (((major<<16)|(minor<<8)) > KAUTH_DISABLE_DEPRECATED_BEFORE_AND_AT)
#else
#  define KAUTH_ENABLE_DEPRECATED_SINCE(major, minor) 0
#endif

#if KAUTH_DEPRECATED_WARNINGS_SINCE >= 0x55c00
#  define KAUTH_DEPRECATED_VERSION_5_92(text) KAUTH_DECL_DEPRECATED_TEXT(text)
#else
#  define KAUTH_DEPRECATED_VERSION_5_92(text)
#endif
#define KAUTH_DEPRECATED_VERSION_5(minor, text)      KAUTH_DEPRECATED_VERSION_5_##minor(text)
#define KAUTH_DEPRECATED_VERSION(major, minor, text) KAUTH_DEPRECATED_VERSION_##major(minor, "Since "#major"."#minor". " text)
#define KAUTH_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) KAUTH_DEPRECATED_VERSION_##major(minor, "Since "#textmajor"."#textminor". " text)
#if defined(__cpp_enumerator_attributes) && __cpp_enumerator_attributes >= 201411
#  define KAUTH_ENUMERATOR_DEPRECATED_VERSION(major, minor, text) KAUTH_DEPRECATED_VERSION(major, minor, text)
#  define KAUTH_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) KAUTH_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#else
#  define KAUTH_ENUMERATOR_DEPRECATED_VERSION(major, minor, text)
#  define KAUTH_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#endif

#endif /* KAUTH_EXPORT_H */
