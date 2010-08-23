#ifndef __GJK_SIMPLEX_H__
#define __GJK_SIMPLEX_H__

#include "vec3.h"
#include "compiler.h"

/**
 * TODO:
 */
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
    return s->last;
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
