/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010,2011 Daniel Fiser <danfis@danfis.cz>
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

#include <stdlib.h>
#include <ccddbl/ccddbl.h>
#include <ccddbl/simplex.h>
#include "dbg.h"

/** Finds origin (center) of Minkowski difference (actually it can be any
 *  interior point of Minkowski difference. */
_ccddbl_inline void findOrigin(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                            ccddbl_support_t *center);

/** Discovers initial portal - that is tetrahedron that intersects with
 *  origin ray (ray from center of Minkowski diff to (0,0,0).
 *
 *  Returns -1 if already recognized that origin is outside Minkowski
 *  portal.
 *  Returns 1 if origin lies on v1 of simplex (only v0 and v1 are present
 *  in simplex).
 *  Returns 2 if origin lies on v0-v1 segment.
 *  Returns 0 if portal was built.
 */
static int discoverPortal(const void *obj1, const void *obj2,
                          const ccddbl_t *ccddbl, ccddbl_simplex_t *portal);


/** Expands portal towards origin and determine if objects intersect.
 *  Already established portal must be given as argument.
 *  If intersection is found 0 is returned, -1 otherwise */
static int refinePortal(const void *obj1, const void *obj2,
                        const ccddbl_t *ccddbl, ccddbl_simplex_t *portal);

/** Finds penetration info by expanding provided portal. */
static void findPenetr(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                       ccddbl_simplex_t *portal,
                       ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos);

/** Finds penetration info if origin lies on portal's v1 */
static void findPenetrTouch(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                            ccddbl_simplex_t *portal,
                            ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos);

/** Find penetration info if origin lies on portal's segment v0-v1 */
static void findPenetrSegment(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                              ccddbl_simplex_t *portal,
                              ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos);

/** Finds position vector from fully established portal */
static void findPos(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                    const ccddbl_simplex_t *portal, ccddbl_vec3_t *pos);

/** Extends portal with new support point.
 *  Portal must have face v1-v2-v3 arranged to face outside portal. */
_ccddbl_inline void expandPortal(ccddbl_simplex_t *portal,
                              const ccddbl_support_t *v4);

/** Fill dir with direction outside portal. Portal's v1-v2-v3 face must be
 *  arranged in correct order! */
_ccddbl_inline void portalDir(const ccddbl_simplex_t *portal, ccddbl_vec3_t *dir);

/** Returns true if portal encapsules origin (0,0,0), dir is direction of
 *  v1-v2-v3 face. */
_ccddbl_inline int portalEncapsulesOrigin(const ccddbl_simplex_t *portal,
                                       const ccddbl_vec3_t *dir);

/** Returns true if portal with new point v4 would reach specified
 *  tolerance (i.e. returns true if portal can _not_ significantly expand
 *  within Minkowski difference).
 *
 *  v4 is candidate for new point in portal, dir is direction in which v4
 *  was obtained. */
_ccddbl_inline int portalReachTolerance(const ccddbl_simplex_t *portal,
                                     const ccddbl_support_t *v4,
                                     const ccddbl_vec3_t *dir,
                                     const ccddbl_t *ccddbl);

/** Returns true if portal expanded by new point v4 could possibly contain
 *  origin, dir is direction in which v4 was obtained. */
_ccddbl_inline int portalCanEncapsuleOrigin(const ccddbl_simplex_t *portal,   
                                         const ccddbl_support_t *v4,
                                         const ccddbl_vec3_t *dir);


int ccddblMPRIntersect(const void *obj1, const void *obj2, const ccddbl_t *ccddbl)
{
    ccddbl_simplex_t portal;
    int res;

    // Phase 1: Portal discovery - find portal that intersects with origin
    // ray (ray from center of Minkowski diff to origin of coordinates)
    res = discoverPortal(obj1, obj2, ccddbl, &portal);
    if (res < 0)
        return 0;
    if (res > 0)
        return 1;

    // Phase 2: Portal refinement
    res = refinePortal(obj1, obj2, ccddbl, &portal);
    return (res == 0 ? 1 : 0);
}

