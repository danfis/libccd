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

#include <stdio.h>
#include <ccddbl/vec3.h>
#include "dbg.h"

static CCDDBL_VEC3(__ccddbl_vec3_origin, CCDDBL_ZERO, CCDDBL_ZERO, CCDDBL_ZERO);
ccddbl_vec3_t *ccddbl_vec3_origin = &__ccddbl_vec3_origin;

static ccddbl_vec3_t points_on_sphere[] = {
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.000000), CCDDBL_REAL(-0.000000), CCDDBL_REAL(-1.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.723608), CCDDBL_REAL(-0.525725), CCDDBL_REAL(-0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.276388), CCDDBL_REAL(-0.850649), CCDDBL_REAL(-0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.894426), CCDDBL_REAL(-0.000000), CCDDBL_REAL(-0.447216)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.276388), CCDDBL_REAL( 0.850649), CCDDBL_REAL(-0.447220)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.723608), CCDDBL_REAL( 0.525725), CCDDBL_REAL(-0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.276388), CCDDBL_REAL(-0.850649), CCDDBL_REAL( 0.447220)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.723608), CCDDBL_REAL(-0.525725), CCDDBL_REAL( 0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.723608), CCDDBL_REAL( 0.525725), CCDDBL_REAL( 0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.276388), CCDDBL_REAL( 0.850649), CCDDBL_REAL( 0.447219)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.894426), CCDDBL_REAL( 0.000000), CCDDBL_REAL( 0.447216)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.000000), CCDDBL_REAL( 0.000000), CCDDBL_REAL( 1.000000)), 
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.425323), CCDDBL_REAL(-0.309011), CCDDBL_REAL(-0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.162456), CCDDBL_REAL(-0.499995), CCDDBL_REAL(-0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.262869), CCDDBL_REAL(-0.809012), CCDDBL_REAL(-0.525738)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.425323), CCDDBL_REAL( 0.309011), CCDDBL_REAL(-0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.850648), CCDDBL_REAL(-0.000000), CCDDBL_REAL(-0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.525730), CCDDBL_REAL(-0.000000), CCDDBL_REAL(-0.850652)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.688190), CCDDBL_REAL(-0.499997), CCDDBL_REAL(-0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.162456), CCDDBL_REAL( 0.499995), CCDDBL_REAL(-0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.688190), CCDDBL_REAL( 0.499997), CCDDBL_REAL(-0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.262869), CCDDBL_REAL( 0.809012), CCDDBL_REAL(-0.525738)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.951058), CCDDBL_REAL( 0.309013), CCDDBL_REAL( 0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.951058), CCDDBL_REAL(-0.309013), CCDDBL_REAL( 0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.587786), CCDDBL_REAL(-0.809017), CCDDBL_REAL( 0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.000000), CCDDBL_REAL(-1.000000), CCDDBL_REAL( 0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.587786), CCDDBL_REAL(-0.809017), CCDDBL_REAL( 0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.951058), CCDDBL_REAL(-0.309013), CCDDBL_REAL(-0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.951058), CCDDBL_REAL( 0.309013), CCDDBL_REAL(-0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.587786), CCDDBL_REAL( 0.809017), CCDDBL_REAL(-0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.000000), CCDDBL_REAL( 1.000000), CCDDBL_REAL(-0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.587786), CCDDBL_REAL( 0.809017), CCDDBL_REAL(-0.000000)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.688190), CCDDBL_REAL(-0.499997), CCDDBL_REAL( 0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.262869), CCDDBL_REAL(-0.809012), CCDDBL_REAL( 0.525738)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.850648), CCDDBL_REAL( 0.000000), CCDDBL_REAL( 0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.262869), CCDDBL_REAL( 0.809012), CCDDBL_REAL( 0.525738)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.688190), CCDDBL_REAL( 0.499997), CCDDBL_REAL( 0.525736)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.525730), CCDDBL_REAL( 0.000000), CCDDBL_REAL( 0.850652)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.162456), CCDDBL_REAL(-0.499995), CCDDBL_REAL( 0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.425323), CCDDBL_REAL(-0.309011), CCDDBL_REAL( 0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL(-0.425323), CCDDBL_REAL( 0.309011), CCDDBL_REAL( 0.850654)),
	CCDDBL_VEC3_STATIC(CCDDBL_REAL( 0.162456), CCDDBL_REAL( 0.499995), CCDDBL_REAL( 0.850654))
};
ccddbl_vec3_t *ccddbl_points_on_sphere = points_on_sphere;
size_t ccddbl_points_on_sphere_len = sizeof(points_on_sphere) / sizeof(ccddbl_vec3_t);


