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

#ifndef __GJK_QUAT_H__
#define __GJK_QUAT_H__

#include <gjk/compiler.h>
#include <gjk/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _gjk_quat_t {
    double q[4]; //!< x, y, z, w
};
typedef struct _gjk_quat_t gjk_quat_t;

#define GJK_QUAT(name, x, y, z, w) \
    gjk_quat_t name = { {x, y, z, w} }

_gjk_inline double gjkQuatLen2(const gjk_quat_t *q);
_gjk_inline double gjkQuatLen(const gjk_quat_t *q);

_gjk_inline void gjkQuatSet(gjk_quat_t *q, double x, double y, double z, double w);
_gjk_inline void gjkQuatCopy(gjk_quat_t *dest, const gjk_quat_t *src);

_gjk_inline int gjkQuatNormalize(gjk_quat_t *q);

_gjk_inline void gjkQuatSetAngleAxis(gjk_quat_t *q,
                                     double angle, const gjk_vec3_t *axis);

_gjk_inline void gjkQuatScale(gjk_quat_t *q, double k);

/**
 * q = q * q2
 */
_gjk_inline void gjkQuatMul(gjk_quat_t *q, const gjk_quat_t *q2);

/**
 * q = a * b
 */
_gjk_inline void gjkQuatMul2(gjk_quat_t *q,
                             const gjk_quat_t *a, const gjk_quat_t *b);

/**
 * Inverts quaternion.
 * Returns 0 on success.
 */
_gjk_inline int gjkQuatInvert(gjk_quat_t *q);
_gjk_inline int gjkQuatInvert2(gjk_quat_t *dest, const gjk_quat_t *src);


/**
 * Rotate vector v by quaternion q.
 */
_gjk_inline void gjkQuatRotVec(gjk_vec3_t *v, const gjk_quat_t *q);


/**** INLINES ****/
_gjk_inline double gjkQuatLen2(const gjk_quat_t *q)
{
    double len;

    len  = q->q[0] * q->q[0];
    len += q->q[1] * q->q[1];
    len += q->q[2] * q->q[2];
    len += q->q[3] * q->q[3];

    return len;
}

_gjk_inline double gjkQuatLen(const gjk_quat_t *q)
{
    return sqrt(gjkQuatLen2(q));
}

_gjk_inline void gjkQuatSet(gjk_quat_t *q, double x, double y, double z, double w)
{
    q->q[0] = x;
    q->q[1] = y;
    q->q[2] = z;
    q->q[3] = w;
}

_gjk_inline void gjkQuatCopy(gjk_quat_t *dest, const gjk_quat_t *src)
{
    *dest = *src;
}


_gjk_inline int gjkQuatNormalize(gjk_quat_t *q)
{
    double len = gjkQuatLen(q);
    if (len < GJK_EPS)
        return 0;

    gjkQuatScale(q, 1. / len);
    return 1;
}

_gjk_inline void gjkQuatSetAngleAxis(gjk_quat_t *q,
                                     double angle, const gjk_vec3_t *axis)
{
    double a, x, y, z, n, s;

    a = angle/2;
    x = gjkVec3X(axis);
    y = gjkVec3Y(axis);
    z = gjkVec3Z(axis);
    n = sqrt(x*x + y*y + z*z);

    // axis==0? (treat this the same as angle==0 with an arbitrary axis)
    if (n < GJK_EPS){
        q->q[0] = q->q[1] = q->q[2] = 0.;
        q->q[3] = 1.;
    }else{
        s = sin(a)/n;

        q->q[3] = cos(a);
        q->q[0] = x*s;
        q->q[1] = y*s;
        q->q[2] = z*s;

        gjkQuatNormalize(q);
    }
}


_gjk_inline void gjkQuatScale(gjk_quat_t *q, double k)
{
    size_t i;
    for (i = 0; i < 4; i++)
        q->q[i] *= k;
}

_gjk_inline void gjkQuatMul(gjk_quat_t *q, const gjk_quat_t *q2)
{
    gjk_quat_t a;
    gjkQuatCopy(&a, q);
    gjkQuatMul2(q, &a, q2);
}

_gjk_inline void gjkQuatMul2(gjk_quat_t *q,
                             const gjk_quat_t *a, const gjk_quat_t *b)
{
    q->q[0] = a->q[3] * b->q[0]
                + a->q[0] * b->q[3]
                + a->q[1] * b->q[2]
                - a->q[2] * b->q[1];
    q->q[1] = a->q[3] * b->q[1]
                + a->q[1] * b->q[3]
                - a->q[0] * b->q[2]
                + a->q[2] * b->q[0];
    q->q[2] = a->q[3] * b->q[2]
                + a->q[2] * b->q[3]
                + a->q[0] * b->q[1]
                - a->q[1] * b->q[0];
    q->q[3] = a->q[3] * b->q[3]
                - a->q[0] * b->q[0]
                - a->q[1] * b->q[1]
                - a->q[2] * b->q[2];
}

_gjk_inline int gjkQuatInvert(gjk_quat_t *q)
{
    double len2 = gjkQuatLen2(q);
    if (len2 < GJK_EPS)
        return -1;

    len2 = 1. / len2;

    q->q[0] = -q->q[0] * len2;
    q->q[1] = -q->q[1] * len2;
    q->q[2] = -q->q[2] * len2;
    q->q[3] = q->q[3] * len2;

    return 0;
}
_gjk_inline int gjkQuatInvert2(gjk_quat_t *dest, const gjk_quat_t *src)
{
    gjkQuatCopy(dest, src);
    return gjkQuatInvert(dest);
}

_gjk_inline void gjkQuatRotVec(gjk_vec3_t *v, const gjk_quat_t *q)
{
    double w, x, y, z, ww, xx, yy, zz, wx, wy, wz, xy, xz, yz;
    double vx, vy, vz;

    w = q->q[3];
    x = q->q[0];
    y = q->q[1];
    z = q->q[2];
    ww = w*w;
    xx = x*x;
    yy = y*y;
    zz = z*z;
    wx = w*x;
    wy = w*y;
    wz = w*z;
    xy = x*y;
    xz = x*z;
    yz = y*z;

    vx = ww * gjkVec3X(v)
            + xx * gjkVec3X(v)
            - yy * gjkVec3X(v)
            - zz * gjkVec3X(v)
            + 2 * ((xy - wz) * gjkVec3Y(v)
            + (xz + wy) * gjkVec3Z(v));
    vy = ww * gjkVec3Y(v)
            - xx * gjkVec3Y(v)
            + yy * gjkVec3Y(v)
            - zz * gjkVec3Y(v)
            + 2 * ((xy + wz) * gjkVec3X(v)
            + (yz - wx) * gjkVec3Z(v));
    vz = ww * gjkVec3Z(v)
            - xx * gjkVec3Z(v)
            - yy * gjkVec3Z(v)
            + zz * gjkVec3Z(v)
            + 2 * ((xz - wy) * gjkVec3X(v)
            + (yz + wx) * gjkVec3Y(v));
    gjkVec3Set(v, vx, vy, vz);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_QUAT_H__ */
