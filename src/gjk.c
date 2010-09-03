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

#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "simplex.h"
#include "polytope.h"
#include "dbg.h"

/** samples points on unit sphere (taken from bullet source) */
static gjk_vec3_t points_on_sphere[] = {
	GJK_VEC3_STATIC( 0.000000, -0.000000, -1.000000),
	GJK_VEC3_STATIC( 0.723608, -0.525725, -0.447219),
	GJK_VEC3_STATIC(-0.276388, -0.850649, -0.447219),
	GJK_VEC3_STATIC(-0.894426, -0.000000, -0.447216),
	GJK_VEC3_STATIC(-0.276388,  0.850649, -0.447220),
	GJK_VEC3_STATIC( 0.723608,  0.525725, -0.447219),
	GJK_VEC3_STATIC( 0.276388, -0.850649,  0.447220),
	GJK_VEC3_STATIC(-0.723608, -0.525725,  0.447219),
	GJK_VEC3_STATIC(-0.723608,  0.525725,  0.447219),
	GJK_VEC3_STATIC( 0.276388,  0.850649,  0.447219),
	GJK_VEC3_STATIC( 0.894426,  0.000000,  0.447216),
	GJK_VEC3_STATIC(-0.000000,  0.000000,  1.000000), 
	GJK_VEC3_STATIC( 0.425323, -0.309011, -0.850654),
	GJK_VEC3_STATIC(-0.162456, -0.499995, -0.850654),
	GJK_VEC3_STATIC( 0.262869, -0.809012, -0.525738),
	GJK_VEC3_STATIC( 0.425323,  0.309011, -0.850654),
	GJK_VEC3_STATIC( 0.850648, -0.000000, -0.525736),
	GJK_VEC3_STATIC(-0.525730, -0.000000, -0.850652),
	GJK_VEC3_STATIC(-0.688190, -0.499997, -0.525736),
	GJK_VEC3_STATIC(-0.162456,  0.499995, -0.850654),
	GJK_VEC3_STATIC(-0.688190,  0.499997, -0.525736),
	GJK_VEC3_STATIC( 0.262869,  0.809012, -0.525738),
	GJK_VEC3_STATIC( 0.951058,  0.309013,  0.000000),
	GJK_VEC3_STATIC( 0.951058, -0.309013,  0.000000),
	GJK_VEC3_STATIC( 0.587786, -0.809017,  0.000000),
	GJK_VEC3_STATIC( 0.000000, -1.000000,  0.000000),
	GJK_VEC3_STATIC(-0.587786, -0.809017,  0.000000),
	GJK_VEC3_STATIC(-0.951058, -0.309013, -0.000000),
	GJK_VEC3_STATIC(-0.951058,  0.309013, -0.000000),
	GJK_VEC3_STATIC(-0.587786,  0.809017, -0.000000),
	GJK_VEC3_STATIC(-0.000000,  1.000000, -0.000000),
	GJK_VEC3_STATIC( 0.587786,  0.809017, -0.000000),
	GJK_VEC3_STATIC( 0.688190, -0.499997,  0.525736),
	GJK_VEC3_STATIC(-0.262869, -0.809012,  0.525738),
	GJK_VEC3_STATIC(-0.850648,  0.000000,  0.525736),
	GJK_VEC3_STATIC(-0.262869,  0.809012,  0.525738),
	GJK_VEC3_STATIC( 0.688190,  0.499997,  0.525736),
	GJK_VEC3_STATIC( 0.525730,  0.000000,  0.850652),
	GJK_VEC3_STATIC( 0.162456, -0.499995,  0.850654),
	GJK_VEC3_STATIC(-0.425323, -0.309011,  0.850654),
	GJK_VEC3_STATIC(-0.425323,  0.309011,  0.850654),
	GJK_VEC3_STATIC( 0.162456,  0.499995,  0.850654)
};
static size_t points_on_sphere_len = sizeof(points_on_sphere) / sizeof(gjk_vec3_t);


/** Tests objects for intersection - this is common function for
 * gjkIntersect() and gjkSeparateEPA() */
static int _gjkIntersect(const void *obj1, const void *obj2,
                         const gjk_t *gjk, gjk_simplex_t *simplex);


/** Fills supp vector by support vector from Minkowski difference of obj1
 *  and obj2 in dir direction. */
static void support(const void *obj1, const void *obj2,
                    const gjk_vec3_t *dir, const gjk_t *gjk,
                    gjk_vec3_t *supp);

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
static void simplexToPolytope4(const gjk_simplex_t *simplex, gjk_pt_t *pt);

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex, gjk_pt_t *pt);

