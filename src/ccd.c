/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2012 Daniel Fiser <danfis@danfis.cz>
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
#include <float.h>
#include <ccddbl/ccddbl.h>
#include <ccddbl/vec3.h>
#include <ccddbl/simplex.h>
#include "polytope.h"
#include "alloc.h"
#include "dbg.h"


/** Performs GJK algorithm. Returns 0 if intersection was found and simplex
 *  is filled with resulting polytope. */
static int __ccddblGJK(const void *obj1, const void *obj2,
                    const ccddbl_t *ccddbl, ccddbl_simplex_t *simplex);

/** Performs GJK+EPA algorithm. Returns 0 if intersection was found and
 *  pt is filled with resulting polytope and nearest with pointer to
 *  nearest element (vertex, edge, face) of polytope to origin. */
static int __ccddblGJKEPA(const void *obj1, const void *obj2,
                       const ccddbl_t *ccddbl,
                       ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest);


/** Returns true if simplex contains origin.
 *  This function also alteres simplex and dir according to further
 *  processing of GJK algorithm. */
static int doSimplex(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir);
static int doSimplex2(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir);
static int doSimplex3(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir);
static int doSimplex4(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir);

/** d = a x b x c */
_ccddbl_inline void tripleCross(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b,
                             const ccddbl_vec3_t *c, ccddbl_vec3_t *d);


/** Transforms simplex to polytope. It is assumed that simplex has 4
 *  vertices. */
static int simplexToPolytope4(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest);

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              const ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest);

/** Transforms simplex to polytope, two vertices required */
static int simplexToPolytope2(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              const ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest);

/** Expands polytope using new vertex v.
 *  Return 0 on success, -2 on memory allocation failure.*/
static int expandPolytope(ccddbl_pt_t *pt, ccddbl_pt_el_t *el,
                          const ccddbl_support_t *newv);

/** Finds next support point (at stores it in out argument).
 *  Returns 0 on success, -1 otherwise */
static int nextSupport(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                       const ccddbl_pt_el_t *el,
                       ccddbl_support_t *out);



void ccddblFirstDirDefault(const void *o1, const void *o2, ccddbl_vec3_t *dir)
{
    ccddblVec3Set(dir, CCDDBL_ONE, CCDDBL_ZERO, CCDDBL_ZERO);
}

int ccddblGJKIntersect(const void *obj1, const void *obj2, const ccddbl_t *ccddbl)
{
    ccddbl_simplex_t simplex;
    return __ccddblGJK(obj1, obj2, ccddbl, &simplex) == 0;
}

int ccddblGJKSeparate(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                   ccddbl_vec3_t *sep)
{
    ccddbl_pt_t polytope;
    ccddbl_pt_el_t *nearest;
    int ret;

    ccddblPtInit(&polytope);

    ret = __ccddblGJKEPA(obj1, obj2, ccddbl, &polytope, &nearest);

    // set separation vector
    if (nearest)
        ccddblVec3Copy(sep, &nearest->witness);

    ccddblPtDestroy(&polytope);

    return ret;
}


static int penEPAPosCmp(const void *a, const void *b)
{
    ccddbl_pt_vertex_t *v1, *v2;
    v1 = *(ccddbl_pt_vertex_t **)a;
    v2 = *(ccddbl_pt_vertex_t **)b;

    if (ccddblEq(v1->dist, v2->dist)){
        return 0;
    }else if (v1->dist < v2->dist){
        return -1;
    }else{
        return 1;
    }
}

static int penEPAPos(const ccddbl_pt_t *pt, const ccddbl_pt_el_t *nearest,
                     ccddbl_vec3_t *pos)
{
    ccddbl_pt_vertex_t *v;
    ccddbl_pt_vertex_t **vs;
    size_t i, len;
    ccddbl_real_t scale;

    // compute median
    len = 0;
    ccddblListForEachEntry(&pt->vertices, v, ccddbl_pt_vertex_t, list){
        len++;
    }

    vs = CCDDBL_ALLOC_ARR(ccddbl_pt_vertex_t *, len);
    if (vs == NULL)
        return -1;

    i = 0;
    ccddblListForEachEntry(&pt->vertices, v, ccddbl_pt_vertex_t, list){
        vs[i++] = v;
    }

    qsort(vs, len, sizeof(ccddbl_pt_vertex_t *), penEPAPosCmp);

    ccddblVec3Set(pos, CCDDBL_ZERO, CCDDBL_ZERO, CCDDBL_ZERO);
    scale = CCDDBL_ZERO;
    if (len % 2 == 1)
        len++;

    for (i = 0; i < len / 2; i++){
        ccddblVec3Add(pos, &vs[i]->v.v1);
        ccddblVec3Add(pos, &vs[i]->v.v2);
        scale += CCDDBL_REAL(2.);
    }
    ccddblVec3Scale(pos, CCDDBL_ONE / scale);

    free(vs);

    return 0;
}

