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

#include <stdio.h>
#include <float.h>
#include "gjk.h"
#include "vec3.h"
#include "simplex.h"
#include "polytope.h"
#include "alloc.h"
#include "dbg.h"


/** Performs GJK algorithm. Returns 0 if intersection was found and simplex
 *  is filled with resulting polytope. */
static int __gjkGJK(const void *obj1, const void *obj2,
                    const gjk_t *gjk, gjk_simplex_t *simplex);

/** Performs GJK+EPA algorithm. Returns 0 if intersection was found and
 *  pt is filled with resulting polytope and nearest with pointer to
 *  nearest element (vertex, edge, face) of polytope to origin. */
static int __gjkGJKEPA(const void *obj1, const void *obj2,
                       const gjk_t *gjk,
                       gjk_pt_t *pt, gjk_pt_el_t **nearest);


/** Returns true if simplex contains origin.
 *  This function also alteres simplex and dir according to further
 *  processing of GJK algorithm. */
static int doSimplex(gjk_simplex_t *simplex, gjk_vec3_t *dir);
static int doSimplex2(gjk_simplex_t *simplex, gjk_vec3_t *dir);
static int doSimplex3(gjk_simplex_t *simplex, gjk_vec3_t *dir);
static int doSimplex4(gjk_simplex_t *simplex, gjk_vec3_t *dir);

/** d = a x b x c */
_gjk_inline void tripleCross(const gjk_vec3_t *a, const gjk_vec3_t *b,
                             const gjk_vec3_t *c, gjk_vec3_t *d);


/** Transforms simplex to polytope. It is assumed that simplex has 4
 *  vertices. */
static int simplexToPolytope4(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest);

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest);

/** Transforms simplex to polytope, two vertices required */
static int simplexToPolytope2(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest);

/** Expands polytope using new vertex v. */
static void expandPolytope(gjk_pt_t *pt, gjk_pt_el_t *el,
                           const gjk_support_t *newv);

/** Finds next support point (at stores it in out argument).
 *  Returns 0 on success, -1 otherwise */
static int nextSupport(const void *obj1, const void *obj2, const gjk_t *gjk,
                       const gjk_pt_el_t *el,
                       gjk_support_t *out);



void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir)
{
    gjkVec3Set(dir, GJK_ONE, GJK_ZERO, GJK_ZERO);
}

int gjkIntersect(const void *obj1, const void *obj2, const gjk_t *gjk)
{
    gjk_simplex_t simplex;
    return __gjkGJK(obj1, obj2, gjk, &simplex) == 0;
}

int gjkSeparateEPA(const void *obj1, const void *obj2, const gjk_t *gjk,
                   gjk_vec3_t *sep)
{
    gjk_pt_t polytope;
    gjk_pt_el_t *nearest;
    int ret;

    gjkPtInit(&polytope);

    ret = __gjkGJKEPA(obj1, obj2, gjk, &polytope, &nearest);

    // set separation vector
    if (nearest)
        gjkVec3Copy(sep, &nearest->witness);

    gjkPtDestroy(&polytope);

    return ret;
}


static int penEPAPosCmp(const void *a, const void *b)
{
    gjk_pt_vertex_t *v1, *v2;
    v1 = *(gjk_pt_vertex_t **)a;
    v2 = *(gjk_pt_vertex_t **)b;

    if (gjkEq(v1->dist, v2->dist)){
        return 0;
    }else if (v1->dist < v2->dist){
        return -1;
    }else{
        return 1;
    }
}

static void penEPAPos(const gjk_pt_t *pt, const gjk_pt_el_t *nearest,
                      gjk_vec3_t *pos)
{
    gjk_pt_vertex_t *v;
    gjk_pt_vertex_t **vs;
    size_t i, len;
    gjk_real_t scale;

    // compute median
    len = 0;
    gjkListForEachEntry(&pt->vertices, v, list){
        len++;
    }

    vs = GJK_ALLOC_ARR(gjk_pt_vertex_t *, len);
    i = 0;
    gjkListForEachEntry(&pt->vertices, v, list){
        vs[i++] = v;
    }

    qsort(vs, len, sizeof(gjk_pt_vertex_t *), penEPAPosCmp);

    gjkVec3Set(pos, GJK_ZERO, GJK_ZERO, GJK_ZERO);
    scale = GJK_ZERO;
    if (len % 2 == 1)
        len++;

    for (i = 0; i < len / 2; i++){
        gjkVec3Add(pos, &vs[i]->v.v1);
        gjkVec3Add(pos, &vs[i]->v.v2);
        scale += GJK_REAL(2.);
    }
    gjkVec3Scale(pos, GJK_ONE / scale);

    free(vs);
}

