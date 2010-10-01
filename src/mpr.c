#include <stdlib.h>
#include "gjk.h"
#include "simplex.h"
#include "dbg.h"

// TODO: add more explanatory comments.

/** Finds origin (center) of Minkowski difference (actually it can be any
 *  interior point of Minkowski difference. */
_gjk_inline void findOrigin(const void *obj1, const void *obj2, const gjk_t *gjk,
                            gjk_support_t *center);

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
                          const gjk_t *gjk, gjk_simplex_t *portal);


/** Expands portal towards origin and determine if objects intersect.
 *  Already established portal must be given as argument.
 *  If intersection is found 0 is returned, -1 otherwise */
static int refinePortal(const void *obj1, const void *obj2,
                        const gjk_t *gjk, gjk_simplex_t *portal);

/** Finds penetration info by expanding provided portal. */
static void findPenetr(const void *obj1, const void *obj2, const gjk_t *gjk,
                       gjk_simplex_t *portal,
                       gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos);

/** Finds penetration info if origin lies on portal's v1 */
static void findPenetrTouch(const void *obj1, const void *obj2, const gjk_t *gjk,
                            gjk_simplex_t *portal,
                            gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos);

/** Find penetration info if origin lies on portal's segment v0-v1 */
static void findPenetrSegment(const void *obj1, const void *obj2, const gjk_t *gjk,
                              gjk_simplex_t *portal,
                              gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos);

/** Finds position vector from fully established portal */
static void findPos(const void *obj1, const void *obj2, const gjk_t *gjk,
                    const gjk_simplex_t *portal, gjk_vec3_t *pos);

/** Extends portal with new support point.
 *  Portal must have face v1-v2-v3 arranged to face outside portal. */
_gjk_inline void expandPortal(gjk_simplex_t *portal,
                              const gjk_support_t *v4);

/** Fill dir with direction outside portal. Portal's v1-v2-v3 face must be
 *  arranged in correct order! */
_gjk_inline void portalDir(const gjk_simplex_t *portal, gjk_vec3_t *dir);

/** Returns true if portal encapsules origin (0,0,0), dir is direction of
 *  v1-v2-v3 face. */
_gjk_inline int portalEncapsulesOrigin(const gjk_simplex_t *portal,
                                       const gjk_vec3_t *dir);

/** Returns true if portal with new point v4 would reach specified
 *  tolerance (i.e. returns true if portal can _not_ significantly expand
 *  within Minkowski difference).
 *
 *  v4 is candidate for new point in portal, dir is direction in which v4
 *  was obtained. */
_gjk_inline int portalReachTolerance(const gjk_simplex_t *portal,
                                     const gjk_support_t *v4,
                                     const gjk_vec3_t *dir,
                                     const gjk_t *gjk);

/** Returns true if portal expanded by new point v4 could possibly contain
 *  origin, dir is direction in which v4 was obtained. */
_gjk_inline int portalCanEncapsuleOrigin(const gjk_simplex_t *portal,   
                                         const gjk_support_t *v4,
                                         const gjk_vec3_t *dir);


int gjkMPRIntersect(const void *obj1, const void *obj2, const gjk_t *gjk)
{
    gjk_simplex_t portal;
    int res;

    // Phase 1: Portal discovery - find portal that intersects with origin
    // ray (ray from center of Minkowski diff to origin of coordinates)
    res = discoverPortal(obj1, obj2, gjk, &portal);
    if (res < 0)
        return 0;
    if (res > 0)
        return 1;

    // Phase 2: Portal refinement
    res = refinePortal(obj1, obj2, gjk, &portal);
    return (res == 0 ? 1 : 0);
}

int gjkMPRPenetration(const void *obj1, const void *obj2, const gjk_t *gjk,
                      gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos)
{
    gjk_simplex_t portal;
    int res;

    // Phase 1: Portal discovery
    res = discoverPortal(obj1, obj2, gjk, &portal);
    if (res < 0){
        // Origin isn't inside portal - no collision.
        return -1;

    }else if (res == 1){
        // Touching contact on portal's v1.
        findPenetrTouch(obj1, obj2, gjk, &portal, depth, dir, pos);

    }else if (res == 2){
        // Origin lies on v0-v1 segment.
        findPenetrSegment(obj1, obj2, gjk, &portal, depth, dir, pos);

    }else if (res == 0){
        // Phase 2: Portal refinement
        res = refinePortal(obj1, obj2, gjk, &portal);
        if (res < 0)
            return -1;

        // Phase 3. Penetration info
        findPenetr(obj1, obj2, gjk, &portal, depth, dir, pos);
    }

    return 0;
}



_gjk_inline void findOrigin(const void *obj1, const void *obj2, const gjk_t *gjk,
                            gjk_support_t *center)
{
    gjk->center1(obj1, &center->v1);
    gjk->center2(obj2, &center->v2);
    gjkVec3Sub2(&center->v, &center->v1, &center->v2);
}