int ccddblGJKPenetration(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                      ccddbl_real_t *depth, ccddbl_vec3_t *dir, ccddbl_vec3_t *pos)
{
    ccddbl_pt_t polytope;
    ccddbl_pt_el_t *nearest;
    int ret;

    ccddblPtInit(&polytope);

    ret = __ccddblGJKEPA(obj1, obj2, ccddbl, &polytope, &nearest);

    // set separation vector
    if (ret == 0 && nearest){
        // compute depth of penetration
        *depth = CCDDBL_SQRT(nearest->dist);

        // store normalized direction vector
        ccddblVec3Copy(dir, &nearest->witness);
        ccddblVec3Normalize(dir);

        // compute position
        if (penEPAPos(&polytope, nearest, pos) != 0){
            ccddblPtDestroy(&polytope);
            return -2;
        }
    }

    ccddblPtDestroy(&polytope);

    return ret;
}


static int __ccddblGJK(const void *obj1, const void *obj2,
                    const ccddbl_t *ccddbl, ccddbl_simplex_t *simplex)
{
    unsigned long iterations;
    ccddbl_vec3_t dir; // direction vector
    ccddbl_support_t last; // last support point
    int do_simplex_res;

    // initialize simplex struct
    ccddblSimplexInit(simplex);

    // get first direction
    ccddbl->first_dir(obj1, obj2, &dir);
    // get first support point
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &last);
    // and add this point to simplex as last one
    ccddblSimplexAdd(simplex, &last);

    // set up direction vector to as (O - last) which is exactly -last
    ccddblVec3Copy(&dir, &last.v);
    ccddblVec3Scale(&dir, -CCDDBL_ONE);

    // start iterations
    for (iterations = 0UL; iterations < ccddbl->max_iterations; ++iterations) {
        // obtain support point
        __ccddblSupport(obj1, obj2, &dir, ccddbl, &last);

        // check if farthest point in Minkowski difference in direction dir
        // isn't somewhere before origin (the test on negative dot product)
        // - because if it is, objects are not intersecting at all.
        if (ccddblVec3Dot(&last.v, &dir) < CCDDBL_ZERO){
            return -1; // intersection not found
        }

        // add last support vector to simplex
        ccddblSimplexAdd(simplex, &last);

        // if doSimplex returns 1 if objects intersect, -1 if objects don't
        // intersect and 0 if algorithm should continue
        do_simplex_res = doSimplex(simplex, &dir);
        if (do_simplex_res == 1){
            return 0; // intersection found
        }else if (do_simplex_res == -1){
            return -1; // intersection not found
        }

        if (ccddblIsZero(ccddblVec3Len2(&dir))){
            return -1; // intersection not found
        }
    }

    // intersection wasn't found
    return -1;
}

static int __ccddblGJKEPA(const void *obj1, const void *obj2,
                       const ccddbl_t *ccddbl,
                       ccddbl_pt_t *polytope, ccddbl_pt_el_t **nearest)
{
    ccddbl_simplex_t simplex;
    ccddbl_support_t supp; // support point
    int ret, size;

    *nearest = NULL;

    // run GJK and obtain terminal simplex
    ret = __ccddblGJK(obj1, obj2, ccddbl, &simplex);
    if (ret != 0)
        return -1;

    // transform simplex to polytope - simplex won't be used anymore
    size = ccddblSimplexSize(&simplex);
    if (size == 4){
        ret = simplexToPolytope4(obj1, obj2, ccddbl, &simplex, polytope, nearest);
    }else if (size == 3){
        ret = simplexToPolytope3(obj1, obj2, ccddbl, &simplex, polytope, nearest);
    }else{ // size == 2
        ret = simplexToPolytope2(obj1, obj2, ccddbl, &simplex, polytope, nearest);
    }

    if (ret == -1){
        // touching contact
        return 0;
    }else if (ret == -2){
        // failed memory allocation
        return -2;
    }


    while (1){
        // get triangle nearest to origin
        *nearest = ccddblPtNearest(polytope);

        // get next support point
        if (nextSupport(obj1, obj2, ccddbl, *nearest, &supp) != 0)
            break;

        // expand nearest triangle using new point - supp
        if (expandPolytope(polytope, *nearest, &supp) != 0)
            return -2;
    }

    return 0;
}



