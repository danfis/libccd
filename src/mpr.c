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
 *  Returns 0 on success, 1 if origin (0,0,0) is contained in portal and -1
 *  if origin is outside of portal. */
static int discoverPortal(const void *obj1, const void *obj2,
                          const gjk_t *gjk, gjk_simplex_t *portal);


/** Expands portal towards origin and determine if objects intersect.
 *  Already established portal must be given as argument.
 *  If intersection is found 0 is returned, -1 otherwise */
static int refinePortal(const void *obj1, const void *obj2,
                        const gjk_t *gjk, gjk_simplex_t *portal);


/*
static void svtSimplex(const gjk_simplex_t *s, FILE *out)
{
    fprintf(out, "-----\n");
    fprintf(out, "Name: origin\n");
    fprintf(out, "Point color: 1 0 0\n");
    fprintf(out, "Points:\n0 0 0\n");
    fprintf(out, "-----\n");
    fprintf(out, "Point color: 0 1 0\n");
    fprintf(out, "Points:\n%lf %lf %lf\n",
                 gjkVec3X(&gjkSimplexPoint(s, 0)->v),
                 gjkVec3Y(&gjkSimplexPoint(s, 0)->v),
                 gjkVec3Z(&gjkSimplexPoint(s, 0)->v));
    fprintf(out, "-----\n");
    fprintf(out, "point color: 0 0 1\n");
    fprintf(out, "edge color: 0 0 1\n");
    fprintf(out, "face color: 0 0 1\n");
    fprintf(out, "Points:\n%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
                 gjkVec3X(&gjkSimplexPoint(s, 0)->v),
                 gjkVec3Y(&gjkSimplexPoint(s, 0)->v),
                 gjkVec3Z(&gjkSimplexPoint(s, 0)->v),
                 gjkVec3X(&gjkSimplexPoint(s, 1)->v),
                 gjkVec3Y(&gjkSimplexPoint(s, 1)->v),
                 gjkVec3Z(&gjkSimplexPoint(s, 1)->v),
                 gjkVec3X(&gjkSimplexPoint(s, 2)->v),
                 gjkVec3Y(&gjkSimplexPoint(s, 2)->v),
                 gjkVec3Z(&gjkSimplexPoint(s, 2)->v),
                 gjkVec3X(&gjkSimplexPoint(s, 3)->v),
                 gjkVec3Y(&gjkSimplexPoint(s, 3)->v),
                 gjkVec3Z(&gjkSimplexPoint(s, 3)->v));
    fprintf(out, "Edges:\n0 1 0 2 0 3 1 2 2 3 3 1\n");
    fprintf(out, "Faces:\n1 2 3\n");
    fprintf(out, "-----\n");
}
*/

int gjkMPRIntersect(const void *obj1, const void *obj2, const gjk_t *gjk)
{
    gjk_simplex_t portal;
    int res;

    // Phase 1: Portal discovery - find portal that intersects with origin
    // ray (ray from center of Minkowski diff to origin of coordinates)
    res = discoverPortal(obj1, obj2, gjk, &portal);
    if (res != 0){
        return (res == 1 ? 1 : 0);
    }

    // TODO: For penetration we need two functions one for collision
    //       detection and second for obtaining penetration info which can
    //       be done by expanding portal in correct direction until
    //       tolerance is reached!

    // Phase 2: Portal refinement
    res = refinePortal(obj1, obj2, gjk, &portal);
    return (res == 0 ? 1 : 0);
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
    gjk_support_t supp;
    gjk_vec3_t dir, va, vb;
    gjk_real_t dot;
    int cont;

    findOrigin(obj1, obj2, gjk, &supp);
    if (gjkVec3Eq(&supp.v, gjk_vec3_origin))
        return 1;
    // TODO: In this case caller will determine penetration information
    //       from rays casted in directions around sphere


    // vertex 0 is center of portal
    gjkSimplexSet(portal, 0, &supp);

    // vertex 1 = support in direction of origin
    gjkVec3Copy(&dir, &supp.v);
    gjkVec3Scale(&dir, GJK_REAL(-1));
    gjkVec3Normalize(&dir);
    __gjkSupport(obj1, obj2, &dir, gjk, gjkSimplexPointW(portal, 1));

    // test if origin isn't outside of v1
    dot = gjkVec3Dot(&gjkSimplexPoint(portal, 1)->v, &dir);
    if (gjkIsZero(dot) || dot < GJK_ZERO)
        return -1;

    // vertex 2
    gjkVec3Cross(&dir, &gjkSimplexPoint(portal, 0)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    if (gjkVec3Eq(&dir, gjk_vec3_origin))
        return 1;
    // TODO: 1. If v1 == origin we have touching contact
    //       2. else we need to build portal to determine penetration info


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
        gjkVec3Scale(&dir, GJK_REAL(-1));
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
    gjk_vec3_t v2v1, v3v1, v4v3, v4v0;
    gjk_vec3_t dir;
    gjk_real_t dot, dot2;
    gjk_support_t v4;

    gjkVec3Sub2(&v2v1, &gjkSimplexPoint(portal, 2)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 3)->v,
                       &gjkSimplexPoint(portal, 1)->v);
    while (1){
        // compute direction outside the portal (from v0 throught v1,v2,v3
        // face)
        gjkVec3Cross(&dir, &v2v1, &v3v1);

        // test if origin is inside the portal
        dot = gjkVec3Dot(&dir, &gjkSimplexPoint(portal, 1)->v);
        if (gjkIsZero(dot) || dot > GJK_ZERO)
            return 0;

        // get next support point
        __gjkSupport(obj1, obj2, &dir, gjk, &v4);

        gjkVec3Normalize(&dir);
        dot = GJK_REAL(-1) * gjkVec3Dot(&v4.v, &dir);
        gjkVec3Sub2(&v4v3, &v4.v, &gjkSimplexPoint(portal, 3)->v);
        dot2 = gjkVec3Dot(&v4v3, &dir);
        if (gjkIsZero(dot) || dot > GJK_ZERO
                || gjkEq(dot2, gjk->mpr_tolerance)
                || dot2 < gjk->mpr_tolerance){
            return -1;
        }

        gjkVec3Cross(&v4v0, &v4.v, &gjkSimplexPoint(portal, 0)->v);
        dot = gjkVec3Dot(&gjkSimplexPoint(portal, 1)->v, &v4v0);
        if (dot > GJK_ZERO){
            dot = gjkVec3Dot(&gjkSimplexPoint(portal, 2)->v, &v4v0);
            if (dot > GJK_ZERO){
                gjkSimplexSet(portal, 1, &v4);
                gjkVec3Sub2(&v2v1, &gjkSimplexPoint(portal, 2)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
                gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 3)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
            }else{
                gjkSimplexSet(portal, 3, &v4);
                gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 3)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
            }
        }else{
            dot = gjkVec3Dot(&gjkSimplexPoint(portal, 3)->v, &v4v0);
            if (dot > GJK_ZERO){
                gjkSimplexSet(portal, 2, &v4);
                gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 2)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
            }else{
                gjkSimplexSet(portal, 1, &v4);
                gjkVec3Sub2(&v2v1, &gjkSimplexPoint(portal, 2)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
                gjkVec3Sub2(&v3v1, &gjkSimplexPoint(portal, 3)->v,
                                   &gjkSimplexPoint(portal, 1)->v);
            }
        }
    }


    return -1;
}
