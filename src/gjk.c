#include <stdio.h>
#include "gjk.h"
#include "simplex.h"
#include "vec3.h"

/** Fills supp vector by support vector from Minkowski difference of obj1
 *  and obj2 in dir direction. */
static void support(const void *obj1, const void *obj2,
                    const gjk_vec3_t *dir, gjk_t *gjk,
                    gjk_vec3_t *supp);

/** Returns true if simplex contains origin.
 *  This function also alteres simplex and dir according to further
 *  processing of GJK algorithm. */
static int doSimplex(gjk_simplex_t *simplex, gjk_vec3_t *dir);

/** d = a x b x c
 */
_gjk_inline void tripleCross(const gjk_vec3_t *a, const gjk_vec3_t *b,
                             const gjk_vec3_t *c, gjk_vec3_t *d);



void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir)
{
    gjkVec3Set(dir, 1., 0., 0.);
}

int gjkIntersect(const void *obj1, const void *obj2, gjk_t *gjk)
{
    unsigned long iterations;
    gjk_simplex_t simplex;
    gjk_vec3_t dir; // direction vector
    gjk_vec3_t last; // last support point

    // initialize simplex struct
    gjkSimplexInit(&simplex);

    // get first direction
    gjk->first_dir(obj1, obj2, &dir);
    // get first support point
    support(obj1, obj2, &dir, gjk, &last);
    // and add this point to simplex as last one
    gjkSimplexAdd(&simplex, &last);

    // set up direction vector to as (O - last) which is exactly -last
    gjkVec3Copy(&dir, &last);
    gjkVec3Scale(&dir, -1.);

    // start iterations
    for (iterations = 0UL; iterations < gjk->max_iterations; ++iterations) {
        // obtain support point
        support(obj1, obj2, &dir, gjk, &last);
        GJK_PRINT_VEC3(&last, "Last:");


        // check if farthest point in Minkowski difference in direction dir
        // isn't somewhere before origin (the test on negative dot product)
        // - because if it is, objects are not intersecting at all.
        fprintf(stderr, "dot(last, dir): %lf\n", gjkVec3Dot(&last, &dir));
        if (gjkVec3Dot(&last, &dir) < 0.){
            return 0; // return false
        }

        // add last support vector to simplex
        gjkSimplexAdd(&simplex, &last);

        // if doSimplex returns true objects intersect, otherwise continue
        // in algorithm
        if (doSimplex(&simplex, &dir)){
            return 1;
        }

        if (isZero(gjkVec3Len2(&dir))){
            return 0;
        }
    }

    // intersection wasn't found
    return 0;
}

static void support(const void *obj1, const void *obj2,
                    const gjk_vec3_t *_dir, gjk_t *gjk,
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
    GJK_PRINT_VEC3(A, "doSimplex2(): A: ");
    GJK_PRINT_VEC3(B, "doSimplex2(): B: ");
    GJK_PRINT_VEC3(&AB, "doSimplex2(): AB: ");
    GJK_PRINT_VEC3(&AO, "doSimplex2(): AO: ");

    // check if origin doesn't lie on AB segment
    gjkVec3Cross(&tmp, &AB, &AO);
    if (isZero(gjkVec3Len2(&tmp)) && dot > 0.){
        fprintf(stderr, " --> Lies on AB line\n");
        return 1;
    }

    // check if origin is in area where AB segment is
    if (dot < 0. || isZero(dot)){
        // origin is in outside are of A
        fprintf(stderr, " --> outside A\n");

        gjkSimplexSet1(simplex, A);
        gjkVec3Copy(dir, &AO);
    }else{
        // origin is in area where AB segment is
        fprintf(stderr, " --> in AB segment\n");

        // keep simplex untouched and set direction to
        // AB x AO x AB
        tripleCross(&AB, &AO, &AB, dir);
    }
    GJK_PRINT_VEC3(dir, "doSimplex2(): dir: ");

    return 0;
}