static int doSimplex2(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir)
{
    const ccddbl_support_t *A, *B;
    ccddbl_vec3_t AB, AO, tmp;
    ccddbl_real_t dot;

    // get last added as A
    A = ccddblSimplexLast(simplex);
    // get the other point
    B = ccddblSimplexPoint(simplex, 0);
    // compute AB oriented segment
    ccddblVec3Sub2(&AB, &B->v, &A->v);
    // compute AO vector
    ccddblVec3Copy(&AO, &A->v);
    ccddblVec3Scale(&AO, -CCDDBL_ONE);

    // dot product AB . AO
    dot = ccddblVec3Dot(&AB, &AO);

    // check if origin doesn't lie on AB segment
    ccddblVec3Cross(&tmp, &AB, &AO);
    if (ccddblIsZero(ccddblVec3Len2(&tmp)) && dot > CCDDBL_ZERO){
        return 1;
    }

    // check if origin is in area where AB segment is
    if (ccddblIsZero(dot) || dot < CCDDBL_ZERO){
        // origin is in outside are of A
        ccddblSimplexSet(simplex, 0, A);
        ccddblSimplexSetSize(simplex, 1);
        ccddblVec3Copy(dir, &AO);
    }else{
        // origin is in area where AB segment is

        // keep simplex untouched and set direction to
        // AB x AO x AB
        tripleCross(&AB, &AO, &AB, dir);
    }

    return 0;
}

static int doSimplex3(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir)
{
    const ccddbl_support_t *A, *B, *C;
    ccddbl_vec3_t AO, AB, AC, ABC, tmp;
    ccddbl_real_t dot, dist;

    // get last added as A
    A = ccddblSimplexLast(simplex);
    // get the other points
    B = ccddblSimplexPoint(simplex, 1);
    C = ccddblSimplexPoint(simplex, 0);

    // check touching contact
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &A->v, &B->v, &C->v, NULL);
    if (ccddblIsZero(dist)){
        return 1;
    }

    // check if triangle is really triangle (has area > 0)
    // if not simplex can't be expanded and thus no itersection is found
    if (ccddblVec3Eq(&A->v, &B->v) || ccddblVec3Eq(&A->v, &C->v)){
        return -1;
    }

    // compute AO vector
    ccddblVec3Copy(&AO, &A->v);
    ccddblVec3Scale(&AO, -CCDDBL_ONE);

    // compute AB and AC segments and ABC vector (perpendircular to triangle)
    ccddblVec3Sub2(&AB, &B->v, &A->v);
    ccddblVec3Sub2(&AC, &C->v, &A->v);
    ccddblVec3Cross(&ABC, &AB, &AC);

    ccddblVec3Cross(&tmp, &ABC, &AC);
    dot = ccddblVec3Dot(&tmp, &AO);
    if (ccddblIsZero(dot) || dot > CCDDBL_ZERO){
        dot = ccddblVec3Dot(&AC, &AO);
        if (ccddblIsZero(dot) || dot > CCDDBL_ZERO){
            // C is already in place
            ccddblSimplexSet(simplex, 1, A);
            ccddblSimplexSetSize(simplex, 2);
            tripleCross(&AC, &AO, &AC, dir);
        }else{
ccddbl_do_simplex3_45:
            dot = ccddblVec3Dot(&AB, &AO);
            if (ccddblIsZero(dot) || dot > CCDDBL_ZERO){
                ccddblSimplexSet(simplex, 0, B);
                ccddblSimplexSet(simplex, 1, A);
                ccddblSimplexSetSize(simplex, 2);
                tripleCross(&AB, &AO, &AB, dir);
            }else{
                ccddblSimplexSet(simplex, 0, A);
                ccddblSimplexSetSize(simplex, 1);
                ccddblVec3Copy(dir, &AO);
            }
        }
    }else{
        ccddblVec3Cross(&tmp, &AB, &ABC);
        dot = ccddblVec3Dot(&tmp, &AO);
        if (ccddblIsZero(dot) || dot > CCDDBL_ZERO){
            goto ccddbl_do_simplex3_45;
        }else{
            dot = ccddblVec3Dot(&ABC, &AO);
            if (ccddblIsZero(dot) || dot > CCDDBL_ZERO){
                ccddblVec3Copy(dir, &ABC);
            }else{
                ccddbl_support_t Ctmp;
                ccddblSupportCopy(&Ctmp, C);
                ccddblSimplexSet(simplex, 0, B);
                ccddblSimplexSet(simplex, 1, &Ctmp);

                ccddblVec3Copy(dir, &ABC);
                ccddblVec3Scale(dir, -CCDDBL_ONE);
            }
        }
    }

    return 0;
}

