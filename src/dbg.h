#ifndef _GJK_DBG_H_
#define _GJK_DBG_H_

/**
 * Some macros which can be used for printing debug info to stderr if macro
 * NDEBUG not defined.
 *
 * DBG_PROLOGUE can be specified as string and this string will be
 * prepended to output text
 */
#ifndef NDEBUG

#include <stdio.h>

#ifndef DBG_PROLOGUE
# define DBG_PROLOGUE
#endif

# define DBG(format, ...) do { \
    fprintf(stderr, DBG_PROLOGUE "%s :: " format "\n", __func__, ## __VA_ARGS__); \
    fflush(stderr); \
    } while (0)

# define DBG2(str) do { \
    fprintf(stderr, DBG_PROLOGUE "%s :: " str "\n", __func__); \
    fflush(stderr); \
    } while (0)

# define DBG_VEC3(vec, prefix) do {\
    fprintf(stderr, DBG_PROLOGUE "%s :: %s[%lf %lf %lf]\n", \
            __func__, prefix, gjkVec3X(vec), gjkVec3Y(vec), gjkVec3Z(vec)); \
    fflush(stderr); \
    } while (0)

#else
# define DBG(format, ...)
# define DBG2(str)
# define DBG_VEC3(v, prefix)
#endif

#endif /* _GJK_DBG_H_ */