static int discoverPortal(const void *obj1, const void *obj2,
                          const gjk_t *gjk, gjk_simplex_t *portal)
{
    gjk_vec3_t dir, va, vb;
    gjk_real_t dot;
    int cont;

    // vertex 0 is center of portal
    findOrigin(obj1, obj2, gjk, gjkSimplexPointW(portal, 0));
    gjkSimplexSetSize(portal, 1);

    if (gjkVec3Eq(&gjkSimplexPoint(portal, 0)->v, gjk_vec3_origin)){
        // Portal's center lies on origin (0,0,0) => we know that objects
        // intersect but we would need to know penetration info.
        // So move center little bit...
        gjkVec3Set(&va, GJK_EPS * GJK_REAL(10.), GJK_ZERO, GJK_ZERO);
        gjkVec3Add(&gjkSimplexPointW(portal, 0)->v, &va);
    }


    // vertex 1 = support in direction of origin
    gjkVec3Copy(&dir, &gjkSimplexPoint(portal, 0)->v);
    gjkVec3Scale(&dir, GJK_REAL(-1.));
    gjkVec3Normalize(&dir);
    __gjkSupport(obj1, obj2, &dir, gjk, gjkSimplexPointW(portal, 1));
    gjkSimplexSetSize(portal, 2);

    // test if origin isn't outside of v1
    dot = gjkVec3Dot(&gjkSimplexPoint(portal, 1)->v, &dir);
    if (gjkIsZero(dot) || dot < GJK_ZERO)
        return -1;


    // vertex 2
    gjkVec3Cross(&dir, &gjkSimplexPoint(portal, 0)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    if (gjkVec3Eq(&dir, gjk_vec3_origin)){
        if (gjkVec3Eq(&gjkSimplexPoint(portal, 1)->v, gjk_vec3_origin)){
            // origin lies on v1
            return 1;
        }else{
            // origin lies on v0-v1 segment
            return 2;
        }
    }

    gjkVec3Normalize(&dir);
    __gjkSupport(obj1, obj2, &dir, gjk, gjkSimplexPointW(portal, 2));
    dot = gjkVec3Dot(&gjkSimplexPoint(portal, 2)->v, &dir);
    if (gjkIsZero(dot) || dot < GJK_ZERO)
        return -1;

    gjkSimplexSetSize(portal, 3);

    // vertex 3 direction
    gjkVec3Sub2(&va, &gjkSimplexPoint(portal, 1)->v,
                     &gjkSimplexPoint(portal, 0)->v);
    gjkVec3Sub2(&vb, &gjkSimplexPoint(portal, 2)->v,
                     &gjkSimplexPoint(portal, 0)->v);
    gjkVec3Cross(&dir, &va, &vb);
    gjkVec3Normalize(&dir);

    // it is better to form portal faces to be oriented "outside" origin
    dot = gjkVec3Dot(&dir, &gjkSimplexPoint(portal, 0)->v);
    if (dot > GJK_ZERO){
        gjkSimplexSwap(portal, 1, 2);
        gjkVec3Scale(&dir, GJK_REAL(-1.));
    }

    while (gjkSimplexSize(portal) < 4){
        __gjkSupport(obj1, obj2, &dir, gjk, gjkSimplexPointW(portal, 3));
        dot = gjkVec3Dot(&gjkSimplexPoint(portal, 3)->v, &dir);
        if (gjkIsZero(dot) || dot < GJK_ZERO)
            return -1;

        cont = 0;

        // test if origin is outside (v1, v0, v3) - set v2 as v3 and
        // continue
        gjkVec3Cross(&va, &gjkSimplexPoint(portal, 1)->v,
                          &gjkSimplexPoint(portal, 3)->v);
        dot = gjkVec3Dot(&va, &gjkSimplexPoint(portal, 0)->v);
        if (dot < GJK_ZERO){
            gjkSimplexSet(portal, 2, gjkSimplexPoint(portal, 3));
            cont = 1;
        }

        if (!cont){
            // test if origin is outside (v3, v0, v2) - set v1 as v3 and
            // continue
            gjkVec3Cross(&va, &gjkSimplexPoint(portal, 3)->v,
                              &gjkSimplexPoint(portal, 2)->v);
            dot = gjkVec3Dot(&va, &gjkSimplexPoint(portal, 0)->v);
            if (dot < GJK_ZERO){
                gjkSimplexSet(portal, 1, gjkSimplexPoint(portal, 3));
                cont = 1;
            }
        }

        if (cont){
            gjkVec3Sub2(&va, &gjkSimplexPoint(portal, 1)->v,
                             &gjkSimplexPoint(portal, 0)->v);
            gjkVec3Sub2(&vb, &gjkSimplexPoint(portal, 2)->v,
                             &gjkSimplexPoint(portal, 0)->v);
            gjkVec3Cross(&dir, &va, &vb);
            gjkVec3Normalize(&dir);
        }else{
            gjkSimplexSetSize(portal, 4);
        }
    }

    return 0;
}

static int refinePortal(const void *obj1, const void *obj2,
                        const gjk_t *gjk, gjk_simplex_t *portal)
{
    gjk_vec3_t dir;
    gjk_support_t v4;

    while (1){
        // compute direction outside the portal (from v0 throught v1,v2,v3
        // face)
        portalDir(portal, &dir);

        // test if origin is inside the portal
        if (portalEncapsulesOrigin(portal, &dir))
            return 0;

        // get next support point
        __gjkSupport(obj1, obj2, &dir, gjk, &v4);

        // test if v4 can expand portal to contain origin and if portal
        // expanding doesn't reach given tolerance
        if (!portalCanEncapsuleOrigin(portal, &v4, &dir)
                || portalReachTolerance(portal, &v4, &dir, gjk)){
            return -1;
        }

        // v1-v2-v3 triangle must be rearranged to face outside Minkowski
        // difference (direction from v0).
        expandPortal(portal, &v4);
    }

    return -1;
}


static void findPenetr(const void *obj1, const void *obj2, const gjk_t *gjk,
                       gjk_simplex_t *portal,
                       gjk_real_t *depth, gjk_vec3_t *pdir, gjk_vec3_t *pos)
{
    gjk_vec3_t dir;
    gjk_support_t v4;

    while (1){
        // compute portal direction and obtain next support point
        portalDir(portal, &dir);
        __gjkSupport(obj1, obj2, &dir, gjk, &v4);

        // reached tolerance -> find penetration info
        if (portalReachTolerance(portal, &v4, &dir, gjk)){
            *depth = gjkVec3PointTriDist2(gjk_vec3_origin,
                                          &gjkSimplexPoint(portal, 1)->v,
                                          &gjkSimplexPoint(portal, 2)->v,
                                          &gjkSimplexPoint(portal, 3)->v,
                                          pdir);
            *depth = GJK_SQRT(*depth);
            gjkVec3Normalize(pdir);

            // barycentric coordinates:
            findPos(obj1, obj2, gjk, portal, pos);

            return;
        }

        expandPortal(portal, &v4);
    }
}

static void findPenetrTouch(const void *obj1, const void *obj2, const gjk_t *gjk,
                            gjk_simplex_t *portal,
                            gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos)
{
    // Touching contact on portal's v1 - so depth is zero and direction
    // is unimportant and pos can be guessed
    *depth = GJK_REAL(0.);
    gjkVec3Copy(dir, gjk_vec3_origin);

    gjkVec3Copy(pos, &gjkSimplexPoint(portal, 1)->v1);
    gjkVec3Add(pos, &gjkSimplexPoint(portal, 1)->v2);
    gjkVec3Scale(pos, 0.5);
}

static void findPenetrSegment(const void *obj1, const void *obj2, const gjk_t *gjk,
                              gjk_simplex_t *portal,
                              gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos)
{
    /*
    gjk_vec3_t vec;
    gjk_real_t k;
    */

    // Origin lies on v0-v1 segment.
    // Depth is distance to v1, direction also and position must be
    // computed

    gjkVec3Copy(pos, &gjkSimplexPoint(portal, 1)->v1);
    gjkVec3Add(pos, &gjkSimplexPoint(portal, 1)->v2);
    gjkVec3Scale(pos, GJK_REAL(0.5));

    /*
    gjkVec3Sub2(&vec, &gjkSimplexPoint(portal, 1)->v,
                      &gjkSimplexPoint(portal, 0)->v);
    k  = GJK_SQRT(gjkVec3Len2(&gjkSimplexPoint(portal, 0)->v));
    k /= GJK_SQRT(gjkVec3Len2(&vec));
    gjkVec3Scale(&vec, -k);
    gjkVec3Add(pos, &vec);
    */

    gjkVec3Copy(dir, &gjkSimplexPoint(portal, 1)->v);
    *depth = GJK_SQRT(gjkVec3Len2(dir));
    gjkVec3Normalize(dir);
}


static void findPos(const void *obj1, const void *obj2, const gjk_t *gjk,
                    const gjk_simplex_t *portal, gjk_vec3_t *pos)
{
    gjk_vec3_t dir;
    size_t i;
    gjk_real_t b[4], sum, inv;
    gjk_vec3_t vec, p1, p2;

    portalDir(portal, &dir);

    // use barycentric coordinates of tetrahedron to find origin
    gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 1)->v,
                       &gjkSimplexPoint(portal, 2)->v);
    b[0] = gjkVec3Dot(&vec, &gjkSimplexPoint(portal, 3)->v);

    gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 3)->v,
                       &gjkSimplexPoint(portal, 2)->v);
    b[1] = gjkVec3Dot(&vec, &gjkSimplexPoint(portal, 0)->v);

    gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 0)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    b[2] = gjkVec3Dot(&vec, &gjkSimplexPoint(portal, 3)->v);

    gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 2)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    b[3] = gjkVec3Dot(&vec, &gjkSimplexPoint(portal, 0)->v);

	sum = b[0] + b[1] + b[2] + b[3];

    if (gjkIsZero(sum) || sum < GJK_ZERO){
		b[0] = GJK_REAL(0.);

        gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 2)->v,
                           &gjkSimplexPoint(portal, 3)->v);
        b[1] = gjkVec3Dot(&vec, &dir);
        gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 3)->v,
                           &gjkSimplexPoint(portal, 1)->v);
        b[2] = gjkVec3Dot(&vec, &dir);
        gjkVec3Cross(&vec, &gjkSimplexPoint(portal, 1)->v,
                           &gjkSimplexPoint(portal, 2)->v);
        b[3] = gjkVec3Dot(&vec, &dir);

		sum = b[1] + b[2] + b[3];
	}

	inv = GJK_REAL(1.) / sum;

    gjkVec3Copy(&p1, gjk_vec3_origin);
    gjkVec3Copy(&p2, gjk_vec3_origin);
    for (i = 0; i < 4; i++){
        gjkVec3Copy(&vec, &gjkSimplexPoint(portal, i)->v1);
        gjkVec3Scale(&vec, b[i]);
        gjkVec3Add(&p1, &vec);

        gjkVec3Copy(&vec, &gjkSimplexPoint(portal, i)->v2);
        gjkVec3Scale(&vec, b[i]);
        gjkVec3Add(&p2, &vec);
    }
    gjkVec3Scale(&p1, inv);
    gjkVec3Scale(&p2, inv);

    gjkVec3Copy(pos, &p1);
    gjkVec3Add(pos, &p2);
    gjkVec3Scale(pos, 0.5);
}