static int doSimplex4(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir)
{
    const ccddbl_support_t *A, *B, *C, *D;
    ccddbl_vec3_t AO, AB, AC, AD, ABC, ACD, ADB;
    int B_on_ACD, C_on_ADB, D_on_ABC;
    int AB_O, AC_O, AD_O;
    ccddbl_real_t dist;

    // get last added as A
    A = ccddblSimplexLast(simplex);
    // get the other points
    B = ccddblSimplexPoint(simplex, 2);
    C = ccddblSimplexPoint(simplex, 1);
    D = ccddblSimplexPoint(simplex, 0);

    // check if tetrahedron is really tetrahedron (has volume > 0)
    // if it is not simplex can't be expanded and thus no intersection is
    // found
    dist = ccddblVec3PointTriDist2(&A->v, &B->v, &C->v, &D->v, NULL);
    if (ccddblIsZero(dist)){
        return -1;
    }

    // check if origin lies on some of tetrahedron's face - if so objects
    // intersect
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &A->v, &B->v, &C->v, NULL);
    if (ccddblIsZero(dist))
        return 1;
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &A->v, &C->v, &D->v, NULL);
    if (ccddblIsZero(dist))
        return 1;
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &A->v, &B->v, &D->v, NULL);
    if (ccddblIsZero(dist))
        return 1;
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &B->v, &C->v, &D->v, NULL);
    if (ccddblIsZero(dist))
        return 1;

    // compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
    ccddblVec3Copy(&AO, &A->v);
    ccddblVec3Scale(&AO, -CCDDBL_ONE);
    ccddblVec3Sub2(&AB, &B->v, &A->v);
    ccddblVec3Sub2(&AC, &C->v, &A->v);
    ccddblVec3Sub2(&AD, &D->v, &A->v);
    ccddblVec3Cross(&ABC, &AB, &AC);
    ccddblVec3Cross(&ACD, &AC, &AD);
    ccddblVec3Cross(&ADB, &AD, &AB);

    // side (positive or negative) of B, C, D relative to planes ACD, ADB
    // and ABC respectively
    B_on_ACD = ccddblSign(ccddblVec3Dot(&ACD, &AB));
    C_on_ADB = ccddblSign(ccddblVec3Dot(&ADB, &AC));
    D_on_ABC = ccddblSign(ccddblVec3Dot(&ABC, &AD));

    // whether origin is on same side of ACD, ADB, ABC as B, C, D
    // respectively
    AB_O = ccddblSign(ccddblVec3Dot(&ACD, &AO)) == B_on_ACD;
    AC_O = ccddblSign(ccddblVec3Dot(&ADB, &AO)) == C_on_ADB;
    AD_O = ccddblSign(ccddblVec3Dot(&ABC, &AO)) == D_on_ABC;

    if (AB_O && AC_O && AD_O){
        // origin is in tetrahedron
        return 1;

    // rearrange simplex to triangle and call doSimplex3()
    }else if (!AB_O){
        // B is farthest from the origin among all of the tetrahedron's
        // points, so remove it from the list and go on with the triangle
        // case

        // D and C are in place
        ccddblSimplexSet(simplex, 2, A);
        ccddblSimplexSetSize(simplex, 3);
    }else if (!AC_O){
        // C is farthest
        ccddblSimplexSet(simplex, 1, D);
        ccddblSimplexSet(simplex, 0, B);
        ccddblSimplexSet(simplex, 2, A);
        ccddblSimplexSetSize(simplex, 3);
    }else{ // (!AD_O)
        ccddblSimplexSet(simplex, 0, C);
        ccddblSimplexSet(simplex, 1, B);
        ccddblSimplexSet(simplex, 2, A);
        ccddblSimplexSetSize(simplex, 3);
    }

    return doSimplex3(simplex, dir);
}

static int doSimplex(ccddbl_simplex_t *simplex, ccddbl_vec3_t *dir)
{
    if (ccddblSimplexSize(simplex) == 2){
        // simplex contains segment only one segment
        return doSimplex2(simplex, dir);
    }else if (ccddblSimplexSize(simplex) == 3){
        // simplex contains triangle
        return doSimplex3(simplex, dir);
    }else{ // ccddblSimplexSize(simplex) == 4
        // tetrahedron - this is the only shape which can encapsule origin
        // so doSimplex4() also contains test on it
        return doSimplex4(simplex, dir);
    }
}


