#ifndef __GJK_VEC3_H__
#define __GJK_VEC3_H__

#include <math.h>
#include <gjk/compiler.h>


#define GJK_EPS 1E-10

struct _gjk_vec3_t {
    double v[3];
};
typedef struct _gjk_vec3_t gjk_vec3_t;


/**
 * Holds origin (0,0,0) - this variable is meant to be read-only!
 */
extern gjk_vec3_t *gjk_vec3_origin;

#define GJK_PRINT_VEC3(vec, prefix) \
    fprintf(stderr, prefix "[%lf %lf %lf]\n", \
            gjkVec3X(vec), gjkVec3Y(vec), gjkVec3Z(vec))


// TODO: add gjk prefix
/** Returns sign of value. */
_gjk_inline int sign(double val);
/** Returns true if val is zero. **/
_gjk_inline int isZero(double val);
/** Returns true if a and b equal. **/
_gjk_inline int gjkEq(double a, double b);


#define GJK_VEC3_STATIC(x, y, z) \
    { { (x), (y), (z) } }

#define GJK_VEC3(name, x, y, z) \
    gjk_vec3_t name = GJK_VEC3_STATIC((x), (y), (z))

_gjk_inline double gjkVec3X(const gjk_vec3_t *v);
_gjk_inline double gjkVec3Y(const gjk_vec3_t *v);
_gjk_inline double gjkVec3Z(const gjk_vec3_t *v);

/**
 * Returns true if a and b equal.
 */
_gjk_inline int gjkVec3Eq(const gjk_vec3_t *a, const gjk_vec3_t *b);

/**
 * Returns squared length of vector.
 */
_gjk_inline double gjkVec3Len2(const gjk_vec3_t *v);

/**
 * Returns distance between a and b.
 */
_gjk_inline double gjkVec3Dist2(const gjk_vec3_t *a, const gjk_vec3_t *b);


_gjk_inline void gjkVec3Set(gjk_vec3_t *v, double x, double y, double z);

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
_gjk_inline void gjkVec3Scale(gjk_vec3_t *d, double k);


/**
 * Dot product of two vectors.
 */
_gjk_inline double gjkVec3Dot(const gjk_vec3_t *a, const gjk_vec3_t *b);

/**
 * Cross product: d = a x b.
 */
_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b);


/**
 * Returns distance^2 of point P from triangle formed by triplet a, b, c.
 * If witness vector is provided it is filled with coordinates of point
 * from which was computed distance to point P.
 */
double gjkVec3PointTriDist2(const gjk_vec3_t *P,
                            const gjk_vec3_t *a, const gjk_vec3_t *b,
                            const gjk_vec3_t *c,
                            gjk_vec3_t *witness);


/**** INLINES ****/
_gjk_inline int sign(double val)
{
    if (isZero(val)){
        return 0;
    }else if (val < 0.){
        return -1;
    }
    return 1;
}

_gjk_inline int isZero(double val)
{
    return fabs(val) < GJK_EPS;
}

_gjk_inline int gjkEq(double a, double b)
{
    if (fabs(a - b) < GJK_EPS)
        return 1;
    return 0;
}


_gjk_inline double gjkVec3X(const gjk_vec3_t *v)
{
    return v->v[0];
}

_gjk_inline double gjkVec3Y(const gjk_vec3_t *v)
{
    return v->v[1];
}

_gjk_inline double gjkVec3Z(const gjk_vec3_t *v)
{
    return v->v[2];
}

_gjk_inline int gjkVec3Eq(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    return gjkEq(gjkVec3X(a), gjkVec3X(b))
            && gjkEq(gjkVec3Y(a), gjkVec3Y(b))
            && gjkEq(gjkVec3Z(a), gjkVec3Z(b));
}

_gjk_inline double gjkVec3Len2(const gjk_vec3_t *v)
{
    return gjkVec3Dot(v, v);
}

_gjk_inline double gjkVec3Dist2(const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    gjk_vec3_t ab;
    gjkVec3Sub2(&ab, a, b);
    return gjkVec3Len2(&ab);
}

_gjk_inline void gjkVec3Set(gjk_vec3_t *v, double x, double y, double z)
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
    return dot;
}

_gjk_inline void gjkVec3Cross(gjk_vec3_t *d, const gjk_vec3_t *a, const gjk_vec3_t *b)
{
    d->v[0] = (a->v[1] * b->v[2]) - (a->v[2] * b->v[1]);
    d->v[1] = (a->v[2] * b->v[0]) - (a->v[0] * b->v[2]);
    d->v[2] = (a->v[0] * b->v[1]) - (a->v[1] * b->v[0]);
}

#endif /* __GJK_VEC3_H__ */
