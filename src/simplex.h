/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#ifndef __GJK_SIMPLEX_H__
#define __GJK_SIMPLEX_H__

#include <gjk/support.h>
#include <gjk/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _gjk_simplex_t {
    gjk_support_t ps[4];
    int last; //!< index of last added point
};
typedef struct _gjk_simplex_t gjk_simplex_t;


_gjk_inline void gjkSimplexInit(gjk_simplex_t *s);
_gjk_inline int gjkSimplexSize(const gjk_simplex_t *s);
_gjk_inline const gjk_support_t *gjkSimplexLast(const gjk_simplex_t *s);
_gjk_inline const gjk_support_t *gjkSimplexPoint(const gjk_simplex_t *s, int idx);
_gjk_inline gjk_support_t *gjkSimplexPointW(gjk_simplex_t *s, int idx);

_gjk_inline void gjkSimplexAdd(gjk_simplex_t *s, const gjk_support_t *v);
_gjk_inline void gjkSimplexSet(gjk_simplex_t *s, size_t pos, const gjk_support_t *a);
_gjk_inline void gjkSimplexSetSize(gjk_simplex_t *s, int size);
_gjk_inline void gjkSimplexSwap(gjk_simplex_t *s, size_t pos1, size_t pos2);


/**** INLINES ****/

_gjk_inline void gjkSimplexInit(gjk_simplex_t *s)
{
    s->last = -1;
}

_gjk_inline int gjkSimplexSize(const gjk_simplex_t *s)
{
    return s->last + 1;
}

_gjk_inline const gjk_support_t *gjkSimplexLast(const gjk_simplex_t *s)
{
    return gjkSimplexPoint(s, s->last);
}

_gjk_inline const gjk_support_t *gjkSimplexPoint(const gjk_simplex_t *s, int idx)
{
    // here is no check on boundaries
    return &s->ps[idx];
}
_gjk_inline gjk_support_t *gjkSimplexPointW(gjk_simplex_t *s, int idx)
{
    return &s->ps[idx];
}

_gjk_inline void gjkSimplexAdd(gjk_simplex_t *s, const gjk_support_t *v)
{
    // here is no check on boundaries in sake of speed
    ++s->last;
    gjkSupportCopy(s->ps + s->last, v);
}

_gjk_inline void gjkSimplexSet(gjk_simplex_t *s, size_t pos, const gjk_support_t *a)
{
    gjkSupportCopy(s->ps + pos, a);
}

_gjk_inline void gjkSimplexSetSize(gjk_simplex_t *s, int size)
{
    s->last = size - 1;
}

_gjk_inline void gjkSimplexSwap(gjk_simplex_t *s, size_t pos1, size_t pos2)
{
    gjk_support_t supp;

    gjkSupportCopy(&supp, &s->ps[pos1]);
    gjkSupportCopy(&s->ps[pos1], &s->ps[pos2]);
    gjkSupportCopy(&s->ps[pos2], &supp);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_SIMPLEX_H__ */