_ccddbl_inline void tripleCross(const ccddbl_vec3_t *a, const ccddbl_vec3_t *b,
                             const ccddbl_vec3_t *c, ccddbl_vec3_t *d)
{
    ccddbl_vec3_t e;
    ccddblVec3Cross(&e, a, b);
    ccddblVec3Cross(d, &e, c);
}



/** Transforms simplex to polytope. It is assumed that simplex has 4
 *  vertices! */
static int simplexToPolytope4(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest)
{
    const ccddbl_support_t *a, *b, *c, *d;
    int use_polytope3;
    ccddbl_real_t dist;
    ccddbl_pt_vertex_t *v[4];
    ccddbl_pt_edge_t *e[6];
    size_t i;

    a = ccddblSimplexPoint(simplex, 0);
    b = ccddblSimplexPoint(simplex, 1);
    c = ccddblSimplexPoint(simplex, 2);
    d = ccddblSimplexPoint(simplex, 3);

    // check if origin lies on some of tetrahedron's face - if so use
    // simplexToPolytope3()
    use_polytope3 = 0;
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &a->v, &b->v, &c->v, NULL);
    if (ccddblIsZero(dist)){
        use_polytope3 = 1;
    }
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &a->v, &c->v, &d->v, NULL);
    if (ccddblIsZero(dist)){
        use_polytope3 = 1;
        ccddblSimplexSet(simplex, 1, c);
        ccddblSimplexSet(simplex, 2, d);
    }
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &a->v, &b->v, &d->v, NULL);
    if (ccddblIsZero(dist)){
        use_polytope3 = 1;
        ccddblSimplexSet(simplex, 2, d);
    }
    dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, &b->v, &c->v, &d->v, NULL);
    if (ccddblIsZero(dist)){
        use_polytope3 = 1;
        ccddblSimplexSet(simplex, 0, b);
        ccddblSimplexSet(simplex, 1, c);
        ccddblSimplexSet(simplex, 2, d);
    }

    if (use_polytope3){
        ccddblSimplexSetSize(simplex, 3);
        return simplexToPolytope3(obj1, obj2, ccddbl, simplex, pt, nearest);
    }

    // no touching contact - simply create tetrahedron
    for (i = 0; i < 4; i++){
        v[i] = ccddblPtAddVertex(pt, ccddblSimplexPoint(simplex, i));
    }
    
    e[0] = ccddblPtAddEdge(pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(pt, v[2], v[0]);
    e[3] = ccddblPtAddEdge(pt, v[3], v[0]);
    e[4] = ccddblPtAddEdge(pt, v[3], v[1]);
    e[5] = ccddblPtAddEdge(pt, v[3], v[2]);

    // ccddblPtAdd*() functions return NULL either if the memory allocation
    // failed of if any of the input pointers are NULL, so the bad
    // allocation can be checked by the last calls of ccddblPtAddFace()
    // because the rest of the bad allocations eventually "bubble up" here
    if (ccddblPtAddFace(pt, e[0], e[1], e[2]) == NULL
            || ccddblPtAddFace(pt, e[3], e[4], e[0]) == NULL
            || ccddblPtAddFace(pt, e[4], e[5], e[1]) == NULL
            || ccddblPtAddFace(pt, e[5], e[3], e[2]) == NULL){
        return -2;
    }

    return 0;
}

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              const ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest)
{
    const ccddbl_support_t *a, *b, *c;
    ccddbl_support_t d, d2;
    ccddbl_vec3_t ab, ac, dir;
    ccddbl_pt_vertex_t *v[5];
    ccddbl_pt_edge_t *e[9];
    ccddbl_real_t dist, dist2;

    *nearest = NULL;

    a = ccddblSimplexPoint(simplex, 0);
    b = ccddblSimplexPoint(simplex, 1);
    c = ccddblSimplexPoint(simplex, 2);

    // If only one triangle left from previous GJK run origin lies on this
    // triangle. So it is necessary to expand triangle into two
    // tetrahedrons connected with base (which is exactly abc triangle).

    // get next support point in direction of normal of triangle
    ccddblVec3Sub2(&ab, &b->v, &a->v);
    ccddblVec3Sub2(&ac, &c->v, &a->v);
    ccddblVec3Cross(&dir, &ab, &ac);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &d);
    dist = ccddblVec3PointTriDist2(&d.v, &a->v, &b->v, &c->v, NULL);

    // and second one take in opposite direction
    ccddblVec3Scale(&dir, -CCDDBL_ONE);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &d2);
    dist2 = ccddblVec3PointTriDist2(&d2.v, &a->v, &b->v, &c->v, NULL);

    // check if face isn't already on edge of minkowski sum and thus we
    // have touching contact
    if (ccddblIsZero(dist) || ccddblIsZero(dist2)){
        v[0] = ccddblPtAddVertex(pt, a);
        v[1] = ccddblPtAddVertex(pt, b);
        v[2] = ccddblPtAddVertex(pt, c);
        e[0] = ccddblPtAddEdge(pt, v[0], v[1]);
        e[1] = ccddblPtAddEdge(pt, v[1], v[2]);
        e[2] = ccddblPtAddEdge(pt, v[2], v[0]);
        *nearest = (ccddbl_pt_el_t *)ccddblPtAddFace(pt, e[0], e[1], e[2]);
        if (*nearest == NULL)
            return -2;

        return -1;
    }

    // form polyhedron
    v[0] = ccddblPtAddVertex(pt, a);
    v[1] = ccddblPtAddVertex(pt, b);
    v[2] = ccddblPtAddVertex(pt, c);
    v[3] = ccddblPtAddVertex(pt, &d);
    v[4] = ccddblPtAddVertex(pt, &d2);

    e[0] = ccddblPtAddEdge(pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(pt, v[2], v[0]);

    e[3] = ccddblPtAddEdge(pt, v[3], v[0]);
    e[4] = ccddblPtAddEdge(pt, v[3], v[1]);
    e[5] = ccddblPtAddEdge(pt, v[3], v[2]);

    e[6] = ccddblPtAddEdge(pt, v[4], v[0]);
    e[7] = ccddblPtAddEdge(pt, v[4], v[1]);
    e[8] = ccddblPtAddEdge(pt, v[4], v[2]);

    if (ccddblPtAddFace(pt, e[3], e[4], e[0]) == NULL
            || ccddblPtAddFace(pt, e[4], e[5], e[1]) == NULL
            || ccddblPtAddFace(pt, e[5], e[3], e[2]) == NULL

            || ccddblPtAddFace(pt, e[6], e[7], e[0]) == NULL
            || ccddblPtAddFace(pt, e[7], e[8], e[1]) == NULL
            || ccddblPtAddFace(pt, e[8], e[6], e[2]) == NULL){
        return -2;
    }

    return 0;
}