int gjkPenetrationEPA(const void *obj1, const void *obj2, const gjk_t *gjk,
                      gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos)
{
    gjk_pt_t polytope;
    gjk_pt_el_t *nearest;
    int ret;

    gjkPtInit(&polytope);

    ret = __gjkGJKEPA(obj1, obj2, gjk, &polytope, &nearest);

    // set separation vector
    if (ret == 0 && nearest){
        // compute depth of penetration
        *depth = GJK_SQRT(nearest->dist);

        // store normalized direction vector
        gjkVec3Copy(dir, &nearest->witness);
        gjkVec3Normalize(dir);

        // compute position
        penEPAPos(&polytope, nearest, pos);
    }

    gjkPtDestroy(&polytope);

    return ret;
}




static int __gjkGJK(const void *obj1, const void *obj2,
                    const gjk_t *gjk, gjk_simplex_t *simplex)
{
    unsigned long iterations;
    gjk_vec3_t dir; // direction vector
    gjk_support_t last; // last support point
    int do_simplex_res;

    // initialize simplex struct
    gjkSimplexInit(simplex);

    // get first direction
    gjk->first_dir(obj1, obj2, &dir);
    // get first support point
    __gjkSupport(obj1, obj2, &dir, gjk, &last);
    // and add this point to simplex as last one
    gjkSimplexAdd(simplex, &last);

    // set up direction vector to as (O - last) which is exactly -last
    gjkVec3Copy(&dir, &last.v);
    gjkVec3Scale(&dir, -GJK_ONE);

    // start iterations
    for (iterations = 0UL; iterations < gjk->max_iterations; ++iterations) {
        // obtain support point
        __gjkSupport(obj1, obj2, &dir, gjk, &last);

        // check if farthest point in Minkowski difference in direction dir
        // isn't somewhere before origin (the test on negative dot product)
        // - because if it is, objects are not intersecting at all.
        if (gjkVec3Dot(&last.v, &dir) < GJK_ZERO){
            return -1; // intersection not found
        }

        // add last support vector to simplex
        gjkSimplexAdd(simplex, &last);

        // if doSimplex returns 1 if objects intersect, -1 if objects don't
        // intersect and 0 if algorithm should continue
        do_simplex_res = doSimplex(simplex, &dir);
        if (do_simplex_res == 1){
            return 0; // intersection found
        }else if (do_simplex_res == -1){
            return -1; // intersection not found
        }

        if (gjkIsZero(gjkVec3Len2(&dir))){
            return -1; // intersection not found
        }
    }

    // intersection wasn't found
    return -1;
}

static int __gjkGJKEPA(const void *obj1, const void *obj2,
                       const gjk_t *gjk,
                       gjk_pt_t *polytope, gjk_pt_el_t **nearest)
{
    gjk_simplex_t simplex;
    gjk_support_t supp; // support point
    int ret, size;

    *nearest = NULL;

    // run GJK and obtain terminal simplex
    ret = __gjkGJK(obj1, obj2, gjk, &simplex);
    if (ret != 0)
        return -1;

    // transform simplex to polytope - simplex won't be used anymore
    size = gjkSimplexSize(&simplex);
    if (size == 4){
        if (simplexToPolytope4(obj1, obj2, gjk, &simplex, polytope, nearest) != 0){
            return 0;// touch contact
        }
    }else if (size == 3){
        if (simplexToPolytope3(obj1, obj2, gjk, &simplex, polytope, nearest) != 0){
            return 0; // touch contact
        }
    }else{ // size == 2
        if (simplexToPolytope2(obj1, obj2, gjk, &simplex, polytope, nearest) != 0){
            return 0; // touch contact
        }
    }

    while (1){
        // get triangle nearest to origin
        *nearest = gjkPtNearest(polytope);

        // get next support point
        if (nextSupport(obj1, obj2, gjk, *nearest, &supp) != 0)
            break;

        // expand nearest triangle using new point - supp
        expandPolytope(polytope, *nearest, &supp);
    }

    return 0;
}