int ccddblMPRPenetration(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                      ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos)
{
    ccddbl_simplex_t portal;
    int res;

    // Phase 1: Portal discovery
    res = discoverPortal(obj1, obj2, ccddbl, &portal);
    if (res < 0){
        // Origin isn't inside portal - no collision.
        return -1;

    }else if (res == 1){
        // Touching contact on portal's v1.
        findPenetrTouch(obj1, obj2, ccddbl, &portal, depth, dir, pos);

    }else if (res == 2){
        // Origin lies on v0-v1 segment.
        findPenetrSegment(obj1, obj2, ccddbl, &portal, depth, dir, pos);

    }else if (res == 0){
        // Phase 2: Portal refinement
        res = refinePortal(obj1, obj2, ccddbl, &portal);
        if (res < 0)
            return -1;

        // Phase 3. Penetration info
        findPenetr(obj1, obj2, ccddbl, &portal, depth, dir, pos);
    }

    return 0;
}



_ccddbl_inline void findOrigin(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                            ccddbl_support_t *center)
{
    ccddbl->center1(obj1, &center->v1);
    ccddbl->center2(obj2, &center->v2);
    ccddblVec3Sub2(&center->v, &center->v1, &center->v2);
}

static int discoverPortal(const void *obj1, const void *obj2,
                          const ccddbl_t *ccddbl, ccddbl_simplex_t *portal)
{
    ccddbl_vec3_t dir, va, vb;
    ccddbl_real_t dot;
    int cont;

    // vertex 0 is center of portal
    findOrigin(obj1, obj2, ccddbl, ccddblSimplexPointW(portal, 0));
    ccddblSimplexSetSize(portal, 1);

    if (ccddblVec3Eq(&ccddblSimplexPoint(portal, 0)->v, ccddbl_vec3_origin)){
        // Portal's center lies on origin (0,0,0) => we know that objects
        // intersect but we would need to know penetration info.
        // So move center little bit...
        ccddblVec3Set(&va, CCDDBL_EPS * CCDDBL_REAL(10.), CCDDBL_ZERO, CCDDBL_ZERO);
        ccddblVec3Add(&ccddblSimplexPointW(portal, 0)->v, &va);
    }


    // vertex 1 = support in direction of origin
    ccddblVec3Copy(&dir, &ccddblSimplexPoint(portal, 0)->v);
    ccddblVec3Scale(&dir, CCDDBL_REAL(-1.));
    ccddblVec3Normalize(&dir);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, ccddblSimplexPointW(portal, 1));
    ccddblSimplexSetSize(portal, 2);

    // test if origin isn't outside of v1
    dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 1)->v, &dir);
    if (ccddblIsZero(dot) || dot < CCDDBL_ZERO)
        return -1;


    // vertex 2
    ccddblVec3Cross(&dir, &ccddblSimplexPoint(portal, 0)->v,
                       &ccddblSimplexPoint(portal, 1)->v);
    if (ccddblIsZero(ccddblVec3Len2(&dir))){
        if (ccddblVec3Eq(&ccddblSimplexPoint(portal, 1)->v, ccddbl_vec3_origin)){
            // origin lies on v1
            return 1;
        }else{
            // origin lies on v0-v1 segment
            return 2;
        }
    }

    ccddblVec3Normalize(&dir);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, ccddblSimplexPointW(portal, 2));
    dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 2)->v, &dir);
    if (ccddblIsZero(dot) || dot < CCDDBL_ZERO)
        return -1;

    ccddblSimplexSetSize(portal, 3);

    // vertex 3 direction
    ccddblVec3Sub2(&va, &ccddblSimplexPoint(portal, 1)->v,
                     &ccddblSimplexPoint(portal, 0)->v);
    ccddblVec3Sub2(&vb, &ccddblSimplexPoint(portal, 2)->v,
                     &ccddblSimplexPoint(portal, 0)->v);
    ccddblVec3Cross(&dir, &va, &vb);
    ccddblVec3Normalize(&dir);

    // it is better to form portal faces to be oriented "outside" origin
    dot = ccddblVec3Dot(&dir, &ccddblSimplexPoint(portal, 0)->v);
    if (dot > CCDDBL_ZERO){
        ccddblSimplexSwap(portal, 1, 2);
        ccddblVec3Scale(&dir, CCDDBL_REAL(-1.));
    }

    while (ccddblSimplexSize(portal) < 4){
        __ccddblSupport(obj1, obj2, &dir, ccddbl, ccddblSimplexPointW(portal, 3));
        dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 3)->v, &dir);
        if (ccddblIsZero(dot) || dot < CCDDBL_ZERO)
            return -1;

        cont = 0;

        // test if origin is outside (v1, v0, v3) - set v2 as v3 and
        // continue
        ccddblVec3Cross(&va, &ccddblSimplexPoint(portal, 1)->v,
                          &ccddblSimplexPoint(portal, 3)->v);
        dot = ccddblVec3Dot(&va, &ccddblSimplexPoint(portal, 0)->v);
        if (dot < CCDDBL_ZERO && !ccddblIsZero(dot)){
            ccddblSimplexSet(portal, 2, ccddblSimplexPoint(portal, 3));
            cont = 1;
        }

        if (!cont){
            // test if origin is outside (v3, v0, v2) - set v1 as v3 and
            // continue
            ccddblVec3Cross(&va, &ccddblSimplexPoint(portal, 3)->v,
                              &ccddblSimplexPoint(portal, 2)->v);
            dot = ccddblVec3Dot(&va, &ccddblSimplexPoint(portal, 0)->v);
            if (dot < CCDDBL_ZERO && !ccddblIsZero(dot)){
                ccddblSimplexSet(portal, 1, ccddblSimplexPoint(portal, 3));
                cont = 1;
            }
        }

        if (cont){
            ccddblVec3Sub2(&va, &ccddblSimplexPoint(portal, 1)->v,
                             &ccddblSimplexPoint(portal, 0)->v);
            ccddblVec3Sub2(&vb, &ccddblSimplexPoint(portal, 2)->v,
                             &ccddblSimplexPoint(portal, 0)->v);
            ccddblVec3Cross(&dir, &va, &vb);
            ccddblVec3Normalize(&dir);
        }else{
            ccddblSimplexSetSize(portal, 4);
        }
    }

    return 0;
}

