
#ifndef CCD_EXPORT_H
#define CCD_EXPORT_H

#ifdef CCD_STATIC_DEFINE
#  define CCD_EXPORT
#  define CCD_NO_EXPORT
#else
#  ifndef CCD_EXPORT
#    ifdef ccd_EXPORTS
        /* We are building this library */
#      define CCD_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define CCD_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef CCD_NO_EXPORT
#    define CCD_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef CCD_DEPRECATED
#  define CCD_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CCD_DEPRECATED_EXPORT
#  define CCD_DEPRECATED_EXPORT CCD_EXPORT CCD_DEPRECATED
#endif

#ifndef CCD_DEPRECATED_NO_EXPORT
#  define CCD_DEPRECATED_NO_EXPORT CCD_NO_EXPORT CCD_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define CCD_NO_DEPRECATED
#endif

#endif