/** Transforms simplex to polytope, two vertices required */
static int simplexToPolytope2(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex, gjk_pt_t *pt);

/** Expands polytope using new vertex v. */
static void expandPolytope(gjk_pt_t *pt, gjk_pt_el_t *el,
                           const gjk_vec3_t *newv);

/** Finds next support point (at stores it in out argument).
 *  Returns 0 on success, -1 otherwise */
static int nextSupport(const void *obj1, const void *obj2, const gjk_t *gjk,
                       const gjk_pt_el_t *el,
                       gjk_vec3_t *out);



void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir)
{
    gjkVec3Set(dir, 1., 0., 0.);
}

int gjkIntersect(const void *obj1, const void *obj2, const gjk_t *gjk)
{
    gjk_simplex_t simplex;
    return _gjkIntersect(obj1, obj2, gjk, &simplex);
}

int gjkSeparateEPA(const void *obj1, const void *obj2, const gjk_t *gjk,
                   gjk_vec3_t *sep)
{
    gjk_simplex_t simplex;
    gjk_pt_t polytope;
    gjk_pt_el_t *nearest;
    gjk_vec3_t supp; // support point
    int ret, size;

    // run GJK and obtain terminal simplex
    ret = _gjkIntersect(obj1, obj2, gjk, &simplex);
    if (!ret)
        return -1;


    gjkPtInit(&polytope);

    // transform simplex to polytope - simplex won't be used anymore
    size = gjkSimplexSize(&simplex);
    if (size == 4){
        simplexToPolytope4(&simplex, &polytope);
    }else if (size == 3){
        if (simplexToPolytope3(obj1, obj2, gjk, &simplex, &polytope) != 0){
            gjkPtDestroy(&polytope);

            // touch contact
            gjkVec3Set(sep, 0., 0., 0.);
            return 0;
        }
    }else{ // size == 2
        if (simplexToPolytope2(obj1, obj2, gjk, &simplex, &polytope) != 0){
            gjkPtDestroy(&polytope);

            // touch contact
            gjkVec3Set(sep, 0., 0., 0.);
            return 0;
        }
    }

    while (1){
        // get triangle nearest to origin
        nearest = gjkPtNearest(&polytope);

        DBG2("nearest:");
        DBG_VEC3(&nearest->witness, "   w: ");
        DBG("   dist: %lf", nearest->dist);

        // get next support point
        if (nextSupport(obj1, obj2, gjk, nearest, &supp) != 0)
            break;

        // expand nearest triangle using new point - supp
        expandPolytope(&polytope, nearest, &supp);
    }

    // set separation vector
    gjkVec3Copy(sep, &nearest->witness);
    gjkVec3Scale(sep, -1.);

    gjkPtDestroy(&polytope);

    return 0;
}




static int _gjkIntersect(const void *obj1, const void *obj2,
                         const gjk_t *gjk, gjk_simplex_t *simplex)
{
    unsigned long iterations;
    gjk_vec3_t dir; // direction vector
    gjk_vec3_t last; // last support point
    int do_simplex_res;

    // initialize simplex struct
    gjkSimplexInit(simplex);

    // get first direction
    gjk->first_dir(obj1, obj2, &dir);
    // get first support point
    support(obj1, obj2, &dir, gjk, &last);
    // and add this point to simplex as last one
    gjkSimplexAdd(simplex, &last);

    // set up direction vector to as (O - last) which is exactly -last
    gjkVec3Copy(&dir, &last);
    gjkVec3Scale(&dir, -1.);

    // start iterations
    for (iterations = 0UL; iterations < gjk->max_iterations; ++iterations) {
        // obtain support point
        support(obj1, obj2, &dir, gjk, &last);

        DBG_VEC3(&last, "Last: ");


        // check if farthest point in Minkowski difference in direction dir
        // isn't somewhere before origin (the test on negative dot product)
        // - because if it is, objects are not intersecting at all.
        DBG("dot(last, dir): %lf", gjkVec3Dot(&last, &dir));
        if (gjkVec3Dot(&last, &dir) < 0.){
            return 0; // return false
        }

        // add last support vector to simplex
        gjkSimplexAdd(simplex, &last);

        // if doSimplex returns 1 if objects intersect, -1 if objects don't
        // intersect and 0 if algorithm should continue
        do_simplex_res = doSimplex(simplex, &dir);
        DBG("do_simplex_res: %d", do_simplex_res);
        if (do_simplex_res == 1){
            return 1;
        }else if (do_simplex_res == -1){
            return 0;
        }
        DBG2("...");

        if (gjkIsZero(gjkVec3Len2(&dir))){
            return 0;
        }
    }

    // intersection wasn't found
    return 0;
}

