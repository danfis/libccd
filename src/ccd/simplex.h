/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libccddbl.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#ifndef __CCDDBL_SIMPLEX_H__
#define __CCDDBL_SIMPLEX_H__

#include <ccddbl/compiler.h>
#include <ccddbl/support.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _ccddbl_simplex_t {
    ccddbl_support_t ps[4];
    int last; //!< index of last added point
};
typedef struct _ccddbl_simplex_t ccddbl_simplex_t;


_ccddbl_inline void ccddblSimplexInit(ccddbl_simplex_t *s);
_ccddbl_inline int ccddblSimplexSize(const ccddbl_simplex_t *s);
_ccddbl_inline const ccddbl_support_t *ccddblSimplexLast(const ccddbl_simplex_t *s);
_ccddbl_inline const ccddbl_support_t *ccddblSimplexPoint(const ccddbl_simplex_t *s, int idx);
_ccddbl_inline ccddbl_support_t *ccddblSimplexPointW(ccddbl_simplex_t *s, int idx);

_ccddbl_inline void ccddblSimplexAdd(ccddbl_simplex_t *s, const ccddbl_support_t *v);
_ccddbl_inline void ccddblSimplexSet(ccddbl_simplex_t *s, size_t pos, const ccddbl_support_t *a);
_ccddbl_inline void ccddblSimplexSetSize(ccddbl_simplex_t *s, int size);
_ccddbl_inline void ccddblSimplexSwap(ccddbl_simplex_t *s, size_t pos1, size_t pos2);


/**** INLINES ****/

_ccddbl_inline void ccddblSimplexInit(ccddbl_simplex_t *s)
{
    s->last = -1;
}

_ccddbl_inline int ccddblSimplexSize(const ccddbl_simplex_t *s)
{
    return s->last + 1;
}

_ccddbl_inline const ccddbl_support_t *ccddblSimplexLast(const ccddbl_simplex_t *s)
{
    return ccddblSimplexPoint(s, s->last);
}

_ccddbl_inline const ccddbl_support_t *ccddblSimplexPoint(const ccddbl_simplex_t *s, int idx)
{
    // here is no check on boundaries
    return &s->ps[idx];
}
_ccddbl_inline ccddbl_support_t *ccddblSimplexPointW(ccddbl_simplex_t *s, int idx)
{
    return &s->ps[idx];
}

_ccddbl_inline void ccddblSimplexAdd(ccddbl_simplex_t *s, const ccddbl_support_t *v)
{
    // here is no check on boundaries in sake of speed
    ++s->last;
    ccddblSupportCopy(s->ps + s->last, v);
}

_ccddbl_inline void ccddblSimplexSet(ccddbl_simplex_t *s, size_t pos, const ccddbl_support_t *a)
{
    ccddblSupportCopy(s->ps + pos, a);
}

_ccddbl_inline void ccddblSimplexSetSize(ccddbl_simplex_t *s, int size)
{
    s->last = size - 1;
}

_ccddbl_inline void ccddblSimplexSwap(ccddbl_simplex_t *s, size_t pos1, size_t pos2)
{
    ccddbl_support_t supp;

    ccddblSupportCopy(&supp, &s->ps[pos1]);
    ccddblSupportCopy(&s->ps[pos1], &s->ps[pos2]);
    ccddblSupportCopy(&s->ps[pos2], &supp);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_SIMPLEX_H__ */