static int refinePortal(const void *obj1, const void *obj2,
                        const ccddbl_t *ccddbl, ccddbl_simplex_t *portal)
{
    ccddbl_vec3_t dir;
    ccddbl_support_t v4;

    while (1){
        // compute direction outside the portal (from v0 throught v1,v2,v3
        // face)
        portalDir(portal, &dir);

        // test if origin is inside the portal
        if (portalEncapsulesOrigin(portal, &dir))
            return 0;

        // get next support point
        __ccddblSupport(obj1, obj2, &dir, ccddbl, &v4);

        // test if v4 can expand portal to contain origin and if portal
        // expanding doesn't reach given tolerance
        if (!portalCanEncapsuleOrigin(portal, &v4, &dir)
                || portalReachTolerance(portal, &v4, &dir, ccddbl)){
            return -1;
        }

        // v1-v2-v3 triangle must be rearranged to face outside Minkowski
        // difference (direction from v0).
        expandPortal(portal, &v4);
    }

    return -1;
}


static void findPenetr(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                       ccddbl_simplex_t *portal,
                       ccddbl_real_t *depth, ccddbl_vec3_t *pdir, ccddbl_vec3_t *pos)
{
    ccddbl_vec3_t dir;
    ccddbl_support_t v4;
    unsigned long iterations;

    iterations = 0UL;
    while (1){
        // compute portal direction and obtain next support point
        portalDir(portal, &dir);
        __ccddblSupport(obj1, obj2, &dir, ccddbl, &v4);

        // reached tolerance -> find penetration info
        if (portalReachTolerance(portal, &v4, &dir, ccddbl)
                || iterations > ccddbl->max_iterations){
            *depth = ccddblVec3PointTriDist2(ccddbl_vec3_origin,
                                          &ccddblSimplexPoint(portal, 1)->v,
                                          &ccddblSimplexPoint(portal, 2)->v,
                                          &ccddblSimplexPoint(portal, 3)->v,
                                          pdir);
            *depth = CCDDBL_SQRT(*depth);
            if (ccddblIsZero(*depth)){
                // If depth is zero, then we have a touching contact.
                // So following findPenetrTouch(), we assign zero to
                // the direction vector (it can actually be anything
                // according to the decription of ccddblMPRPenetration
                // function).
                ccddblVec3Copy(pdir, ccddbl_vec3_origin);
            }else{
                ccddblVec3Normalize(pdir);
            }

            // barycentric coordinates:
            findPos(obj1, obj2, ccddbl, portal, pos);

            return;
        }

        expandPortal(portal, &v4);

        iterations++;
    }
}