static void support(const void *obj1, const void *obj2,
                    const gjk_vec3_t *_dir, const gjk_t *gjk,
                    gjk_vec3_t *supp)
{
    gjk_vec3_t a, b, dir;

    gjkVec3Copy(&dir, _dir);

    gjk->support(obj1, &dir, &a);

    gjkVec3Scale(&dir, -1.);
    gjk->support(obj2, &dir, &b);

    gjkVec3Sub2(supp, &a, &b);
}

static int doSimplex2(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_vec3_t *A, *B;
    gjk_vec3_t AB, AO, tmp;
    double dot;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other point
    B = gjkSimplexPoint(simplex, 0);
    // compute AB oriented segment
    gjkVec3Sub2(&AB, B, A);
    // compute AO vector
    gjkVec3Copy(&AO, A);
    gjkVec3Scale(&AO, -1.);

    // dot product AB . AO
    dot = gjkVec3Dot(&AB, &AO);
    DBG_VEC3(A, "doSimplex2(): A: ");
    DBG_VEC3(B, "doSimplex2(): B: ");
    DBG_VEC3(&AB, "doSimplex2(): AB: ");
    DBG_VEC3(&AO, "doSimplex2(): AO: ");

    // check if origin doesn't lie on AB segment
    gjkVec3Cross(&tmp, &AB, &AO);
    if (gjkIsZero(gjkVec3Len2(&tmp)) && dot > 0.){
        DBG2(" --> Lies on AB line");
        DBG2("ENDS with only one segment");
        return 1;
    }

    // check if origin is in area where AB segment is
    if (dot < 0. || gjkIsZero(dot)){
        // origin is in outside are of A
        DBG2(" --> outside A");

        gjkSimplexSet1(simplex, A);
        gjkVec3Copy(dir, &AO);
    }else{
        // origin is in area where AB segment is
        DBG2(" --> in AB segment");

        // keep simplex untouched and set direction to
        // AB x AO x AB
        tripleCross(&AB, &AO, &AB, dir);
    }
    DBG_VEC3(dir, "doSimplex2(): dir: ");

    return 0;
}

static int doSimplex3(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    gjk_vec3_t A, B, C;
    gjk_vec3_t AO, AB, AC, ABC, tmp;
    double dot, dist;

    // get last added as A
    gjkVec3Copy(&A, gjkSimplexLast(simplex));
    // get the other points
    gjkVec3Copy(&B, gjkSimplexPoint(simplex, 1));
    gjkVec3Copy(&C, gjkSimplexPoint(simplex, 0));

    // check touching contact
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A, &B, &C, NULL);
    DBG("dist: %lf", dist);
    if (gjkIsZero(dist)){
        DBG2("ENDS with one triangle");
        return 1;
    }

    // check if triangle is really triangle (has area > 0)
    // if not simplex can't be expanded and thus no itersection is found
    if (gjkVec3Eq(&A, &B) || gjkVec3Eq(&A, &C)){
        return -1;
    }

    // compute AO vector
    gjkVec3Copy(&AO, &A);
    gjkVec3Scale(&AO, -1.);

    // compute AB and AC segments and ABC vector (perpendircular to triangle)
    gjkVec3Sub2(&AB, &B, &A);
    gjkVec3Sub2(&AC, &C, &A);
    gjkVec3Cross(&ABC, &AB, &AC);

    DBG_VEC3(&A, "doSimplex3(): A: ");
    DBG_VEC3(&B, "doSimplex3(): B: ");
    DBG_VEC3(&C, "doSimplex3(): C: ");
    DBG_VEC3(&AB, "doSimplex3(): AB: ");
    DBG_VEC3(&AC, "doSimplex3(): AC: ");
    DBG_VEC3(&AO, "doSimplex3(): AO: ");

    gjkVec3Cross(&tmp, &ABC, &AC);
    dot = gjkVec3Dot(&tmp, &AO);
    if (dot > 0. || gjkIsZero(dot)){
        dot = gjkVec3Dot(&AC, &AO);
        if (dot > 0. || gjkIsZero(dot)){
            DBG2("1");
            gjkSimplexSet2(simplex, &C, &A);
            tripleCross(&AC, &AO, &AC, dir);
        }else{
gjk_do_simplex3_45:
            dot = gjkVec3Dot(&AB, &AO);
            if (dot > 0. || gjkIsZero(dot)){
                DBG2("2");
                gjkSimplexSet2(simplex, &B, &A);
                tripleCross(&AB, &AO, &AB, dir);
            }else{
                DBG2("3");
                gjkSimplexSet1(simplex, &A);
                gjkVec3Copy(dir, &AO);
            }
        }
    }else{
        gjkVec3Cross(&tmp, &AB, &ABC);
        dot = gjkVec3Dot(&tmp, &AO);
        if (dot > 0. || gjkIsZero(dot)){
            goto gjk_do_simplex3_45;
        }else{
            dot = gjkVec3Dot(&ABC, &AO);
            if (dot > 0. || gjkIsZero(dot)){
                DBG2("4");
                gjkVec3Copy(dir, &ABC);
            }else{
                DBG2("5");
                gjkSimplexSet3(simplex, &B, &C, &A);
                gjkVec3Copy(dir, &ABC);
                gjkVec3Scale(dir, -1.);
            }
        }
    }

    DBG_VEC3(&A, "doSimplex3(): A: ");
    DBG_VEC3(&B, "doSimplex3(): B: ");
    DBG_VEC3(&C, "doSimplex3(): C: ");
    DBG_VEC3(dir, "doSimplex3(): dir: ");

    return 0;
}

