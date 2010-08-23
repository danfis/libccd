#ifndef __GJK_VEC3_H__
#define __GJK_VEC3_H__

#include <math.h>
#include "gjk.h"
#include "compiler.h"


/**
 * v = w
 */
_gjk_inline void gjkVec3Copy(gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * Substracts coordinates of vector w from vector v.
 */
_gjk_inline void gjkVec3Sub(gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * d = v - w
 */
_gjk_inline void gjkVec3Sub2(gjk_vec3_t *d, const gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * d = d * k;
 */
_gjk_inline void gjkVec3Scale(gjk_vec3_t *d, double k);


/**
 * Dot product of two vectors.
 */
_gjk_inline double gjkVec3Dot(const gjk_vec3_t *a, const gjk_vec3_t *b);

/**
 * Cross product: d = a x b.
 */
_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b);



/**** INLINES ****/
_gjk_inline void gjkVec3Copy(gjk_vec3_t *v, const gjk_vec3_t *w)
{
    *v = *w;
}

_gjk_inline void gjkVec3Sub(gjk_vec3_t *v, const gjk_vec3_t *w)
{
    v->v[0] -= w->v[0];
    v->v[1] -= w->v[1];
    v->v[2] -= w->v[2];
}
_gjk_inline void gjkVec3Sub2(gjk_vec3_t *d, const gjk_vec3_t *v, const gjk_vec3_t *w)
{
    d->v[0] = v->v[0] - w->v[0];
    d->v[1] = v->v[1] - w->v[1];
    d->v[2] = v->v[2] - w->v[2];
}

_gjk_inline void gjkVec3Scale(gjk_vec3_t *d, double k)
{
    d->v[0] *= k;
    d->v[1] *= k;
    d->v[2] *= k;
}

_gjk_inline double gjkVec3Dot(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    double dot;

    dot  = a->v[0] * b->v[0];
    dot += a->v[1] * b->v[1];
    dot += a->v[2] * b->v[2];
    return sqrt(dot);
}

_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    d->v[0] = (a->v[1] * b->v[2]) - (a->v[2] * b->v[1]);
    d->v[1] = (a->v[2] * b->v[0]) - (a->v[0] * b->v[2]);
    d->v[2] = (a->v[0] * b->v[1]) - (a->v[1] * b->v[0]);
}

#endif /* __GJK_VEC3_H__ */
