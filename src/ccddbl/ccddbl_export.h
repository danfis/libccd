#ifndef CCDDBL_EXPORT_H
#define CCDDBL_EXPORT_H

#ifdef CCDDBL_STATIC_DEFINE
#  define CCDDBL_EXPORT
#else
#  ifdef _MSC_VER
#    ifdef ccddbl_EXPORTS
#      define CCDDBL_EXPORT __declspec(dllexport)
#   else /* ccddbl_EXPORTS */
#     define CCDDBL_EXPORT  __declspec(dllimport)
#   endif /* ccddbl_EXPORTS */
#  else
#    ifndef CCDDBL_EXPORT
#      ifdef ccddbl_EXPORTS
          /* We are building this library */
#        define CCDDBL_EXPORT __attribute__((visibility("default")))
#      else
          /* We are using this library */
#        define CCDDBL_EXPORT __attribute__((visibility("default")))
#      endif
#    endif
#  endif
#endif

#endif
