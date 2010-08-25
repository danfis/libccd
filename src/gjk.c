#include <stdio.h>
#include "gjk.h"
#include "simplex.h"
#include "vec3.h"

#define EPS 1E-10

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

/** Returns sign of value. */
_gjk_inline int sign(double val);
/** Returns true if val is zero. **/
_gjk_inline int isZero(double val);



void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir)
{
    gjkVec3Set3(dir, 1., 0., 0.);
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


        fprintf(stderr, "gjkIntersect last:[%lf %lf %lf], dir:[%lf %lf %lf]\n",
                gjkVec3X(&last), gjkVec3Y(&last), gjkVec3Z(&last),
                gjkVec3X(&dir), gjkVec3Y(&dir), gjkVec3Z(&dir));
        fprintf(stderr, "gjkIntersect dot(last, dir): %lf\n", gjkVec3Dot(&last, &dir));
        // check if farthest point in Minkowski difference in direction dir
        // isn't somewhere before origin (the test on negative dot product)
        // - because if it is, objects are not intersecting at all.
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

    // check if origin doesn't lie on AB segment
    gjkVec3Cross(&tmp, &AB, &AO);
    if (isZero(gjkVec3Len2(&tmp)) && dot > 0.){
        return 1;
    }

    // check if origin is in area where AB segment is
    fprintf(stderr, "doSimplex2(): A: [%lf, %lf, %lf]\n",
            gjkVec3X(A), gjkVec3Y(A), gjkVec3Z(A));
    fprintf(stderr, "doSimplex2(): B: [%lf, %lf, %lf]\n",
            gjkVec3X(B), gjkVec3Y(B), gjkVec3Z(B));
    fprintf(stderr, "doSimplex2(): AO: [%lf, %lf, %lf]\n",
            gjkVec3X(&AO), gjkVec3Y(&AO), gjkVec3Z(&AO));
    fprintf(stderr, "doSimplex2(): AB: [%lf, %lf, %lf]\n",
            gjkVec3X(&AB), gjkVec3Y(&AB), gjkVec3Z(&AB));
    fprintf(stderr, "doSimplex2: %lf - %d\n", dot, isZero(dot));
    if (dot < 0. || isZero(dot)){
        // origin is in outside are of A

        gjkSimplexSet1(simplex, A);
        gjkVec3Copy(dir, &AO);
    }else{
        // origin is in area where AB segment is

        // keep simplex untouched and set direction to
        // AB x AO x AB
        tripleCross(&AB, &AO, &AB, dir);
    }
    fprintf(stderr, "doSimplex2(): dir: [%lf, %lf, %lf] %lf\n",
            gjkVec3X(dir), gjkVec3Y(dir), gjkVec3Z(dir), gjkVec3Len2(dir));

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
    fprintf(stderr, "doSimplex3(): A: [%lf, %lf, %lf]\n",
            gjkVec3X(A), gjkVec3Y(A), gjkVec3Z(A));
    fprintf(stderr, "doSimplex3(): B: [%lf, %lf, %lf]\n",
            gjkVec3X(B), gjkVec3Y(B), gjkVec3Z(B));
    fprintf(stderr, "doSimplex3(): C: [%lf, %lf, %lf]\n",
            gjkVec3X(C), gjkVec3Y(C), gjkVec3Z(C));

    // compute AO vector
    gjkVec3Copy(&AO, A);
    gjkVec3Scale(&AO, -1.);

    // compute AB and AC segments and ABC vector (perpendircular to triangle)
    gjkVec3Sub2(&AB, B, A);
    gjkVec3Sub2(&AC, C, A);
    gjkVec3Cross(&ABC, &AB, &AO);
    fprintf(stderr, "doSimplex3(): ABO: [%lf, %lf, %lf]\n",
            gjkVec3X(&ABC), gjkVec3Y(&ABC), gjkVec3Z(&ABC));
    gjkVec3Cross(&ABC, &AB, &AC);
    fprintf(stderr, "doSimplex3(): AO: [%lf, %lf, %lf]\n",
            gjkVec3X(&AO), gjkVec3Y(&AO), gjkVec3Z(&AO));
    fprintf(stderr, "doSimplex3(): AB: [%lf, %lf, %lf]\n",
            gjkVec3X(&AB), gjkVec3Y(&AB), gjkVec3Z(&AB));
    fprintf(stderr, "doSimplex3(): AC: [%lf, %lf, %lf]\n",
            gjkVec3X(&AC), gjkVec3Y(&AC), gjkVec3Z(&AC));
    fprintf(stderr, "doSimplex3(): ABC: [%lf, %lf, %lf]\n",
            gjkVec3X(&ABC), gjkVec3Y(&ABC), gjkVec3Z(&ABC));

    gjkVec3Cross(&tmp, &ABC, &AC);
    dot = gjkVec3Dot(&tmp, &AO);
    if (dot > 0. || isZero(dot)){
        dot = gjkVec3Dot(&AC, &AO);
        if (dot > 0. || isZero(dot)){
            fprintf(stderr, "    1\n");
            gjkSimplexSet2(simplex, C, A);
            tripleCross(&AC, &AO, &AC, dir);
        }else{
gjk_do_simplex3_45:
            dot = gjkVec3Dot(&AB, &AO);
            if (dot > 0. || isZero(dot)){
                fprintf(stderr, "    4\n");
                gjkSimplexSet2(simplex, B, A);
                tripleCross(&AB, &AO, &AB, dir);
            }else{
                fprintf(stderr, "    5\n");
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
                fprintf(stderr, "    2\n");
                gjkVec3Copy(dir, &ABC);
            }else{
                fprintf(stderr, "    3\n");
                gjkSimplexSet3(simplex, B, C, A);
                gjkVec3Copy(dir, &ABC);
                gjkVec3Scale(dir, -1.);
            }
        }
    }

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

    // compute AO, AB, AC, AD segments and ABC, ACD, ADB normal vectors
    gjkVec3Copy(&AO, A);
    gjkVec3Scale(&AO, -1.);
    gjkVec3Sub2(&AB, B, A);
    gjkVec3Sub2(&AC, C, A);
    gjkVec3Sub2(&AD, D, A);
    gjkVec3Cross(&ABC, &AB, &AC);
    gjkVec3Cross(&ACD, &AC, &AD);
    gjkVec3Cross(&ADB, &AD, &AB);

    // side (positive or negative) of B, C, D relative to planes ACD, ADB
    // and ABC respectively
    B_on_ACD = sign(gjkVec3Dot(&ACD, &AB));
    C_on_ADB = sign(gjkVec3Dot(&ADB, &AC));
    D_on_ABC = sign(gjkVec3Dot(&ABC, &AD));
    fprintf(stderr, "doSimplex4(): B_on_ACD: %d, C_on_ADB: %d, D_on_ABC: %d\n",
            B_on_ACD, C_on_ADB, D_on_ABC);

    // whether origin is on same side of ACD, ADB, ABC as B, C, D
    // respectively
    AB_O = sign(gjkVec3Dot(&ACD, &AO)) == B_on_ACD;
    AC_O = sign(gjkVec3Dot(&ADB, &AO)) == C_on_ADB;
    AD_O = sign(gjkVec3Dot(&ABC, &AO)) == D_on_ABC;
    fprintf(stderr, "doSimplex4(): AB_O: %d, AC_O: %d, AD_O: %d\n",
            AB_O, AC_O, AD_O);

    if (AB_O && AC_O && AD_O){
        // origin is in tetrahedron
        return 1;

    // rearrange simplex to triangle and call doSimplex3()
    }else if (!AB_O){
        // B is farthest from the origin among all of the tetrahedron's
        // points, so remove it from the list and go on with the triangle
        // case
        gjkSimplexSet3(simplex, D, C, A);
    }else if (!AC_O){
        // C is farthest
        gjkSimplexSet3(simplex, B, D, A);
    }else{ // (!AD_O)
        gjkSimplexSet3(simplex, C, B, A);
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

_gjk_inline int sign(double val)
{
    fprintf(stderr, "sign: %lf\n", val);
    if (isZero(val)){
        fprintf(stderr, "sign: 0\n");
        return 0;
    }else if (val < 0.){
        fprintf(stderr, "sign: -1\n");
        return -1;
    }
        fprintf(stderr, "sign: 1\n");
    return 1;
}

_gjk_inline int isZero(double val)
{
    return fabs(val) < EPS;
}