/** Transforms simplex to polytope, two vertices required */
static int simplexToPolytope2(const void *obj1, const void *obj2,
                              const ccddbl_t *ccddbl,
                              const ccddbl_simplex_t *simplex,
                              ccddbl_pt_t *pt, ccddbl_pt_el_t **nearest)
{
    const ccddbl_support_t *a, *b;
    ccddbl_vec3_t ab, ac, dir;
    ccddbl_support_t supp[4];
    ccddbl_pt_vertex_t *v[6];
    ccddbl_pt_edge_t *e[12];
    size_t i;
    int found;

    a = ccddblSimplexPoint(simplex, 0);
    b = ccddblSimplexPoint(simplex, 1);

    // This situation is a bit tricky. If only one segment comes from
    // previous run of GJK - it means that either this segment is on
    // minkowski edge (and thus we have touch contact) or it it isn't and
    // therefore segment is somewhere *inside* minkowski sum and it *must*
    // be possible to fully enclose this segment with polyhedron formed by
    // at least 8 triangle faces.

    // get first support point (any)
    found = 0;
    for (i = 0; i < ccddbl_points_on_sphere_len; i++){
        __ccddblSupport(obj1, obj2, &ccddbl_points_on_sphere[i], ccddbl, &supp[0]);
        if (!ccddblVec3Eq(&a->v, &supp[0].v) && !ccddblVec3Eq(&b->v, &supp[0].v)){
            found = 1;
            break;
        }
    }
    if (!found)
        goto simplexToPolytope2_touching_contact;

    // get second support point in opposite direction than supp[0]
    ccddblVec3Copy(&dir, &supp[0].v);
    ccddblVec3Scale(&dir, -CCDDBL_ONE);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &supp[1]);
    if (ccddblVec3Eq(&a->v, &supp[1].v) || ccddblVec3Eq(&b->v, &supp[1].v))
        goto simplexToPolytope2_touching_contact;

    // next will be in direction of normal of triangle a,supp[0],supp[1]
    ccddblVec3Sub2(&ab, &supp[0].v, &a->v);
    ccddblVec3Sub2(&ac, &supp[1].v, &a->v);
    ccddblVec3Cross(&dir, &ab, &ac);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &supp[2]);
    if (ccddblVec3Eq(&a->v, &supp[2].v) || ccddblVec3Eq(&b->v, &supp[2].v))
        goto simplexToPolytope2_touching_contact;

    // and last one will be in opposite direction
    ccddblVec3Scale(&dir, -CCDDBL_ONE);
    __ccddblSupport(obj1, obj2, &dir, ccddbl, &supp[3]);
    if (ccddblVec3Eq(&a->v, &supp[3].v) || ccddblVec3Eq(&b->v, &supp[3].v))
        goto simplexToPolytope2_touching_contact;

    goto simplexToPolytope2_not_touching_contact;
