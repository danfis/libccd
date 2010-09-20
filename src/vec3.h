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

#ifndef __GJK_VEC3_H__
#define __GJK_VEC3_H__

#include <math.h>
#include <float.h>
#include <gjk/compiler.h>
#include <gjk/config.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef GJK_SINGLE
# ifndef GJK_DOUBLE
#  error You must define GJK_SINGLE or GJK_DOUBLE
# endif /* GJK_DOUBLE */
#endif /* GJK_SINGLE */


#ifdef GJK_SINGLE
# ifdef GJK_DOUBLE
#  error You can define either GJK_SINGLE or GJK_DOUBLE, not both!
# endif /* GJK_DOUBLE */

typedef float gjk_real_t;

//# define GJK_EPS 1E-6
# define GJK_EPS FLT_EPSILON

# define GJK_REAL_MAX FLT_MAX

# define GJK_REAL(x) (x ## f)   /*!< form a constant */
# define GJK_SQRT(x) (sqrtf(x)) /*!< square root */
# define GJK_FABS(x) (fabsf(x)) /*!< absolute value */
# define GJK_FMAX(x, y) (fmaxf((x), (y))) /*!< maximum of two floats */
#endif /* GJK_SINGLE */

#ifdef GJK_DOUBLE
typedef double gjk_real_t;

//# define GJK_EPS 1E-10
# define GJK_EPS DBL_EPSILON

# define GJK_REAL_MAX DBL_MAX

# define GJK_REAL(x) (x)       /*!< form a constant */
# define GJK_SQRT(x) (sqrt(x)) /*!< square root */
# define GJK_FABS(x) (fabs(x)) /*!< absolute value */
# define GJK_FMAX(x, y) (fmax((x), (y))) /*!< maximum of two floats */
#endif /* GJK_DOUBLE */

#define GJK_ONE GJK_REAL(1.)
#define GJK_ZERO GJK_REAL(0.)

struct _gjk_vec3_t {
    gjk_real_t v[3];
};
typedef struct _gjk_vec3_t gjk_vec3_t;


/**
 * Holds origin (0,0,0) - this variable is meant to be read-only!
 */
extern gjk_vec3_t *gjk_vec3_origin;

/** Returns sign of value. */
_gjk_inline int gjkSign(gjk_real_t val);
/** Returns true if val is zero. **/
_gjk_inline int gjkIsZero(gjk_real_t val);
/** Returns true if a and b equal. **/
_gjk_inline int gjkEq(gjk_real_t a, gjk_real_t b);


#define GJK_VEC3_STATIC(x, y, z) \
    { { (x), (y), (z) } }

#define GJK_VEC3(name, x, y, z) \
    gjk_vec3_t name = GJK_VEC3_STATIC((x), (y), (z))

_gjk_inline gjk_real_t gjkVec3X(const gjk_vec3_t *v);
_gjk_inline gjk_real_t gjkVec3Y(const gjk_vec3_t *v);
_gjk_inline gjk_real_t gjkVec3Z(const gjk_vec3_t *v);

/**
 * Returns true if a and b equal.
 */
_gjk_inline int gjkVec3Eq(const gjk_vec3_t *a, const gjk_vec3_t *b);

/**
 * Returns squared length of vector.
 */
_gjk_inline gjk_real_t gjkVec3Len2(const gjk_vec3_t *v);

/**
 * Returns distance between a and b.
 */
_gjk_inline gjk_real_t gjkVec3Dist2(const gjk_vec3_t *a, const gjk_vec3_t *b);


_gjk_inline void gjkVec3Set(gjk_vec3_t *v, gjk_real_t x, gjk_real_t y, gjk_real_t z);

/**
 * v = w
 */