static int doSimplex4(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    gjk_vec3_t A, B, C, D;
    gjk_vec3_t AO, AB, AC, AD, ABC, ACD, ADB;
    int B_on_ACD, C_on_ADB, D_on_ABC;
    int AB_O, AC_O, AD_O;
    double dist;

    // get last added as A
    gjkVec3Copy(&A, gjkSimplexLast(simplex));
    // get the other points
    gjkVec3Copy(&B, gjkSimplexPoint(simplex, 2));
    gjkVec3Copy(&C, gjkSimplexPoint(simplex, 1));
    gjkVec3Copy(&D, gjkSimplexPoint(simplex, 0));
    DBG_VEC3(&A, "doSimplex4(): A: ");
    DBG_VEC3(&B, "doSimplex4(): B: ");
    DBG_VEC3(&C, "doSimplex4(): C: ");
    DBG_VEC3(&D, "doSimplex4(): D: ");

    // check if tetrahedron is really tetrahedron (has volume > 0)
    // if it is not simplex can't be expanded and thus no intersection is
    // found
    dist = gjkVec3PointTriDist2(&A, &B, &C, &D, NULL);
    DBG("dist4: %lf", dist);
    if (gjkIsZero(dist)){
        return -1;
    }

    // check if origin lies on some of tetrahedron's face - if so objects
    // intersect
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A, &B, &C, NULL);
    DBG("--dist: %lf", dist);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A, &C, &D, NULL);
    DBG("--dist: %lf", dist);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &A, &B, &D, NULL);
    DBG("--dist: %lf", dist);
    if (gjkIsZero(dist))
        return 1;
    dist = gjkVec3PointTriDist2(gjk_vec3_origin, &B, &C, &D, NULL);
    DBG("--dist: %lf", dist);
    if (gjkIsZero(dist))
        return 1;

    // compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
    gjkVec3Copy(&AO, &A);
    gjkVec3Scale(&AO, -1.);
    gjkVec3Sub2(&AB, &B, &A);
    gjkVec3Sub2(&AC, &C, &A);
    gjkVec3Sub2(&AD, &D, &A);
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
        gjkSimplexSet3(simplex, &D, &C, &A);
        DBG2("  !AB_O");
    }else if (!AC_O){
        // C is farthest
        gjkSimplexSet3(simplex, &B, &D, &A);
        DBG2("  !AC_O");
    }else{ // (!AD_O)
        gjkSimplexSet3(simplex, &C, &B, &A);
        DBG2("  !AD_O");
    }

    return doSimplex3(simplex, dir);
}