static void findPenetrTouch(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                            ccddbl_simplex_t *portal,
                            ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos)
{
    // Touching contact on portal's v1 - so depth is zero and direction
    // is unimportant and pos can be guessed
    *depth = CCDDBL_REAL(0.);
    ccddblVec3Copy(dir, ccddbl_vec3_origin);

    ccddblVec3Copy(pos, &ccddblSimplexPoint(portal, 1)->v1);
    ccddblVec3Add(pos, &ccddblSimplexPoint(portal, 1)->v2);
    ccddblVec3Scale(pos, 0.5);
}

static void findPenetrSegment(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                              ccddbl_simplex_t *portal,
                              ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos)
{
    /*
    ccddbl_vec3_t vec;
    ccddbl_real_t k;
    */

    // Origin lies on v0-v1 segment.
    // Depth is distance to v1, direction also and position must be
    // computed

    ccddblVec3Copy(pos, &ccddblSimplexPoint(portal, 1)->v1);
    ccddblVec3Add(pos, &ccddblSimplexPoint(portal, 1)->v2);
    ccddblVec3Scale(pos, CCDDBL_REAL(0.5));

    /*
    ccddblVec3Sub2(&vec, &ccddblSimplexPoint(portal, 1)->v,
                      &ccddblSimplexPoint(portal, 0)->v);
    k  = CCDDBL_SQRT(ccddblVec3Len2(&ccddblSimplexPoint(portal, 0)->v));
    k /= CCDDBL_SQRT(ccddblVec3Len2(&vec));
    ccddblVec3Scale(&vec, -k);
    ccddblVec3Add(pos, &vec);
    */

    ccddblVec3Copy(dir, &ccddblSimplexPoint(portal, 1)->v);
    *depth = CCDDBL_SQRT(ccddblVec3Len2(dir));
    ccddblVec3Normalize(dir);
}


static void findPos(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                    const ccddbl_simplex_t *portal, ccddbl_vec3_t *pos)
{
    ccddbl_vec3_t dir;
    size_t i;
    ccddbl_real_t b[4], sum, inv;
    ccddbl_vec3_t vec, p1, p2;

    portalDir(portal, &dir);

    // use barycentric coordinates of tetrahedron to find origin
    ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 1)->v,
                       &ccddblSimplexPoint(portal, 2)->v);
    b[0] = ccddblVec3Dot(&vec, &ccddblSimplexPoint(portal, 3)->v);

    ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 3)->v,
                       &ccddblSimplexPoint(portal, 2)->v);
    b[1] = ccddblVec3Dot(&vec, &ccddblSimplexPoint(portal, 0)->v);

    ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 0)->v,
                       &ccddblSimplexPoint(portal, 1)->v);
    b[2] = ccddblVec3Dot(&vec, &ccddblSimplexPoint(portal, 3)->v);

    ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 2)->v,
                       &ccddblSimplexPoint(portal, 1)->v);
    b[3] = ccddblVec3Dot(&vec, &ccddblSimplexPoint(portal, 0)->v);

	sum = b[0] + b[1] + b[2] + b[3];

    if (ccddblIsZero(sum) || sum < CCDDBL_ZERO){
		b[0] = CCDDBL_REAL(0.);

        ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 2)->v,
                           &ccddblSimplexPoint(portal, 3)->v);
        b[1] = ccddblVec3Dot(&vec, &dir);
        ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 3)->v,
                           &ccddblSimplexPoint(portal, 1)->v);
        b[2] = ccddblVec3Dot(&vec, &dir);
        ccddblVec3Cross(&vec, &ccddblSimplexPoint(portal, 1)->v,
                           &ccddblSimplexPoint(portal, 2)->v);
        b[3] = ccddblVec3Dot(&vec, &dir);

		sum = b[1] + b[2] + b[3];
	}

	inv = CCDDBL_REAL(1.) / sum;

    ccddblVec3Copy(&p1, ccddbl_vec3_origin);
    ccddblVec3Copy(&p2, ccddbl_vec3_origin);
    for (i = 0; i < 4; i++){
        ccddblVec3Copy(&vec, &ccddblSimplexPoint(portal, i)->v1);
        ccddblVec3Scale(&vec, b[i]);
        ccddblVec3Add(&p1, &vec);

        ccddblVec3Copy(&vec, &ccddblSimplexPoint(portal, i)->v2);
        ccddblVec3Scale(&vec, b[i]);
        ccddblVec3Add(&p2, &vec);
    }
    ccddblVec3Scale(&p1, inv);
    ccddblVec3Scale(&p2, inv);

    ccddblVec3Copy(pos, &p1);
    ccddblVec3Add(pos, &p2);
    ccddblVec3Scale(pos, 0.5);
}

