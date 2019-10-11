/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010-2013 Daniel Fiser <danfis@danfis.cz>
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

#ifndef __CCDDBL_VEC3_H__
#define __CCDDBL_VEC3_H__

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <ccddbl/compiler.h>
#include <ccddbl/config.h>
#include <ccddbl/ccddbl_export.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifndef CCDDBL_SINGLE
# ifndef CCDDBL_DOUBLE
#  error You must define CCDDBL_SINGLE or CCDDBL_DOUBLE
# endif /* CCDDBL_DOUBLE */
#endif /* CCDDBL_SINGLE */

#ifdef WIN32
# define CCDDBL_FMIN(x, y) ((x) < (y) ? (x) : (y))
#endif /* WIN32 */

#ifdef CCDDBL_SINGLE
# ifdef CCDDBL_DOUBLE
#  error You can define either CCDDBL_SINGLE or CCDDBL_DOUBLE, not both!
# endif /* CCDDBL_DOUBLE */

typedef float ccddbl_real_t;

//# define CCDDBL_EPS 1E-6
# define CCDDBL_EPS FLT_EPSILON

# define CCDDBL_REAL_MAX FLT_MAX

# define CCDDBL_REAL(x) (x ## f)   /*!< form a constant */
# define CCDDBL_SQRT(x) (sqrtf(x)) /*!< square root */
# define CCDDBL_FABS(x) (fabsf(x)) /*!< absolute value */
# define CCDDBL_FMAX(x, y) (fmaxf((x), (y))) /*!< maximum of two floats */

# ifndef CCDDBL_FMIN
#  define CCDDBL_FMIN(x, y) (fminf((x), (y))) /*!< minimum of two floats */
# endif /* CCDDBL_FMIN */

#endif /* CCDDBL_SINGLE */

#ifdef CCDDBL_DOUBLE
typedef double ccddbl_real_t;

//# define CCDDBL_EPS 1E-10
# define CCDDBL_EPS DBL_EPSILON

# define CCDDBL_REAL_MAX DBL_MAX

# define CCDDBL_REAL(x) (x)       /*!< form a constant */
# define CCDDBL_SQRT(x) (sqrt(x)) /*!< square root */
# define CCDDBL_FABS(x) (fabs(x)) /*!< absolute value */
# define CCDDBL_FMAX(x, y) (fmax((x), (y))) /*!< maximum of two floats */

# ifndef CCDDBL_FMIN
#  define CCDDBL_FMIN(x, y) (fmin((x), (y))) /*!< minimum of two floats */
# endif /* CCDDBL_FMIN */

#endif /* CCDDBL_DOUBLE */

#define CCDDBL_ONE CCDDBL_REAL(1.)
#define CCDDBL_ZERO CCDDBL_REAL(0.)

struct _ccddbl_vec3_t {
    ccddbl_real_t v[3];
};
typedef struct _ccddbl_vec3_t ccddbl_vec3_t;


/**
 * Holds origin (0,0,0) - this variable is meant to be read-only!
 */
CCDDBL_EXPORT extern ccddbl_vec3_t *ccddbl_vec3_origin;

/**
 * Array of points uniformly distributed on unit sphere.
 */
CCDDBL_EXPORT extern ccddbl_vec3_t *ccddbl_points_on_sphere;
CCDDBL_EXPORT extern size_t ccddbl_points_on_sphere_len;

/** Returns sign of value. */
_ccddbl_inline int ccddblSign(ccddbl_real_t val);
/** Returns true if val is zero. **/
_ccddbl_inline int ccddblIsZero(ccddbl_real_t val);
/** Returns true if a and b equal. **/
_ccddbl_inline int ccddblEq(ccddbl_real_t a, ccddbl_real_t b);


#define CCDDBL_VEC3_STATIC(x, y, z) \
    { { (x), (y), (z) } }

#define CCDDBL_VEC3(name, x, y, z) \
    ccddbl_vec3_t name = CCDDBL_VEC3_STATIC((x), (y), (z))