static int doSimplex(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    DBG("doSimplex: size: %d", gjkSimplexSize(simplex));
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
static void simplexToPolytope4(const gjk_simplex_t *simplex, gjk_pt_t *pt)
{
    // simply create tetrahedron
    gjk_pt_vertex_t *v[4];
    gjk_pt_edge_t *e[6];
    size_t i;

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
}

/** Transforms simplex to polytope, three vertices required */
static int simplexToPolytope3(const void *obj1, const void *obj2,
                              const gjk_t *gjk,
                              const gjk_simplex_t *simplex, gjk_pt_t *pt)
{
    const gjk_vec3_t *a, *b, *c;
    gjk_vec3_t ab, ac, dir, d, d2;
    gjk_pt_vertex_t *v[5];
    gjk_pt_edge_t *e[9];
    double dist;

    a = gjkSimplexPoint(simplex, 0);
    b = gjkSimplexPoint(simplex, 1);
    c = gjkSimplexPoint(simplex, 2);

    // If only one triangle left from previous GJK run origin lies on this
    // triangle. So it is necessary to expand triangle into two
    // tetrahedrons connected with base (which is exactly abc triangle).

    // get next support point in direction of normal of triangle
    gjkVec3Sub2(&ab, b, a);
    gjkVec3Sub2(&ac, c, a);
    gjkVec3Cross(&dir, &ab, &ac);
    support(obj1, obj2, &dir, gjk, &d);

    // check if abc isn't already on edge
    dist = gjkVec3PointTriDist2(&d, a, b, c, NULL);
    if (gjkIsZero(dist))
        return -1;

    // and second one take in opposite direction
    gjkVec3Scale(&dir, -1.);
    support(obj1, obj2, &dir, gjk, &d2);

    // check if abc isn't already on edge
    dist = gjkVec3PointTriDist2(&d2, a, b, c, NULL);
    if (gjkIsZero(dist))
        return -1;

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
                              const gjk_simplex_t *simplex, gjk_pt_t *pt)
{
    const gjk_vec3_t *a, *b;
    gjk_vec3_t ab, ac, dir;
    gjk_vec3_t supp[4];
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
    for (i = 0; i < points_on_sphere_len; i++){
        support(obj1, obj2, &points_on_sphere[i], gjk, &supp[0]);
        if (!gjkVec3Eq(a, &supp[0]) && !gjkVec3Eq(b, &supp[0])){
            found = 1;
            break;
        }
    }
    if (!found)
        return -1;

    // get second support point in opposite direction than supp[0]
    gjkVec3Copy(&dir, &supp[0]);
    gjkVec3Scale(&dir, -1.);
    support(obj1, obj2, &dir, gjk, &supp[1]);
    if (gjkVec3Eq(a, &supp[1]) || gjkVec3Eq(b, &supp[1]))
        return -1;

    // next will be in direction of normal of triangle a,supp[0],supp[1]
    gjkVec3Sub2(&ab, &supp[0], a);
    gjkVec3Sub2(&ac, &supp[1], a);
    gjkVec3Cross(&dir, &ab, &ac);
    support(obj1, obj2, &dir, gjk, &supp[2]);
    if (gjkVec3Eq(a, &supp[2]) || gjkVec3Eq(b, &supp[2]))
        return -1;

    // and last one will be in opposite direction
    gjkVec3Scale(&dir, -1.);
    support(obj1, obj2, &dir, gjk, &supp[3]);
    if (gjkVec3Eq(a, &supp[3]) || gjkVec3Eq(b, &supp[3]))
        return -1;

    DBG2("");
    DBG_VEC3(&supp[0], "   0: ");
    DBG_VEC3(&supp[1], "   1: ");
    DBG_VEC3(&supp[2], "   2: ");
    DBG_VEC3(&supp[3], "   3: ");

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
                           const gjk_vec3_t *newv)
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
                if (v[1] != v[2])
                    v[1] = v[3];
            }else{
                if (v[1] != v[0])
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
            if (f[1])
                gjkPtDelFace(pt, f[1]);
            gjkPtDelEdge(pt, (gjk_pt_edge_t *)el);

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
                       gjk_vec3_t *out)
{
    gjk_vec3_t *a, *b, *c;
    double dist;

    if (el->type == GJK_PT_VERTEX)
        return -1;

    // touch contact
    if (gjkIsZero(el->dist))
        return -1;

    DBG2(" != 0");
    support(obj1, obj2, &el->witness, gjk, out);
    DBG_VEC3(out, "new support: ");

    if (el->type == GJK_PT_EDGE){
        // fetch end points of edge
        gjkPtEdgeVec3((gjk_pt_edge_t *)el, &a, &b);

        // get distance from segment
        dist = gjkVec3PointSegmentDist2(out, a, b, NULL);
    }else{ // el->type == GJK_PT_FACE
        // fetch vertices of triangle face
        gjkPtFaceVec3((gjk_pt_face_t *)el, &a, &b, &c);

        // check if new point can significantly expand polytope
        dist = gjkVec3PointTriDist2(out, a, b, c, NULL);
    }

    DBG("  dist: %lf", dist);
    if (dist < gjk->epa_tolerance)
        return -1;

    return 0;
}