_ccddbl_inline void expandPortal(ccddbl_simplex_t *portal,
                              const ccddbl_support_t *v4)
{
    ccddbl_real_t dot;
    ccddbl_vec3_t v4v0;

    ccddblVec3Cross(&v4v0, &v4->v, &ccddblSimplexPoint(portal, 0)->v);
    dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 1)->v, &v4v0);
    if (dot > CCDDBL_ZERO){
        dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 2)->v, &v4v0);
        if (dot > CCDDBL_ZERO){
            ccddblSimplexSet(portal, 1, v4);
        }else{
            ccddblSimplexSet(portal, 3, v4);
        }
    }else{
        dot = ccddblVec3Dot(&ccddblSimplexPoint(portal, 3)->v, &v4v0);
        if (dot > CCDDBL_ZERO){
            ccddblSimplexSet(portal, 2, v4);
        }else{
            ccddblSimplexSet(portal, 1, v4);
        }
    }
}

_ccddbl_inline void portalDir(const ccddbl_simplex_t *portal, ccddbl_vec3_t *dir)
{
    ccddbl_vec3_t v2v1, v3v1;

    ccddblVec3Sub2(&v2v1, &ccddblSimplexPoint(portal, 2)->v,
                       &ccddblSimplexPoint(portal, 1)->v);
    ccddblVec3Sub2(&v3v1, &ccddblSimplexPoint(portal, 3)->v,
                       &ccddblSimplexPoint(portal, 1)->v);
    ccddblVec3Cross(dir, &v2v1, &v3v1);
    ccddblVec3Normalize(dir);
}

_ccddbl_inline int portalEncapsulesOrigin(const ccddbl_simplex_t *portal,
                                       const ccddbl_vec3_t *dir)
{
    ccddbl_real_t dot;
    dot = ccddblVec3Dot(dir, &ccddblSimplexPoint(portal, 1)->v);
    return ccddblIsZero(dot) || dot > CCDDBL_ZERO;
}

_ccddbl_inline int portalReachTolerance(const ccddbl_simplex_t *portal,
                                     const ccddbl_support_t *v4,
                                     const ccddbl_vec3_t *dir,
                                     const ccddbl_t *ccddbl)
{
    ccddbl_real_t dv1, dv2, dv3, dv4;
    ccddbl_real_t dot1, dot2, dot3;

    // find the smallest dot product of dir and {v1-v4, v2-v4, v3-v4}

    dv1 = ccddblVec3Dot(&ccddblSimplexPoint(portal, 1)->v, dir);
    dv2 = ccddblVec3Dot(&ccddblSimplexPoint(portal, 2)->v, dir);
    dv3 = ccddblVec3Dot(&ccddblSimplexPoint(portal, 3)->v, dir);
    dv4 = ccddblVec3Dot(&v4->v, dir);

    dot1 = dv4 - dv1;
    dot2 = dv4 - dv2;
    dot3 = dv4 - dv3;

    dot1 = CCDDBL_FMIN(dot1, dot2);
    dot1 = CCDDBL_FMIN(dot1, dot3);

    return ccddblEq(dot1, ccddbl->mpr_tolerance) || dot1 < ccddbl->mpr_tolerance;
}

_ccddbl_inline int portalCanEncapsuleOrigin(const ccddbl_simplex_t *portal,   
                                         const ccddbl_support_t *v4,
                                         const ccddbl_vec3_t *dir)
{
    ccddbl_real_t dot;
    dot = ccddblVec3Dot(&v4->v, dir);
    return ccddblIsZero(dot) || dot > CCDDBL_ZERO;
}