_ccddbl_inline ccddbl_real_t __ccddblVec3PointSegmentDist2(const ccddbl_vec3_t *P,
                                                  const ccddbl_vec3_t *x0,
                                                  const ccddbl_vec3_t *b,
                                                  ccddbl_vec3_t *witness)
{
    // The computation comes from solving equation of segment:
    //      S(t) = x0 + t.d
    //          where - x0 is initial point of segment
    //                - d is direction of segment from x0 (|d| > 0)
    //                - t belongs to <0, 1> interval
    // 
    // Than, distance from a segment to some point P can be expressed:
    //      D(t) = |x0 + t.d - P|^2
    //          which is distance from any point on segment. Minimization
    //          of this function brings distance from P to segment.
    // Minimization of D(t) leads to simple quadratic equation that's
    // solving is straightforward.
    //
    // Bonus of this method is witness point for free.

    ccddbl_real_t dist, t;
    ccddbl_vec3_t d, a;

    // direction of segment
    ccddblVec3Sub2(&d, b, x0);

    // precompute vector from P to x0
    ccddblVec3Sub2(&a, x0, P);

    t  = -CCDDBL_REAL(1.) * ccddblVec3Dot(&a, &d);
    t /= ccddblVec3Len2(&d);

    if (t < CCDDBL_ZERO || ccddblIsZero(t)){
        dist = ccddblVec3Dist2(x0, P);
        if (witness)
            ccddblVec3Copy(witness, x0);
    }else if (t > CCDDBL_ONE || ccddblEq(t, CCDDBL_ONE)){
        dist = ccddblVec3Dist2(b, P);
        if (witness)
            ccddblVec3Copy(witness, b);
    }else{
        if (witness){
            ccddblVec3Copy(witness, &d);
            ccddblVec3Scale(witness, t);
            ccddblVec3Add(witness, x0);
            dist = ccddblVec3Dist2(witness, P);
        }else{
            // recycling variables
            ccddblVec3Scale(&d, t);
            ccddblVec3Add(&d, &a);
            dist = ccddblVec3Len2(&d);
        }
    }

    return dist;
}

ccddbl_real_t ccddblVec3PointSegmentDist2(const ccddbl_vec3_t *P,
                                    const ccddbl_vec3_t *x0, const ccddbl_vec3_t *b,
                                    ccddbl_vec3_t *witness)
{
    return __ccddblVec3PointSegmentDist2(P, x0, b, witness);
}

ccddbl_real_t ccddblVec3PointTriDist2(const ccddbl_vec3_t *P,
                                const ccddbl_vec3_t *x0, const ccddbl_vec3_t *B,
                                const ccddbl_vec3_t *C,
                                ccddbl_vec3_t *witness)
{
    // Computation comes from analytic expression for triangle (x0, B, C)
    //      T(s, t) = x0 + s.d1 + t.d2, where d1 = B - x0 and d2 = C - x0 and
    // Then equation for distance is:
    //      D(s, t) = | T(s, t) - P |^2
    // This leads to minimization of quadratic function of two variables.
    // The solution from is taken only if s is between 0 and 1, t is
    // between 0 and 1 and t + s < 1, otherwise distance from segment is
    // computed.

    ccddbl_vec3_t d1, d2, a;
    ccddbl_real_t u, v, w, p, q, r, d;
    ccddbl_real_t s, t, dist, dist2;
    ccddbl_vec3_t witness2;

    ccddblVec3Sub2(&d1, B, x0);
    ccddblVec3Sub2(&d2, C, x0);
    ccddblVec3Sub2(&a, x0, P);

    u = ccddblVec3Dot(&a, &a);
    v = ccddblVec3Dot(&d1, &d1);
    w = ccddblVec3Dot(&d2, &d2);
    p = ccddblVec3Dot(&a, &d1);
    q = ccddblVec3Dot(&a, &d2);
    r = ccddblVec3Dot(&d1, &d2);

    d = w * v - r * r;
    if (ccddblIsZero(d)){
        // To avoid division by zero for zero (or near zero) area triangles
        s = t = -1.;
    }else{
        s = (q * r - w * p) / d;
        t = (-s * r - q) / w;
    }

    if ((ccddblIsZero(s) || s > CCDDBL_ZERO)
            && (ccddblEq(s, CCDDBL_ONE) || s < CCDDBL_ONE)
            && (ccddblIsZero(t) || t > CCDDBL_ZERO)
            && (ccddblEq(t, CCDDBL_ONE) || t < CCDDBL_ONE)
            && (ccddblEq(t + s, CCDDBL_ONE) || t + s < CCDDBL_ONE)){

        if (witness){
            ccddblVec3Scale(&d1, s);
            ccddblVec3Scale(&d2, t);
            ccddblVec3Copy(witness, x0);
            ccddblVec3Add(witness, &d1);
            ccddblVec3Add(witness, &d2);

            dist = ccddblVec3Dist2(witness, P);
        }else{
            dist  = s * s * v;
            dist += t * t * w;
            dist += CCDDBL_REAL(2.) * s * t * r;
            dist += CCDDBL_REAL(2.) * s * p;
            dist += CCDDBL_REAL(2.) * t * q;
            dist += u;
        }
    }else{
        dist = __ccddblVec3PointSegmentDist2(P, x0, B, witness);

        dist2 = __ccddblVec3PointSegmentDist2(P, x0, C, &witness2);
        if (dist2 < dist){
            dist = dist2;
            if (witness)
                ccddblVec3Copy(witness, &witness2);
        }

        dist2 = __ccddblVec3PointSegmentDist2(P, B, C, &witness2);
        if (dist2 < dist){
            dist = dist2;
            if (witness)
                ccddblVec3Copy(witness, &witness2);
        }
    }

    return dist;
}