_gjk_inline void gjkVec3Copy(gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * Substracts coordinates of vector w from vector v. v = v - w
 */
_gjk_inline void gjkVec3Sub(gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * Adds coordinates of vector w to vector v. v = v + w
 */
_gjk_inline void gjkVec3Add(gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * d = v - w
 */
_gjk_inline void gjkVec3Sub2(gjk_vec3_t *d, const gjk_vec3_t *v, const gjk_vec3_t *w);

/**
 * d = d * k;
 */
_gjk_inline void gjkVec3Scale(gjk_vec3_t *d, gjk_real_t k);

/**
 * Normalizes given vector to unit length.
 */
_gjk_inline void gjkVec3Normalize(gjk_vec3_t *d);


/**
 * Dot product of two vectors.
 */
_gjk_inline gjk_real_t gjkVec3Dot(const gjk_vec3_t *a, const gjk_vec3_t *b);

/**
 * Cross product: d = a x b.
 */
_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b);


/**
 * Returns distance^2 of point P to segment ab.
 * If witness is non-NULL it is filled with coordinates of point from which
 * was computaed distance to point P.
 */
gjk_real_t gjkVec3PointSegmentDist2(const gjk_vec3_t *P,
                                const gjk_vec3_t *a, const gjk_vec3_t *b,
                                gjk_vec3_t *witness);

/**
 * Returns distance^2 of point P from triangle formed by triplet a, b, c.
 * If witness vector is provided it is filled with coordinates of point
 * from which was computed distance to point P.
 */
gjk_real_t gjkVec3PointTriDist2(const gjk_vec3_t *P,
                            const gjk_vec3_t *a, const gjk_vec3_t *b,
                            const gjk_vec3_t *c,
                            gjk_vec3_t *witness);


/**** INLINES ****/
_gjk_inline int gjkSign(gjk_real_t val)
{
    if (gjkIsZero(val)){
        return 0;
    }else if (val < GJK_ZERO){
        return -1;
    }
    return 1;
}

_gjk_inline int gjkIsZero(gjk_real_t val)
{
    return GJK_FABS(val) < GJK_EPS;
}

_gjk_inline int gjkEq(gjk_real_t _a, gjk_real_t _b)
{
    gjk_real_t ab;

    ab = GJK_FABS(_a - _b);
    if (GJK_FABS(ab) < GJK_EPS)
        return 1;

    gjk_real_t a, b;
    a = GJK_FABS(_a);
    b = GJK_FABS(_b);
    if (b > a){
        return ab < GJK_EPS * b;
    }else{
        return ab < GJK_EPS * a;
    }
}


_gjk_inline gjk_real_t gjkVec3X(const gjk_vec3_t *v)
{
    return v->v[0];
}

_gjk_inline gjk_real_t gjkVec3Y(const gjk_vec3_t *v)
{
    return v->v[1];
}

_gjk_inline gjk_real_t gjkVec3Z(const gjk_vec3_t *v)
{
    return v->v[2];
}

_gjk_inline int gjkVec3Eq(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    return gjkEq(gjkVec3X(a), gjkVec3X(b))
            && gjkEq(gjkVec3Y(a), gjkVec3Y(b))
            && gjkEq(gjkVec3Z(a), gjkVec3Z(b));
}

_gjk_inline gjk_real_t gjkVec3Len2(const gjk_vec3_t *v)
{
    return gjkVec3Dot(v, v);
}

_gjk_inline gjk_real_t gjkVec3Dist2(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    gjk_vec3_t ab;
    gjkVec3Sub2(&ab, a, b);
    return gjkVec3Len2(&ab);
}

_gjk_inline void gjkVec3Set(gjk_vec3_t *v, gjk_real_t x, gjk_real_t y, gjk_real_t z)
{
    v->v[0] = x;
    v->v[1] = y;
    v->v[2] = z;
}

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

_gjk_inline void gjkVec3Add(gjk_vec3_t *v, const gjk_vec3_t *w)
{
    v->v[0] += w->v[0];
    v->v[1] += w->v[1];
    v->v[2] += w->v[2];
}

_gjk_inline void gjkVec3Scale(gjk_vec3_t *d, gjk_real_t k)
{
    d->v[0] *= k;
    d->v[1] *= k;
    d->v[2] *= k;
}

_gjk_inline void gjkVec3Normalize(gjk_vec3_t *d)
{
    gjk_real_t k = GJK_ONE / GJK_SQRT(gjkVec3Len2(d));
    gjkVec3Scale(d, k);
}

_gjk_inline gjk_real_t gjkVec3Dot(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    gjk_real_t dot;

    dot  = a->v[0] * b->v[0];
    dot += a->v[1] * b->v[1];
    dot += a->v[2] * b->v[2];
    return dot;
}

_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    d->v[0] = (a->v[1] * b->v[2]) - (a->v[2] * b->v[1]);
    d->v[1] = (a->v[2] * b->v[0]) - (a->v[0] * b->v[2]);
    d->v[2] = (a->v[0] * b->v[1]) - (a->v[1] * b->v[0]);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_VEC3_H__ */