simplexToPolytope2_touching_contact:
    v[0] = ccddblPtAddVertex(pt, a);
    v[1] = ccddblPtAddVertex(pt, b);
    *nearest = (ccddbl_pt_el_t *)ccddblPtAddEdge(pt, v[0], v[1]);
    if (*nearest == NULL)
        return -2;

    return -1;

simplexToPolytope2_not_touching_contact:
    // form polyhedron
    v[0] = ccddblPtAddVertex(pt, a);
    v[1] = ccddblPtAddVertex(pt, &supp[0]);
    v[2] = ccddblPtAddVertex(pt, b);
    v[3] = ccddblPtAddVertex(pt, &supp[1]);
    v[4] = ccddblPtAddVertex(pt, &supp[2]);
    v[5] = ccddblPtAddVertex(pt, &supp[3]);

    e[0] = ccddblPtAddEdge(pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(pt, v[2], v[3]);
    e[3] = ccddblPtAddEdge(pt, v[3], v[0]);

    e[4] = ccddblPtAddEdge(pt, v[4], v[0]);
    e[5] = ccddblPtAddEdge(pt, v[4], v[1]);
    e[6] = ccddblPtAddEdge(pt, v[4], v[2]);
    e[7] = ccddblPtAddEdge(pt, v[4], v[3]);

    e[8]  = ccddblPtAddEdge(pt, v[5], v[0]);
    e[9]  = ccddblPtAddEdge(pt, v[5], v[1]);
    e[10] = ccddblPtAddEdge(pt, v[5], v[2]);
    e[11] = ccddblPtAddEdge(pt, v[5], v[3]);

    if (ccddblPtAddFace(pt, e[4], e[5], e[0]) == NULL
            || ccddblPtAddFace(pt, e[5], e[6], e[1]) == NULL
            || ccddblPtAddFace(pt, e[6], e[7], e[2]) == NULL
            || ccddblPtAddFace(pt, e[7], e[4], e[3]) == NULL

            || ccddblPtAddFace(pt, e[8],  e[9],  e[0]) == NULL
            || ccddblPtAddFace(pt, e[9],  e[10], e[1]) == NULL
            || ccddblPtAddFace(pt, e[10], e[11], e[2]) == NULL
            || ccddblPtAddFace(pt, e[11], e[8],  e[3]) == NULL){
        return -2;
    }

    return 0;
}

/** Expands polytope's tri by new vertex v. Triangle tri is replaced by
 *  three triangles each with one vertex in v. */