_gjk_inline void expandPortal(gjk_simplex_t *portal,
                              const gjk_support_t *v4)
{
    gjk_real_t dot;
    gjk_vec3_t v4v0;

    gjkVec3Cross(&v4v0, &v4->v, &gjkSimplexPoint(portal, 0)->v);
    dot = gjkVec3Dot(&gjkSimplexPoint(portal, 1)->v, &v4v0);
    if (dot > GJK_ZERO){
        dot = gjkVec3Dot(&gjkSimplexPoint(portal, 2)->v, &v4v0);
        if (dot > GJK_ZERO){
            gjkSimplexSet(portal, 1, v4);
        }else{
            gjkSimplexSet(portal, 3, v4);
        }
    }else{
        dot = gjkVec3Dot(&gjkSimplexPoint(portal, 3)->v, &v4v0);
        if (dot > GJK_ZERO){
            gjkSimplexSet(portal, 2, v4);
        }else{
            gjkSimplexSet(portal, 1, v4);
        }
    }
}

_gjk_inline void portalDir(const gjk_simplex_t *portal, gjk_vec3_t *dir)
{
    gjk_vec3_t v2v1, v3v1;

    gjkVec3Sub2(&v2v1, &gjkSimplexPoint(portal, 2)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 3)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    gjkVec3Cross(dir, &v2v1, &v3v1);
    gjkVec3Normalize(dir);
}

_gjk_inline int portalEncapsulesOrigin(const gjk_simplex_t *portal,
                                       const gjk_vec3_t *dir)
{
    gjk_real_t dot;
    dot = gjkVec3Dot(dir, &gjkSimplexPoint(portal, 1)->v);
    return gjkIsZero(dot) || dot > GJK_ZERO;
}

_gjk_inline int portalReachTolerance(const gjk_simplex_t *portal,
                                     const gjk_support_t *v4,
                                     const gjk_vec3_t *dir,
                                     const gjk_t *gjk)
{
    gjk_vec3_t vec;
    gjk_real_t dot;

    gjkVec3Sub2(&vec, &v4->v, &gjkSimplexPoint(portal, 3)->v);
    dot = gjkVec3Dot(&vec, dir);

    return gjkEq(dot, gjk->mpr_tolerance) || dot < gjk->mpr_tolerance;
}

_gjk_inline int portalCanEncapsuleOrigin(const gjk_simplex_t *portal,   
                                         const gjk_support_t *v4,
                                         const gjk_vec3_t *dir)
{
    gjk_real_t dot;
    dot = gjkVec3Dot(&v4->v, dir);
    return gjkIsZero(dot) || dot > GJK_ZERO;
}