static int doSimplex2(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_support_t *A, *B;
    gjk_vec3_t AB, AO, tmp;
    gjk_real_t dot;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other point
    B = gjkSimplexPoint(simplex, 0);
    // compute AB oriented segment
    gjkVec3Sub2(&AB, &B->v, &A->v);
    // compute AO vector
    gjkVec3Copy(&AO, &A->v);
    gjkVec3Scale(&AO, -GJK_ONE);

    // dot product AB . AO
    dot = gjkVec3Dot(&AB, &AO);

    // check if origin doesn't lie on AB segment
    gjkVec3Cross(&tmp, &AB, &AO);
    if (gjkIsZero(gjkVec3Len2(&tmp)) && dot > GJK_ZERO){
        return 1;
    }

    // check if origin is in area where AB segment is
    if (gjkIsZero(dot) || dot < GJK_ZERO){
        // origin is in outside are of A
        gjkSimplexSet(simplex, 0, A);
        gjkSimplexSetSize(simplex, 1);
        gjkVec3Copy(dir, &AO);
    }else{
        // origin is in area where AB segment is

        // keep simplex untouched and set direction to
        // AB x AO x AB
        tripleCross(&AB, &AO, &AB, dir);
    }

    return 0;
}

static int doSimplex3(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_support_t *A, *B, *C;
    gjk_vec3_t AO, AB, AC, ABC, tmp;
    gjk_real_t dot, dist;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other points
    B = gjkSimplexPoint(simplex, 1);
    C = gjkSimplexPoint(simplex, 0);

    // check touching contact
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A->v, &B->v, &C->v, NULL);
    if (gjkIsZero(dist)){
        return 1;
    }

    // check if triangle is really triangle (has area > 0)
    // if not simplex can't be expanded and thus no itersection is found
    if (gjkVec3Eq(&A->v, &B->v) || gjkVec3Eq(&A->v, &C->v)){
        return -1;
    }

    // compute AO vector
    gjkVec3Copy(&AO, &A->v);
    gjkVec3Scale(&AO, -GJK_ONE);

    // compute AB and AC segments and ABC vector (perpendircular to triangle)
    gjkVec3Sub2(&AB, &B->v, &A->v);
    gjkVec3Sub2(&AC, &C->v, &A->v);
    gjkVec3Cross(&ABC, &AB, &AC);

    gjkVec3Cross(&tmp, &ABC, &AC);
    dot = gjkVec3Dot(&tmp, &AO);
    if (gjkIsZero(dot) || dot > GJK_ZERO){
        dot = gjkVec3Dot(&AC, &AO);
        if (gjkIsZero(dot) || dot > GJK_ZERO){
            // C is already in place
            gjkSimplexSet(simplex, 1, A);
            gjkSimplexSetSize(simplex, 2);
            tripleCross(&AC, &AO, &AC, dir);
        }else{
gjk_do_simplex3_45:
            dot = gjkVec3Dot(&AB, &AO);
            if (gjkIsZero(dot) || dot > GJK_ZERO){
                gjkSimplexSet(simplex, 0, B);
                gjkSimplexSet(simplex, 1, A);
                gjkSimplexSetSize(simplex, 2);
                tripleCross(&AB, &AO, &AB, dir);
            }else{
                gjkSimplexSet(simplex, 0, A);
                gjkSimplexSetSize(simplex, 1);
                gjkVec3Copy(dir, &AO);
            }
        }
    }else{
        gjkVec3Cross(&tmp, &AB, &ABC);
        dot = gjkVec3Dot(&tmp, &AO);
        if (gjkIsZero(dot) || dot > GJK_ZERO){
            goto gjk_do_simplex3_45;
        }else{
            dot = gjkVec3Dot(&ABC, &AO);
            if (gjkIsZero(dot) || dot > GJK_ZERO){
                gjkVec3Copy(dir, &ABC);
            }else{
                gjk_support_t Ctmp;
                gjkSupportCopy(&Ctmp, C);
                gjkSimplexSet(simplex, 0, B);
                gjkSimplexSet(simplex, 1, &Ctmp);

                gjkVec3Copy(dir, &ABC);
                gjkVec3Scale(dir, -GJK_ONE);
            }
        }
    }

    return 0;
}

