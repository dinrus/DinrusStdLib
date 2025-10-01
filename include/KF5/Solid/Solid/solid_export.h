
#ifndef SOLID_EXPORT_H
#define SOLID_EXPORT_H

#ifdef SOLID_STATIC_DEFINE
#  define SOLID_EXPORT
#  define SOLID_NO_EXPORT
#else
#  ifndef SOLID_EXPORT
#    ifdef KF5Solid_EXPORTS
        /* We are building this library */
#      define SOLID_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define SOLID_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef SOLID_NO_EXPORT
#    define SOLID_NO_EXPORT 
#  endif
#endif

#ifndef SOLID_DECL_DEPRECATED
#  define SOLID_DECL_DEPRECATED __declspec(deprecated)
#endif

#ifndef SOLID_DECL_DEPRECATED_EXPORT
#  define SOLID_DECL_DEPRECATED_EXPORT SOLID_EXPORT SOLID_DECL_DEPRECATED
#endif

#ifndef SOLID_DECL_DEPRECATED_NO_EXPORT
#  define SOLID_DECL_DEPRECATED_NO_EXPORT SOLID_NO_EXPORT SOLID_DECL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SOLID_NO_DEPRECATED
#    define SOLID_NO_DEPRECATED
#  endif
#endif

#define SOLID_DECL_DEPRECATED_TEXT(text) __attribute__ ((__deprecated__(text)))

/* Take any defaults from group settings */
#if !defined(SOLID_NO_DEPRECATED) && !defined(SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  ifdef KF_NO_DEPRECATED
#    define SOLID_NO_DEPRECATED
#  elif defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#    define SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#  endif
#endif
#if !defined(SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT) && defined(KF_DISABLE_DEPRECATED_BEFORE_AND_AT)
#  define SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT KF_DISABLE_DEPRECATED_BEFORE_AND_AT
#endif

#if !defined(SOLID_NO_DEPRECATED_WARNINGS) && !defined(SOLID_DEPRECATED_WARNINGS_SINCE)
#  ifdef KF_NO_DEPRECATED_WARNINGS
#    define SOLID_NO_DEPRECATED_WARNINGS
#  elif defined(KF_DEPRECATED_WARNINGS_SINCE)
#    define SOLID_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#  endif
#endif
#if !defined(SOLID_DEPRECATED_WARNINGS_SINCE) && defined(KF_DEPRECATED_WARNINGS_SINCE)
#  define SOLID_DEPRECATED_WARNINGS_SINCE KF_DEPRECATED_WARNINGS_SINCE
#endif

#if defined(SOLID_NO_DEPRECATED)
#  undef SOLID_DEPRECATED
#  define SOLID_DEPRECATED_EXPORT SOLID_EXPORT
#  define SOLID_DEPRECATED_NO_EXPORT SOLID_NO_EXPORT
#elif defined(SOLID_NO_DEPRECATED_WARNINGS)
#  define SOLID_DEPRECATED
#  define SOLID_DEPRECATED_EXPORT SOLID_EXPORT
#  define SOLID_DEPRECATED_NO_EXPORT SOLID_NO_EXPORT
#else
#  define SOLID_DEPRECATED SOLID_DECL_DEPRECATED
#  define SOLID_DEPRECATED_EXPORT SOLID_DECL_DEPRECATED_EXPORT
#  define SOLID_DEPRECATED_NO_EXPORT SOLID_DECL_DEPRECATED_NO_EXPORT
#endif

/* No deprecated API had been removed from build */
#define SOLID_EXCLUDE_DEPRECATED_BEFORE_AND_AT 0

#define SOLID_BUILD_DEPRECATED_SINCE(major, minor) 1

#ifdef SOLID_NO_DEPRECATED
#  define SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT 0x56e00
#endif
#ifdef SOLID_NO_DEPRECATED_WARNINGS
#  define SOLID_DEPRECATED_WARNINGS_SINCE 0
#endif

#ifndef SOLID_DEPRECATED_WARNINGS_SINCE
#  ifdef SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT
#    define SOLID_DEPRECATED_WARNINGS_SINCE SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT
#  else
#    define SOLID_DEPRECATED_WARNINGS_SINCE 0x56e00
#  endif
#endif

#ifndef SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT
#  define SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT 0
#endif

#ifdef SOLID_DEPRECATED
#  define SOLID_ENABLE_DEPRECATED_SINCE(major, minor) (((major<<16)|(minor<<8)) > SOLID_DISABLE_DEPRECATED_BEFORE_AND_AT)
#else
#  define SOLID_ENABLE_DEPRECATED_SINCE(major, minor) 0
#endif

#if SOLID_DEPRECATED_WARNINGS_SINCE >= 0x50000
#  define SOLID_DEPRECATED_VERSION_5_0(text) SOLID_DECL_DEPRECATED_TEXT(text)
#else
#  define SOLID_DEPRECATED_VERSION_5_0(text)
#endif

#if SOLID_DEPRECATED_WARNINGS_SINCE >= 0x56900
#  define SOLID_DEPRECATED_VERSION_5_105(text) SOLID_DECL_DEPRECATED_TEXT(text)
#else
#  define SOLID_DEPRECATED_VERSION_5_105(text)
#endif
#define SOLID_DEPRECATED_VERSION_5(minor, text)      SOLID_DEPRECATED_VERSION_5_##minor(text)
#define SOLID_DEPRECATED_VERSION(major, minor, text) SOLID_DEPRECATED_VERSION_##major(minor, "Since "#major"."#minor". " text)
#define SOLID_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) SOLID_DEPRECATED_VERSION_##major(minor, "Since "#textmajor"."#textminor". " text)
#if defined(__cpp_enumerator_attributes) && __cpp_enumerator_attributes >= 201411
#  define SOLID_ENUMERATOR_DEPRECATED_VERSION(major, minor, text) SOLID_DEPRECATED_VERSION(major, minor, text)
#  define SOLID_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text) SOLID_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#else
#  define SOLID_ENUMERATOR_DEPRECATED_VERSION(major, minor, text)
#  define SOLID_ENUMERATOR_DEPRECATED_VERSION_BELATED(major, minor, textmajor, textminor, text)
#endif

#endif /* SOLID_EXPORT_H */