_ccddbl_inline ccddbl_real_t ccddblVec3X(const ccddbl_vec3_t *v);
_ccddbl_inline ccddbl_real_t ccddblVec3Y(const ccddbl_vec3_t *v);
_ccddbl_inline ccddbl_real_t ccddblVec3Z(const ccddbl_vec3_t *v);

/**
 * Returns true if a and b equal.
 */
_ccddbl_inline int ccddblVec3Eq(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b);

/**
 * Returns squared length of vector.
 */
_ccddbl_inline ccddbl_real_t ccddblVec3Len2(const ccddbl_vec3_t *v);

/**
 * Returns distance between a and b.
 */
_ccddbl_inline ccddbl_real_t ccddblVec3Dist2(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b);


_ccddbl_inline void ccddblVec3Set(ccddbl_vec3_t *v, ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z);

/**
 * v = w
 */
_ccddbl_inline void ccddblVec3Copy(ccddbl_vec3_t *v, const ccddbl_vec3_t *w);

/**
 * Substracts coordinates of vector w from vector v. v = v - w
 */
_ccddbl_inline void ccddblVec3Sub(ccddbl_vec3_t *v, const ccddbl_vec3_t *w);

/**
 * Adds coordinates of vector w to vector v. v = v + w
 */
_ccddbl_inline void ccddblVec3Add(ccddbl_vec3_t *v, const ccddbl_vec3_t *w);

/**
 * d = v - w
 */
_ccddbl_inline void ccddblVec3Sub2(ccddbl_vec3_t *d, const ccddbl_vec3_t *v, const ccddbl_vec3_t *w);

/**
 * d = d * k;
 */
_ccddbl_inline void ccddblVec3Scale(ccddbl_vec3_t *d, ccddbl_real_t k);

/**
 * Normalizes given vector to unit length.
 */
_ccddbl_inline void ccddblVec3Normalize(ccddbl_vec3_t *d);


/**
 * Dot product of two vectors.
 */
_ccddbl_inline ccddbl_real_t ccddblVec3Dot(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b);

/**
 * Cross product: d = a x b.
 */
_ccddbl_inline void ccddblVec3Cross(ccddbl_vec3_t *d, const ccddbl_vec3_t *a, const ccddbl_vec3_t *b);


/**
 * Returns distance^2 of point P to segment ab.
 * If witness is non-NULL it is filled with coordinates of point from which
 * was computed distance to point P.
 */
CCDDBL_EXPORT ccddbl_real_t ccddblVec3PointSegmentDist2(const ccddbl_vec3_t *P,
                                                const ccddbl_vec3_t *a,
                                                const ccddbl_vec3_t *b,
                                                ccddbl_vec3_t *witness);

/**
 * Returns distance^2 of point P from triangle formed by triplet a, b, c.
 * If witness vector is provided it is filled with coordinates of point
 * from which was computed distance to point P.
 */
CCDDBL_EXPORT ccddbl_real_t ccddblVec3PointTriDist2(const ccddbl_vec3_t *P,
                                            const ccddbl_vec3_t *a,
                                            const ccddbl_vec3_t *b,
                                            const ccddbl_vec3_t *c,
                                            ccddbl_vec3_t *witness);


/**** INLINES ****/
_ccddbl_inline int ccddblSign(ccddbl_real_t val)
{
    if (ccddblIsZero(val)){
        return 0;
    }else if (val < CCDDBL_ZERO){
        return -1;
    }
    return 1;
}

_ccddbl_inline int ccddblIsZero(ccddbl_real_t val)
{
    return CCDDBL_FABS(val) < CCDDBL_EPS;
}