static int doSimplex4(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_support_t *A, *B, *C, *D;
    gjk_vec3_t AO, AB, AC, AD, ABC, ACD, ADB;
    int B_on_ACD, C_on_ADB, D_on_ABC;
    int AB_O, AC_O, AD_O;
    gjk_real_t dist;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other points
    B = gjkSimplexPoint(simplex, 2);
    C = gjkSimplexPoint(simplex, 1);
    D = gjkSimplexPoint(simplex, 0);

    // check if tetrahedron is really tetrahedron (has volume > 0)
    // if it is not simplex can't be expanded and thus no intersection is
    // found
    dist = gjkVec3PointTriDist2(&A->v, &B->v, &C->v, &D->v, NULL);
    if (gjkIsZero(dist)){
        return -1;
    }

    // check if origin lies on some of tetrahedron's face - if so objects
    // intersect
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A->v, &B->v, &C->v, NULL);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A->v, &C->v, &D->v, NULL);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A->v, &B->v, &D->v, NULL);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &B->v, &C->v, &D->v, NULL);
    if (gjkIsZero(dist))
        return 1;

    // compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
    gjkVec3Copy(&AO, &A->v);
    gjkVec3Scale(&AO, -GJK_ONE);
    gjkVec3Sub2(&AB, &B->v, &A->v);
    gjkVec3Sub2(&AC, &C->v, &A->v);
    gjkVec3Sub2(&AD, &D->v, &A->v);
    gjkVec3Cross(&ABC, &AB, &AC);
    gjkVec3Cross(&ACD, &AC, &AD);
    gjkVec3Cross(&ADB, &AD, &AB);

    // side (positive or negative) of B, C, D relative to planes ACD, ADB
    // and ABC respectively
    B_on_ACD = gjkSign(gjkVec3Dot(&ACD, &AB));
    C_on_ADB = gjkSign(gjkVec3Dot(&ADB, &AC));
    D_on_ABC = gjkSign(gjkVec3Dot(&ABC, &AD));

    // whether origin is on same side of ACD, ADB, ABC as B, C, D
    // respectively
    AB_O = gjkSign(gjkVec3Dot(&ACD, &AO)) == B_on_ACD;
    AC_O = gjkSign(gjkVec3Dot(&ADB, &AO)) == C_on_ADB;
    AD_O = gjkSign(gjkVec3Dot(&ABC, &AO)) == D_on_ABC;

    if (AB_O && AC_O && AD_O){
        // origin is in tetrahedron
        return 1;

    // rearrange simplex to triangle and call doSimplex3()
    }else if (!AB_O){
        // B is farthest from the origin among all of the tetrahedron's
        // points, so remove it from the list and go on with the triangle
        // case

        // D and C are in place
        gjkSimplexSet(simplex, 2, A);
        gjkSimplexSetSize(simplex, 3);
    }else if (!AC_O){
        // C is farthest
        gjkSimplexSet(simplex, 1, D);
        gjkSimplexSet(simplex, 0, B);
        gjkSimplexSet(simplex, 2, A);
        gjkSimplexSetSize(simplex, 3);
    }else{ // (!AD_O)
        gjkSimplexSet(simplex, 0, C);
        gjkSimplexSet(simplex, 1, B);
        gjkSimplexSet(simplex, 2, A);
        gjkSimplexSetSize(simplex, 3);
    }

    return doSimplex3(simplex, dir);
}

static int doSimplex(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    if (gjkSimplexSize(simplex) == 2){
        // simplex contains segment only one segment
        return doSimplex2(simplex, dir);
    }else if (gjkSimplexSize(simplex) == 3){
        // simplex contains triangle
        return doSimplex3(simplex, dir);
    }else{ // gjkSimplexSize(simplex) == 4
        // tetrahedron - this is the only shape which can encapsule origin
        // so doSimplex4() also contains test on it
        return doSimplex4(simplex, dir);
    }

    return 0;
}

_gjk_inline void tripleCross(const gjk_vec3_t *a, const gjk_vec3_t *b,
                             const gjk_vec3_t *c, gjk_vec3_t *d)
{
    gjk_vec3_t e;
    gjkVec3Cross(&e, a, b);
    gjkVec3Cross(d, &e, c);
}