static int expandPolytope(ccddbl_pt_t *pt, ccddbl_pt_el_t *el,
                          const ccddbl_support_t *newv)
{
    ccddbl_pt_vertex_t *v[5];
    ccddbl_pt_edge_t *e[8];
    ccddbl_pt_face_t *f[2];


    // element can be either segment or triangle
    if (el->type == CCDDBL_PT_EDGE){
        // In this case, segment should be replaced by new point.
        // Simpliest case is when segment stands alone and in this case
        // this segment is replaced by two other segments both connected to
        // newv.
        // Segment can be also connected to max two faces and in that case
        // each face must be replaced by two other faces. To do this
        // correctly it is necessary to have correctly ordered edges and
        // vertices which is exactly what is done in following code.
        //

        ccddblPtEdgeVertices((const ccddbl_pt_edge_t *)el, &v[0], &v[2]);

        ccddblPtEdgeFaces((ccddbl_pt_edge_t *)el, &f[0], &f[1]);

        if (f[0]){
            ccddblPtFaceEdges(f[0], &e[0], &e[1], &e[2]);
            if (e[0] == (ccddbl_pt_edge_t *)el){
                e[0] = e[2];
            }else if (e[1] == (ccddbl_pt_edge_t *)el){
                e[1] = e[2];
            }
            ccddblPtEdgeVertices(e[0], &v[1], &v[3]);
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
                ccddblPtFaceEdges(f[1], &e[2], &e[3], &e[4]);
                if (e[2] == (ccddbl_pt_edge_t *)el){
                    e[2] = e[4];
                }else if (e[3] == (ccddbl_pt_edge_t *)el){
                    e[3] = e[4];
                }
                ccddblPtEdgeVertices(e[2], &v[3], &v[4]);
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


            v[4] = ccddblPtAddVertex(pt, newv);

            ccddblPtDelFace(pt, f[0]);
            if (f[1]){
                ccddblPtDelFace(pt, f[1]);
                ccddblPtDelEdge(pt, (ccddbl_pt_edge_t *)el);
            }

            e[4] = ccddblPtAddEdge(pt, v[4], v[2]);
            e[5] = ccddblPtAddEdge(pt, v[4], v[0]);
            e[6] = ccddblPtAddEdge(pt, v[4], v[1]);
            if (f[1])
                e[7] = ccddblPtAddEdge(pt, v[4], v[3]);


            if (ccddblPtAddFace(pt, e[1], e[4], e[6]) == NULL
                    || ccddblPtAddFace(pt, e[0], e[6], e[5]) == NULL){
                return -2;
            }

            if (f[1]){
                if (ccddblPtAddFace(pt, e[3], e[5], e[7]) == NULL
                        || ccddblPtAddFace(pt, e[4], e[7], e[2]) == NULL){
                    return -2;
                }
            }else{
                if (ccddblPtAddFace(pt, e[4], e[5], (ccddbl_pt_edge_t *)el) == NULL)
                    return -2;
            }
        }
    }else{ // el->type == CCDDBL_PT_FACE
        // replace triangle by tetrahedron without base (base would be the
        // triangle that will be removed)

        // get triplet of surrounding edges and vertices of triangle face
        ccddblPtFaceEdges((const ccddbl_pt_face_t *)el, &e[0], &e[1], &e[2]);
        ccddblPtEdgeVertices(e[0], &v[0], &v[1]);
        ccddblPtEdgeVertices(e[1], &v[2], &v[3]);

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
        ccddblPtDelFace(pt, (ccddbl_pt_face_t *)el);

        // expand triangle to tetrahedron
        v[3] = ccddblPtAddVertex(pt, newv);
        e[3] = ccddblPtAddEdge(pt, v[3], v[0]);
        e[4] = ccddblPtAddEdge(pt, v[3], v[1]);
        e[5] = ccddblPtAddEdge(pt, v[3], v[2]);

        if (ccddblPtAddFace(pt, e[3], e[4], e[0]) == NULL
                || ccddblPtAddFace(pt, e[4], e[5], e[1]) == NULL
                || ccddblPtAddFace(pt, e[5], e[3], e[2]) == NULL){
            return -2;
        }
    }

    return 0;
}

/** Finds next support point (and stores it in out argument).
 *  Returns 0 on success, -1 otherwise */
static int nextSupport(const void *obj1, const void *obj2, const ccddbl_t *ccddbl,
                       const ccddbl_pt_el_t *el,
                       ccddbl_support_t *out)
{
    ccddbl_vec3_t *a, *b, *c;
    ccddbl_real_t dist;

    if (el->type == CCDDBL_PT_VERTEX)
        return -1;

    // touch contact
    if (ccddblIsZero(el->dist))
        return -1;

    __ccddblSupport(obj1, obj2, &el->witness, ccddbl, out);

    // Compute dist of support point along element witness point direction
    // so we can determine whether we expanded a polytope surrounding the
    // origin a bit.
    dist = ccddblVec3Dot(&out->v, &el->witness);

    if (dist - el->dist < ccddbl->epa_tolerance)
        return -1;

    if (el->type == CCDDBL_PT_EDGE){
        // fetch end points of edge
        ccddblPtEdgeVec3((ccddbl_pt_edge_t *)el, &a, &b);

        // get distance from segment
        dist = ccddblVec3PointSegmentDist2(&out->v, a, b, NULL);
    }else{ // el->type == CCDDBL_PT_FACE
        // fetch vertices of triangle face
        ccddblPtFaceVec3((ccddbl_pt_face_t *)el, &a, &b, &c);

        // check if new point can significantly expand polytope
        dist = ccddblVec3PointTriDist2(&out->v, a, b, c, NULL);
    }

    if (dist < ccddbl->epa_tolerance)
        return -1;

    return 0;
}