_ccddbl_inline int ccddblEq(ccddbl_real_t _a, ccddbl_real_t _b)
{
    ccddbl_real_t ab;
    ccddbl_real_t a, b;

    ab = CCDDBL_FABS(_a - _b);
    if (CCDDBL_FABS(ab) < CCDDBL_EPS)
        return 1;

    a = CCDDBL_FABS(_a);
    b = CCDDBL_FABS(_b);
    if (b > a){
        return ab < CCDDBL_EPS * b;
    }else{
        return ab < CCDDBL_EPS * a;
    }
}


_ccddbl_inline ccddbl_real_t ccddblVec3X(const ccddbl_vec3_t *v)
{
    return v->v[0];
}

_ccddbl_inline ccddbl_real_t ccddblVec3Y(const ccddbl_vec3_t *v)
{
    return v->v[1];
}

_ccddbl_inline ccddbl_real_t ccddblVec3Z(const ccddbl_vec3_t *v)
{
    return v->v[2];
}

_ccddbl_inline int ccddblVec3Eq(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b)
{
    return ccddblEq(ccddblVec3X(a), ccddblVec3X(b))
            && ccddblEq(ccddblVec3Y(a), ccddblVec3Y(b))
            && ccddblEq(ccddblVec3Z(a), ccddblVec3Z(b));
}

_ccddbl_inline ccddbl_real_t ccddblVec3Len2(const ccddbl_vec3_t *v)
{
    return ccddblVec3Dot(v, v);
}

_ccddbl_inline ccddbl_real_t ccddblVec3Dist2(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b)
{
    ccddbl_vec3_t ab;
    ccddblVec3Sub2(&ab, a, b);
    return ccddblVec3Len2(&ab);
}

_ccddbl_inline void ccddblVec3Set(ccddbl_vec3_t *v, ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z)
{
    v->v[0] = x;
    v->v[1] = y;
    v->v[2] = z;
}

_ccddbl_inline void ccddblVec3Copy(ccddbl_vec3_t *v, const ccddbl_vec3_t *w)
{
    *v = *w;
}

_ccddbl_inline void ccddblVec3Sub(ccddbl_vec3_t *v, const ccddbl_vec3_t *w)
{
    v->v[0] -= w->v[0];
    v->v[1] -= w->v[1];
    v->v[2] -= w->v[2];
}
_ccddbl_inline void ccddblVec3Sub2(ccddbl_vec3_t *d, const ccddbl_vec3_t *v, const ccddbl_vec3_t *w)
{
    d->v[0] = v->v[0] - w->v[0];
    d->v[1] = v->v[1] - w->v[1];
    d->v[2] = v->v[2] - w->v[2];
}

_ccddbl_inline void ccddblVec3Add(ccddbl_vec3_t *v, const ccddbl_vec3_t *w)
{
    v->v[0] += w->v[0];
    v->v[1] += w->v[1];
    v->v[2] += w->v[2];
}

_ccddbl_inline void ccddblVec3Scale(ccddbl_vec3_t *d, ccddbl_real_t k)
{
    d->v[0] *= k;
    d->v[1] *= k;
    d->v[2] *= k;
}

_ccddbl_inline void ccddblVec3Normalize(ccddbl_vec3_t *d)
{
    ccddbl_real_t k = CCDDBL_ONE / CCDDBL_SQRT(ccddblVec3Len2(d));
    ccddblVec3Scale(d, k);
}

_ccddbl_inline ccddbl_real_t ccddblVec3Dot(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b)
{
    ccddbl_real_t dot;

    dot  = a->v[0] * b->v[0];
    dot += a->v[1] * b->v[1];
    dot += a->v[2] * b->v[2];
    return dot;
}

_ccddbl_inline void ccddblVec3Cross(ccddbl_vec3_t *d, const ccddbl_vec3_t *a, const ccddbl_vec3_t *b)
{
    d->v[0] = (a->v[1] * b->v[2]) - (a->v[2] * b->v[1]);
    d->v[1] = (a->v[2] * b->v[0]) - (a->v[0] * b->v[2]);
    d->v[2] = (a->v[0] * b->v[1]) - (a->v[1] * b->v[0]);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_VEC3_H__ */
