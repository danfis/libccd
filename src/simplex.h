/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  libgjk is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  libgjk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GJK_SIMPLEX_H__
#define __GJK_SIMPLEX_H__

#include <gjk/vec3.h>
#include <gjk/compiler.h>

struct _gjk_simplex_t {
    gjk_vec3_t ps[4];
    int last; //!< index of last added point
};
typedef struct _gjk_simplex_t gjk_simplex_t;


_gjk_inline void gjkSimplexInit(gjk_simplex_t *s);
_gjk_inline int gjkSimplexSize(const gjk_simplex_t *s);
_gjk_inline const gjk_vec3_t *gjkSimplexLast(const gjk_simplex_t *s);
_gjk_inline const gjk_vec3_t *gjkSimplexPoint(const gjk_simplex_t *s, int idx);

_gjk_inline void gjkSimplexAdd(gjk_simplex_t *s, const gjk_vec3_t *v);
_gjk_inline void gjkSimplexSet1(gjk_simplex_t *s, const gjk_vec3_t *last);
_gjk_inline void gjkSimplexSet2(gjk_simplex_t *s,
                                const gjk_vec3_t *a, const gjk_vec3_t *last);
_gjk_inline void gjkSimplexSet3(gjk_simplex_t *s, const gjk_vec3_t *a,
                                const gjk_vec3_t *b, const gjk_vec3_t *last);


/**** INLINES ****/

_gjk_inline void gjkSimplexInit(gjk_simplex_t *s)
{
    s->last = -1;
}

_gjk_inline int gjkSimplexSize(const gjk_simplex_t *s)
{
    return s->last + 1;
}

_gjk_inline const gjk_vec3_t *gjkSimplexLast(const gjk_simplex_t *s)
{
    return gjkSimplexPoint(s, s->last);
}

_gjk_inline const gjk_vec3_t *gjkSimplexPoint(const gjk_simplex_t *s, int idx)
{
    // here is no check on boundaries
    return &s->ps[idx];
}

_gjk_inline void gjkSimplexAdd(gjk_simplex_t *s, const gjk_vec3_t *v)
{
    // here is no check on boundaries in sake of speed
    ++s->last;
    gjkVec3Copy(s->ps + s->last, v);
}

_gjk_inline void gjkSimplexSet1(gjk_simplex_t *s, const gjk_vec3_t *last)
{
    s->last = 0;
    gjkVec3Copy(s->ps, last);
}

_gjk_inline void gjkSimplexSet2(gjk_simplex_t *s,
                                const gjk_vec3_t *a, const gjk_vec3_t *last)
{
    s->last = 1;
    gjkVec3Copy(s->ps, a);
    gjkVec3Copy(s->ps + 1, last);
}

_gjk_inline void gjkSimplexSet3(gjk_simplex_t *s,
                                const gjk_vec3_t *a, const gjk_vec3_t *b,
                                const gjk_vec3_t *last)
{
    s->last = 2;
    gjkVec3Copy(s->ps, a);
    gjkVec3Copy(s->ps + 1, b);
    gjkVec3Copy(s->ps + 2, last);
}

#endif /* __GJK_SIMPLEX_H__ */
