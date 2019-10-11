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

#ifndef __CCDDBL_QUAT_H__
#define __CCDDBL_QUAT_H__

#include <ccddbl/compiler.h>
#include <ccddbl/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _ccddbl_quat_t {
    ccddbl_real_t q[4]; //!< x, y, z, w
};
typedef struct _ccddbl_quat_t ccddbl_quat_t;

#define CCDDBL_QUAT(name, x, y, z, w) \
    ccddbl_quat_t name = { {x, y, z, w} }

_ccddbl_inline ccddbl_real_t ccddblQuatLen2(const ccddbl_quat_t *q);
_ccddbl_inline ccddbl_real_t ccddblQuatLen(const ccddbl_quat_t *q);

_ccddbl_inline void ccddblQuatSet(ccddbl_quat_t *q, ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z, ccddbl_real_t w);
_ccddbl_inline void ccddblQuatCopy(ccddbl_quat_t *dest, const ccddbl_quat_t *src);

_ccddbl_inline int ccddblQuatNormalize(ccddbl_quat_t *q);

_ccddbl_inline void ccddblQuatSetAngleAxis(ccddbl_quat_t *q,
                                     ccddbl_real_t angle, const ccddbl_vec3_t *axis);

_ccddbl_inline void ccddblQuatScale(ccddbl_quat_t *q, ccddbl_real_t k);

/**
 * q = q * q2
 */
_ccddbl_inline void ccddblQuatMul(ccddbl_quat_t *q, const ccddbl_quat_t *q2);

/**
 * q = a * b
 */
_ccddbl_inline void ccddblQuatMul2(ccddbl_quat_t *q,
                             const ccddbl_quat_t *a, const ccddbl_quat_t *b);

/**
 * Inverts quaternion.
 * Returns 0 on success.
 */
_ccddbl_inline int ccddblQuatInvert(ccddbl_quat_t *q);
_ccddbl_inline int ccddblQuatInvert2(ccddbl_quat_t *dest, const ccddbl_quat_t *src);


/**
 * Rotate vector v by quaternion q.
 */
_ccddbl_inline void ccddblQuatRotVec(ccddbl_vec3_t *v, const ccddbl_quat_t *q);


/**** INLINES ****/
_ccddbl_inline ccddbl_real_t ccddblQuatLen2(const ccddbl_quat_t *q)
{
    ccddbl_real_t len;

    len  = q->q[0] * q->q[0];
    len += q->q[1] * q->q[1];
    len += q->q[2] * q->q[2];
    len += q->q[3] * q->q[3];

    return len;
}

_ccddbl_inline ccddbl_real_t ccddblQuatLen(const ccddbl_quat_t *q)
{
    return CCDDBL_SQRT(ccddblQuatLen2(q));
}

_ccddbl_inline void ccddblQuatSet(ccddbl_quat_t *q, ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z, ccddbl_real_t w)
{
    q->q[0] = x;
    q->q[1] = y;
    q->q[2] = z;
    q->q[3] = w;
}

_ccddbl_inline void ccddblQuatCopy(ccddbl_quat_t *dest, const ccddbl_quat_t *src)
{
    *dest = *src;
}


_ccddbl_inline int ccddblQuatNormalize(ccddbl_quat_t *q)
{
    ccddbl_real_t len = ccddblQuatLen(q);
    if (len < CCDDBL_EPS)
        return 0;

    ccddblQuatScale(q, CCDDBL_ONE / len);
    return 1;
}

_ccddbl_inline void ccddblQuatSetAngleAxis(ccddbl_quat_t *q,
                                     ccddbl_real_t angle, const ccddbl_vec3_t *axis)
{
    ccddbl_real_t a, x, y, z, n, s;

    a = angle/2;
    x = ccddblVec3X(axis);
    y = ccddblVec3Y(axis);
    z = ccddblVec3Z(axis);
    n = CCDDBL_SQRT(x*x + y*y + z*z);

    // axis==0? (treat this the same as angle==0 with an arbitrary axis)
    if (n < CCDDBL_EPS){
        q->q[0] = q->q[1] = q->q[2] = CCDDBL_ZERO;
        q->q[3] = CCDDBL_ONE;
    }else{
        s = sin(a)/n;

        q->q[3] = cos(a);
        q->q[0] = x*s;
        q->q[1] = y*s;
        q->q[2] = z*s;

        ccddblQuatNormalize(q);
    }
}


_ccddbl_inline void ccddblQuatScale(ccddbl_quat_t *q, ccddbl_real_t k)
{
    size_t i;
    for (i = 0; i < 4; i++)
        q->q[i] *= k;
}

_ccddbl_inline void ccddblQuatMul(ccddbl_quat_t *q, const ccddbl_quat_t *q2)
{
    ccddbl_quat_t a;
    ccddblQuatCopy(&a, q);
    ccddblQuatMul2(q, &a, q2);
}

_ccddbl_inline void ccddblQuatMul2(ccddbl_quat_t *q,
                             const ccddbl_quat_t *a, const ccddbl_quat_t *b)
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

_ccddbl_inline int ccddblQuatInvert(ccddbl_quat_t *q)
{
    ccddbl_real_t len2 = ccddblQuatLen2(q);
    if (len2 < CCDDBL_EPS)
        return -1;

    len2 = CCDDBL_ONE / len2;

    q->q[0] = -q->q[0] * len2;
    q->q[1] = -q->q[1] * len2;
    q->q[2] = -q->q[2] * len2;
    q->q[3] = q->q[3] * len2;

    return 0;
}
_ccddbl_inline int ccddblQuatInvert2(ccddbl_quat_t *dest, const ccddbl_quat_t *src)
{
    ccddblQuatCopy(dest, src);
    return ccddblQuatInvert(dest);
}

_ccddbl_inline void ccddblQuatRotVec(ccddbl_vec3_t *v, const ccddbl_quat_t *q)
{
    // original version: 31 mul + 21 add
    // optimized version: 18 mul + 12 add
    // formula: v = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v)
    ccddbl_real_t cross1_x, cross1_y, cross1_z, cross2_x, cross2_y, cross2_z;
    ccddbl_real_t x, y, z, w;
    ccddbl_real_t vx, vy, vz;

    vx = ccddblVec3X(v);
    vy = ccddblVec3Y(v);
    vz = ccddblVec3Z(v);

    w = q->q[3];
    x = q->q[0];
    y = q->q[1];
    z = q->q[2];

    cross1_x = y * vz - z * vy + w * vx;
    cross1_y = z * vx - x * vz + w * vy;
    cross1_z = x * vy - y * vx + w * vz;
    cross2_x = y * cross1_z - z * cross1_y;
    cross2_y = z * cross1_x - x * cross1_z;
    cross2_z = x * cross1_y - y * cross1_x;
    ccddblVec3Set(v, vx + 2 * cross2_x, vy + 2 * cross2_y, vz + 2 * cross2_z);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_QUAT_H__ */
