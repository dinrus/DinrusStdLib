
#ifndef KGUIADDONS_EXPORT_H
#define KGUIADDONS_EXPORT_H

#ifdef KGUIADDONS_STATIC_DEFINE
#  define KGUIADDONS_EXPORT
#  define KGUIADDONS_NO_EXPORT
#else
#  ifndef KGUIADDONS_EXPORT
#    ifdef KF5GuiAddons_EXPORTS
        /* We are building this library */
#      define KGUIADDONS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define KGUIADDONS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef KGUIADDONS_NO_EXPORT
#    define KGUIADDONS_NO_EXPORT 
#  endif
#endif

#ifndef KGUIADDONS_DEPRECATED
#  define KGUIADDONS_DEPRECATED __declspec(deprecated)
#endif

#ifndef KGUIADDONS_DEPRECATED_EXPORT
#  define KGUIADDONS_DEPRECATED_EXPORT KGUIADDONS_EXPORT KGUIADDONS_DEPRECATED
#endif

#ifndef KGUIADDONS_DEPRECATED_NO_EXPORT
#  define KGUIADDONS_DEPRECATED_NO_EXPORT KGUIADDONS_NO_EXPORT KGUIADDONS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KGUIADDONS_NO_DEPRECATED
#    define KGUIADDONS_NO_DEPRECATED
#  endif
#endif

#endif /* KGUIADDONS_EXPORT_H */