static int doSimplex3(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_vec3_t *A, *B, *C;
    gjk_vec3_t AO, AB, AC, ABC, tmp;
    double dot;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other points
    B = gjkSimplexPoint(simplex, 1);
    C = gjkSimplexPoint(simplex, 0);

    // compute AO vector
    gjkVec3Copy(&AO, A);
    gjkVec3Scale(&AO, -1.);

    // compute AB and AC segments and ABC vector (perpendircular to triangle)
    gjkVec3Sub2(&AB, B, A);
    gjkVec3Sub2(&AC, C, A);
    gjkVec3Cross(&ABC, &AB, &AC);

    GJK_PRINT_VEC3(A, "doSimplex3(): A: ");
    GJK_PRINT_VEC3(B, "doSimplex3(): B: ");
    GJK_PRINT_VEC3(C, "doSimplex3(): C: ");
    GJK_PRINT_VEC3(&AB, "doSimplex3(): AB: ");
    GJK_PRINT_VEC3(&AC, "doSimplex3(): AC: ");
    GJK_PRINT_VEC3(&AO, "doSimplex3(): AO: ");

    gjkVec3Cross(&tmp, &ABC, &AC);
    dot = gjkVec3Dot(&tmp, &AO);
    if (dot > 0. || isZero(dot)){
        dot = gjkVec3Dot(&AC, &AO);
        if (dot > 0. || isZero(dot)){
            gjkSimplexSet2(simplex, C, A);
            tripleCross(&AC, &AO, &AC, dir);
        }else{
gjk_do_simplex3_45:
            dot = gjkVec3Dot(&AB, &AO);
            if (dot > 0. || isZero(dot)){
                gjkSimplexSet2(simplex, B, A);
                tripleCross(&AB, &AO, &AB, dir);
            }else{
                gjkSimplexSet1(simplex, A);
                gjkVec3Copy(dir, &AO);
            }
        }
    }else{
        gjkVec3Cross(&tmp, &AB, &ABC);
        dot = gjkVec3Dot(&tmp, &AO);
        if (dot > 0. || isZero(dot)){
            goto gjk_do_simplex3_45;
        }else{
            dot = gjkVec3Dot(&ABC, &AO);
            if (dot > 0. || isZero(dot)){
                gjkVec3Copy(dir, &ABC);
            }else{
                gjkSimplexSet3(simplex, B, C, A);
                gjkVec3Copy(dir, &ABC);
                gjkVec3Scale(dir, -1.);
            }
        }
    }
    GJK_PRINT_VEC3(dir, "doSimplex3(): dir: ");

    return 0;
}

static int doSimplex4(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    const gjk_vec3_t *A, *B, *C, *D;
    gjk_vec3_t AO, AB, AC, AD, ABC, ACD, ADB;
    int B_on_ACD, C_on_ADB, D_on_ABC;
    int AB_O, AC_O, AD_O;

    // get last added as A
    A = gjkSimplexLast(simplex);
    // get the other points
    B = gjkSimplexPoint(simplex, 2);
    C = gjkSimplexPoint(simplex, 1);
    D = gjkSimplexPoint(simplex, 0);
    fprintf(stderr, "  %ld\n", (long)A);
    fprintf(stderr, "  %ld\n", (long)B);
    fprintf(stderr, "  %ld\n", (long)C);
    fprintf(stderr, "  %ld\n", (long)D);
    GJK_PRINT_VEC3(A, "doSimplex3(): A: ");
    GJK_PRINT_VEC3(B, "doSimplex3(): B: ");
    GJK_PRINT_VEC3(C, "doSimplex3(): C: ");
    GJK_PRINT_VEC3(D, "doSimplex3(): D: ");

    // compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
    gjkVec3Copy(&AO, A);
    gjkVec3Scale(&AO, -1.);
    gjkVec3Sub2(&AB, B, A);
    gjkVec3Sub2(&AC, C, A);
    gjkVec3Sub2(&AD, D, A);
    gjkVec3Cross(&ABC, &AB, &AC);
    gjkVec3Cross(&ACD, &AC, &AD);
    gjkVec3Cross(&ADB, &AD, &AB);
    GJK_PRINT_VEC3(A, "doSimplex3(): A: ");
    GJK_PRINT_VEC3(B, "doSimplex3(): B: ");
    GJK_PRINT_VEC3(C, "doSimplex3(): C: ");
    GJK_PRINT_VEC3(D, "doSimplex3(): D: ");

    // side (positive or negative) of B, C, D relative to planes ACD, ADB
    // and ABC respectively
    B_on_ACD = sign(gjkVec3Dot(&ACD, &AB));
    C_on_ADB = sign(gjkVec3Dot(&ADB, &AC));
    D_on_ABC = sign(gjkVec3Dot(&ABC, &AD));

    // whether origin is on same side of ACD, ADB, ABC as B, C, D
    // respectively
    AB_O = sign(gjkVec3Dot(&ACD, &AO)) == B_on_ACD;
    AC_O = sign(gjkVec3Dot(&ADB, &AO)) == C_on_ADB;
    AD_O = sign(gjkVec3Dot(&ABC, &AO)) == D_on_ABC;

    if (AB_O && AC_O && AD_O){
        // origin is in tetrahedron
        return 1;

    // rearrange simplex to triangle and call doSimplex3()
    }else if (!AB_O){
        // B is farthest from the origin among all of the tetrahedron's
        // points, so remove it from the list and go on with the triangle
        // case
        gjkSimplexSet3(simplex, D, C, A);
        fprintf(stderr, "  !AB_O");
    }else if (!AC_O){
        // C is farthest
        gjkSimplexSet3(simplex, B, D, A);
        fprintf(stderr, "  !AC_O");
    }else{ // (!AD_O)
        gjkSimplexSet3(simplex, C, B, A);
        fprintf(stderr, "  !AD_O");
    }

    return doSimplex3(simplex, dir);
}

static int doSimplex(gjk_simplex_t *simplex, gjk_vec3_t *dir)
{
    fprintf(stderr, "doSimplex: size: %d\n", gjkSimplexSize(simplex));
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