/** Transforms simplex to polytope. It is assumed that simplex has 4
 *  vertices! */
static int simplexToPolytope4(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest)
{
    const gjk_support_t *a, *b, *c, *d;
    int use_polytope3;
    gjk_real_t dist;
    gjk_pt_vertex_t *v[4];
    gjk_pt_edge_t *e[6];
    size_t i;

    a = gjkSimplexPoint(simplex, 0);
    b = gjkSimplexPoint(simplex, 1);
    c = gjkSimplexPoint(simplex, 2);
    d = gjkSimplexPoint(simplex, 3);

    // check if origin lies on some of tetrahedron's face - if so use
    // simplexToPolytope3()
    use_polytope3 = 0;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &a->v, &b->v, &c->v, NULL);
    if (gjkIsZero(dist)){
        use_polytope3 = 1;
    }
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &a->v, &c->v, &d->v, NULL);
    if (gjkIsZero(dist)){
        use_polytope3 = 1;
        gjkSimplexSet(simplex, 1, c);
        gjkSimplexSet(simplex, 2, d);
    }
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &a->v, &b->v, &d->v, NULL);
    if (gjkIsZero(dist)){
        use_polytope3 = 1;
        gjkSimplexSet(simplex, 2, d);
    }
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &b->v, &c->v, &d->v, NULL);
    if (gjkIsZero(dist)){
        use_polytope3 = 1;
        gjkSimplexSet(simplex, 0, b);
        gjkSimplexSet(simplex, 1, c);
        gjkSimplexSet(simplex, 2, d);
    }

    if (use_polytope3){
        gjkSimplexSetSize(simplex, 3);
        return simplexToPolytope3(obj1, obj2, gjk, simplex, pt, nearest);
    }

    // no touching contact - simply create tetrahedron
    for (i = 0; i < 4; i++){
        v[i] = gjkPtAddVertex(pt, gjkSimplexPoint(simplex, i));
    }
    
    e[0] = gjkPtAddEdge(pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(pt, v[2], v[0]);
    e[3] = gjkPtAddEdge(pt, v[3], v[0]);
    e[4] = gjkPtAddEdge(pt, v[3], v[1]);
    e[5] = gjkPtAddEdge(pt, v[3], v[2]);

    gjkPtAddFace(pt, e[0], e[1], e[2]);
    gjkPtAddFace(pt, e[3], e[4], e[0]);
    gjkPtAddFace(pt, e[4], e[5], e[1]);
    gjkPtAddFace(pt, e[5], e[3], e[2]);

    return 0;
}

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest)
{
    const gjk_support_t *a, *b, *c;
    gjk_support_t d, d2;
    gjk_vec3_t ab, ac, dir;
    gjk_pt_vertex_t *v[5];
    gjk_pt_edge_t *e[9];
    gjk_real_t dist, dist2;

    *nearest = NULL;

    a = gjkSimplexPoint(simplex, 0);
    b = gjkSimplexPoint(simplex, 1);
    c = gjkSimplexPoint(simplex, 2);

    // If only one triangle left from previous GJK run origin lies on this
    // triangle. So it is necessary to expand triangle into two
    // tetrahedrons connected with base (which is exactly abc triangle).

    // get next support point in direction of normal of triangle
    gjkVec3Sub2(&ab, &b->v, &a->v);
    gjkVec3Sub2(&ac, &c->v, &a->v);
    gjkVec3Cross(&dir, &ab, &ac);
    __gjkSupport(obj1, obj2, &dir, gjk, &d);
    dist = gjkVec3PointTriDist2(&d.v, &a->v, &b->v, &c->v, NULL);

    // and second one take in opposite direction
    gjkVec3Scale(&dir, -GJK_ONE);
    __gjkSupport(obj1, obj2, &dir, gjk, &d2);
    dist2 = gjkVec3PointTriDist2(&d2.v, &a->v, &b->v, &c->v, NULL);

    // check if face isn't already on edge of minkowski sum and thus we
    // have touching contact
    if (gjkIsZero(dist) || gjkIsZero(dist2)){
        v[0] = gjkPtAddVertex(pt, a);
        v[1] = gjkPtAddVertex(pt, b);
        v[2] = gjkPtAddVertex(pt, c);
        e[0] = gjkPtAddEdge(pt, v[0], v[1]);
        e[1] = gjkPtAddEdge(pt, v[1], v[2]);
        e[2] = gjkPtAddEdge(pt, v[2], v[0]);
        *nearest = (gjk_pt_el_t *)gjkPtAddFace(pt, e[0], e[1], e[2]);

        return -1;
    }

    // form polyhedron
    v[0] = gjkPtAddVertex(pt, a);
    v[1] = gjkPtAddVertex(pt, b);
    v[2] = gjkPtAddVertex(pt, c);
    v[3] = gjkPtAddVertex(pt, &d);
    v[4] = gjkPtAddVertex(pt, &d2);

    e[0] = gjkPtAddEdge(pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(pt, v[2], v[0]);

    e[3] = gjkPtAddEdge(pt, v[3], v[0]);
    e[4] = gjkPtAddEdge(pt, v[3], v[1]);
    e[5] = gjkPtAddEdge(pt, v[3], v[2]);

    e[6] = gjkPtAddEdge(pt, v[4], v[0]);
    e[7] = gjkPtAddEdge(pt, v[4], v[1]);
    e[8] = gjkPtAddEdge(pt, v[4], v[2]);

    gjkPtAddFace(pt, e[3], e[4], e[0]);
    gjkPtAddFace(pt, e[4], e[5], e[1]);
    gjkPtAddFace(pt, e[5], e[3], e[2]);

    gjkPtAddFace(pt, e[6], e[7], e[0]);
    gjkPtAddFace(pt, e[7], e[8], e[1]);
    gjkPtAddFace(pt, e[8], e[6], e[2]);

    return 0;
}

/** Transforms simplex to polytope, two vertices required */
static int simplexToPolytope2(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex,
                              gjk_pt_t *pt, gjk_pt_el_t **nearest)
{
    const gjk_support_t *a, *b;
    gjk_vec3_t ab, ac, dir;
    gjk_support_t supp[4];
    gjk_pt_vertex_t *v[6];
    gjk_pt_edge_t *e[12];
    size_t i;
    int found;

    a = gjkSimplexPoint(simplex, 0);
    b = gjkSimplexPoint(simplex, 1);

    // This situation is a bit tricky. If only one segment comes from
    // previous run of GJK - it means that either this segment is on
    // minkowski edge (and thus we have touch contact) or it it isn't and
    // therefore segment is somewhere *inside* minkowski sum and it *must*
    // be possible to fully enclose this segment with polyhedron formed by
    // at least 8 triangle faces.

    // get first support point (any)
    found = 0;
    for (i = 0; i < gjk_points_on_sphere_len; i++){
        __gjkSupport(obj1, obj2, &gjk_points_on_sphere[i], gjk, &supp[0]);
        if (!gjkVec3Eq(&a->v, &supp[0].v) && !gjkVec3Eq(&b->v, &supp[0].v)){
            found = 1;
            break;
        }
    }
    if (!found)
        goto simplexToPolytope2_touching_contact;

    // get second support point in opposite direction than supp[0]
    gjkVec3Copy(&dir, &supp[0].v);
    gjkVec3Scale(&dir, -GJK_ONE);
    __gjkSupport(obj1, obj2, &dir, gjk, &supp[1]);
    if (gjkVec3Eq(&a->v, &supp[1].v) || gjkVec3Eq(&b->v, &supp[1].v))
        goto simplexToPolytope2_touching_contact;

    // next will be in direction of normal of triangle a,supp[0],supp[1]
    gjkVec3Sub2(&ab, &supp[0].v, &a->v);
    gjkVec3Sub2(&ac, &supp[1].v, &a->v);
    gjkVec3Cross(&dir, &ab, &ac);
    __gjkSupport(obj1, obj2, &dir, gjk, &supp[2]);
    if (gjkVec3Eq(&a->v, &supp[2].v) || gjkVec3Eq(&b->v, &supp[2].v))
        goto simplexToPolytope2_touching_contact;

    // and last one will be in opposite direction
    gjkVec3Scale(&dir, -GJK_ONE);
    __gjkSupport(obj1, obj2, &dir, gjk, &supp[3]);
    if (gjkVec3Eq(&a->v, &supp[3].v) || gjkVec3Eq(&b->v, &supp[3].v))
        goto simplexToPolytope2_touching_contact;

    goto simplexToPolytope2_not_touching_contact;
simplexToPolytope2_touching_contact:
    v[0] = gjkPtAddVertex(pt, a);
    v[1] = gjkPtAddVertex(pt, b);
    *nearest = (gjk_pt_el_t *)gjkPtAddEdge(pt, v[0], v[1]);
    return -1;

simplexToPolytope2_not_touching_contact:
    // form polyhedron
    v[0] = gjkPtAddVertex(pt, a);
    v[1] = gjkPtAddVertex(pt, &supp[0]);
    v[2] = gjkPtAddVertex(pt, b);
    v[3] = gjkPtAddVertex(pt, &supp[1]);
    v[4] = gjkPtAddVertex(pt, &supp[2]);
    v[5] = gjkPtAddVertex(pt, &supp[3]);

    e[0] = gjkPtAddEdge(pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(pt, v[2], v[3]);
    e[3] = gjkPtAddEdge(pt, v[3], v[0]);

    e[4] = gjkPtAddEdge(pt, v[4], v[0]);
    e[5] = gjkPtAddEdge(pt, v[4], v[1]);
    e[6] = gjkPtAddEdge(pt, v[4], v[2]);
    e[7] = gjkPtAddEdge(pt, v[4], v[3]);

    e[8]  = gjkPtAddEdge(pt, v[5], v[0]);
    e[9]  = gjkPtAddEdge(pt, v[5], v[1]);
    e[10] = gjkPtAddEdge(pt, v[5], v[2]);
    e[11] = gjkPtAddEdge(pt, v[5], v[3]);

    gjkPtAddFace(pt, e[4], e[5], e[0]);
    gjkPtAddFace(pt, e[5], e[6], e[1]);
    gjkPtAddFace(pt, e[6], e[7], e[2]);
    gjkPtAddFace(pt, e[7], e[4], e[3]);

    gjkPtAddFace(pt, e[8],  e[9],  e[0]);
    gjkPtAddFace(pt, e[9],  e[10], e[1]);
    gjkPtAddFace(pt, e[10], e[11], e[2]);
    gjkPtAddFace(pt, e[11], e[8],  e[3]);

    return 0;
}

/** Expands polytope's tri by new vertex v. Triangle tri is replaced by
 *  three triangles each with one vertex in v. */
static void expandPolytope(gjk_pt_t *pt, gjk_pt_el_t *el,
                           const gjk_support_t *newv)
{
    gjk_pt_vertex_t *v[5];
    gjk_pt_edge_t *e[8];
    gjk_pt_face_t *f[2];


    // element can be either segment or triangle
    if (el->type == GJK_PT_EDGE){
        // In this case, segment should be replaced by new point.
        // Simpliest case is when segment stands alone and in this case
        // this segment is replaced by two other segments both connected to
        // newv.
        // Segment can be also connected to max two faces and in that case
        // each face must be replaced by two other faces. To do this
        // correctly it is necessary to have correctly ordered edges and
        // vertices which is exactly what is done in following code.
        //

        gjkPtEdgeVertices((const gjk_pt_edge_t *)el, &v[0], &v[2]);

        gjkPtEdgeFaces((gjk_pt_edge_t *)el, &f[0], &f[1]);

        if (f[0]){
            gjkPtFaceEdges(f[0], &e[0], &e[1], &e[2]);
            if (e[0] == (gjk_pt_edge_t *)el){
                e[0] = e[2];
            }else if (e[1] == (gjk_pt_edge_t *)el){
                e[1] = e[2];
            }
            gjkPtEdgeVertices(e[0], &v[1], &v[3]);
            if (v[1] != v[0] && v[3] != v[0]){
                e[2] = e[0];
                e[0] = e[1];
                e[1] = e[2];
                if (v[1] == v[2])
                    v[1] = v[3];
            }else{
                if (v[1] == v[0])
                    v[1] = v[3];
            }

            if (f[1]){
                gjkPtFaceEdges(f[1], &e[2], &e[3], &e[4]);
                if (e[2] == (gjk_pt_edge_t *)el){
                    e[2] = e[4];
                }else if (e[3] == (gjk_pt_edge_t *)el){
                    e[3] = e[4];
                }
                gjkPtEdgeVertices(e[2], &v[3], &v[4]);
                if (v[3] != v[2] && v[4] != v[2]){
                    e[4] = e[2];
                    e[2] = e[3];
                    e[3] = e[4];
                    if (v[3] == v[0])
                        v[3] = v[4];
                }else{
                    if (v[3] == v[2])
                        v[3] = v[4];
                }
            }


            v[4] = gjkPtAddVertex(pt, newv);

            gjkPtDelFace(pt, f[0]);
            if (f[1]){
                gjkPtDelFace(pt, f[1]);
                gjkPtDelEdge(pt, (gjk_pt_edge_t *)el);
            }

            e[4] = gjkPtAddEdge(pt, v[4], v[2]);
            e[5] = gjkPtAddEdge(pt, v[4], v[0]);
            e[6] = gjkPtAddEdge(pt, v[4], v[1]);
            if (f[1])
                e[7] = gjkPtAddEdge(pt, v[4], v[3]);

            gjkPtAddFace(pt, e[1], e[4], e[6]);
            gjkPtAddFace(pt, e[0], e[6], e[5]);
            if (f[1]){
                gjkPtAddFace(pt, e[3], e[5], e[7]);
                gjkPtAddFace(pt, e[4], e[7], e[2]);
            }else{
                gjkPtAddFace(pt, e[4], e[5], (gjk_pt_edge_t *)el);
            }
        }
    }else{ // el->type == GJK_PT_FACE
        // replace triangle by tetrahedron without base (base would be the
        // triangle that will be removed)

        // get triplet of surrounding edges and vertices of triangle face
        gjkPtFaceEdges((const gjk_pt_face_t *)el, &e[0], &e[1], &e[2]);
        gjkPtEdgeVertices(e[0], &v[0], &v[1]);
        gjkPtEdgeVertices(e[1], &v[2], &v[3]);

        // following code sorts edges to have e[0] between vertices 0-1,
        // e[1] between 1-2 and e[2] between 2-0
        if (v[2] != v[1] && v[3] != v[1]){
            // swap e[1] and e[2] 
            e[3] = e[1];
            e[1] = e[2];
            e[2] = e[3];
        }
        if (v[3] != v[0] && v[3] != v[1])
            v[2] = v[3];

        // remove triangle face
        gjkPtDelFace(pt, (gjk_pt_face_t *)el);

        // expand triangle to tetrahedron
        v[3] = gjkPtAddVertex(pt, newv);
        e[3] = gjkPtAddEdge(pt, v[3], v[0]);
        e[4] = gjkPtAddEdge(pt, v[3], v[1]);
        e[5] = gjkPtAddEdge(pt, v[3], v[2]);

        gjkPtAddFace(pt, e[3], e[4], e[0]);
        gjkPtAddFace(pt, e[4], e[5], e[1]);
        gjkPtAddFace(pt, e[5], e[3], e[2]);
    }
}

/** Finds next support point (at stores it in out argument).
 *  Returns 0 on success, -1 otherwise */
static int nextSupport(const void *obj1, const void *obj2, const gjk_t *gjk,
                       const gjk_pt_el_t *el,
                       gjk_support_t *out)
{
    gjk_vec3_t *a, *b, *c;
    gjk_real_t dist;

    if (el->type == GJK_PT_VERTEX)
        return -1;

    // touch contact
    if (gjkIsZero(el->dist))
        return -1;

    __gjkSupport(obj1, obj2, &el->witness, gjk, out);

    if (el->type == GJK_PT_EDGE){
        // fetch end points of edge
        gjkPtEdgeVec3((gjk_pt_edge_t *)el, &a, &b);

        // get distance from segment
        dist = gjkVec3PointSegmentDist2(&out->v, a, b, NULL);
    }else{ // el->type == GJK_PT_FACE
        // fetch vertices of triangle face
        gjkPtFaceVec3((gjk_pt_face_t *)el, &a, &b, &c);

        // check if new point can significantly expand polytope
        dist = gjkVec3PointTriDist2(&out->v, a, b, c, NULL);
    }

    if (dist < gjk->epa_tolerance)
        return -1;

    return 0;
}
