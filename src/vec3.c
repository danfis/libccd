#include <stdio.h>
#include "vec3.h"
#include "dbg.h"

static GJK_VEC3(__gjk_vec3_origin, 0., 0. ,0.);
gjk_vec3_t *gjk_vec3_origin = &__gjk_vec3_origin;


double gjkVec3PointSegmentDist2(const gjk_vec3_t *P,
                                const gjk_vec3_t *x0, const gjk_vec3_t *b,
                                gjk_vec3_t *witness)
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

    double dist, t;
    gjk_vec3_t d, a;

    // direction of segment
    gjkVec3Sub2(&d, b, x0);

    // precompute vector from P to x0
    gjkVec3Sub2(&a, x0, P);

    t  = -1. * gjkVec3Dot(&a, &d);
    t /= gjkVec3Len2(&d);

    if (t < 0. || isZero(t)){
        dist = gjkVec3Dist2(x0, P);
        if (witness)
            gjkVec3Copy(witness, x0);
    }else if (t > 1. || gjkEq(t, 1.)){
        dist = gjkVec3Dist2(b, P);
        if (witness)
            gjkVec3Copy(witness, b);
    }else{
        if (witness){
            gjkVec3Copy(witness, &d);
            gjkVec3Scale(witness, t);
            gjkVec3Add(witness, x0);
            dist = gjkVec3Dist2(witness, P);
        }else{
            // recycling variables
            gjkVec3Scale(&d, t);
            gjkVec3Add(&d, &a);
            dist = gjkVec3Len2(&d);
        }
    }

    return dist;
}


double gjkVec3PointTriDist2(const gjk_vec3_t *P,
                            const gjk_vec3_t *A, const gjk_vec3_t *B,
                            const gjk_vec3_t *C,
                            gjk_vec3_t *witness)
{
    gjk_vec3_t E0, E1, D;
    double a, b, c, d, e, f, det, s, t;
    double tmp0, tmp1;
    double dist;

    /*
    fprintf(stderr, "gjkVec3PointTriDist2():\n");
    GJK_PRINT_VEC3(P, "    P: ");
    GJK_PRINT_VEC3(A, "    A: ");
    GJK_PRINT_VEC3(B, "    B: ");
    GJK_PRINT_VEC3(C, "    C: ");
    */

    // precompute all values
    gjkVec3Sub2(&E0, B, A);
    gjkVec3Sub2(&E1, C, A);
    gjkVec3Sub2(&D, A, P);
    a = gjkVec3Dot(&E0, &E0);
    b = gjkVec3Dot(&E0, &E1);
    c = gjkVec3Dot(&E1, &E1);
    d = gjkVec3Dot(&E0, &D);
    e = gjkVec3Dot(&E1, &D);
    f = gjkVec3Dot(&D, &D);

    det = (a * c) - (b * b);
    s = (b * e) - (c * d);
    t = (b * d) - (a * e);

    if (s + t < det || gjkEq(s + t, det)){
        if (s < 0.){
            if (t < 0.){
                // region 4
                //fprintf(stderr, "region 4\n");
                // distance is distance from point A
                gjkVec3Sub2(&D, A, P);
                if (witness)
                    gjkVec3Copy(witness, A);
                return gjkVec3Len2(&D);
            }else{
                // region 3
                //fprintf(stderr, "region 3\n");
                s = 0.;
                if (e > 0. || isZero(e)){
                    t = 0.;
                }else{
                    if (-e > c || gjkEq(-e, c)){
                        t = 1.;
                    }else{
                        t = -e / c;
                    }
                }
            }
        }else if (t < 0.){
            // region 5
            //fprintf(stderr, "region 5\n");
            t = 0.;
            if (d > 0. || isZero(d)){
                s = 0.;
            }else{
                if (-d > a || gjkEq(-d, a)){
                    s = 1.;
                }else{
                    s = -d / a;
                }
            }
        }else{
            // region 0
            //fprintf(stderr, "region 0\n");
            tmp0 = 1. / det;
            s *= tmp0;
            t *= tmp0;
        }
    }else{
        if (s < 0.){
            // region 2
            //fprintf(stderr, "region 2\n");
            // distance is distance from point C
            gjkVec3Sub2(&D, C, P);
            if (witness)
                gjkVec3Copy(witness, C);
            return gjkVec3Len2(&D);
        }else if (t < 0.){
            // region 6
            //fprintf(stderr, "region 6\n");
            // distance is distance from point B
            gjkVec3Sub2(&D, B, P);
            if (witness)
                gjkVec3Copy(witness, B);
            return sqrt(gjkVec3Len2(&D));
        }else{
            // region 1
            //fprintf(stderr, "region 1\n");
            tmp0 = c + e - b - d;

            if (tmp0 < 0.){
                s = 0.;
            }else{
                tmp1 = a - 2. * b + c;
                if (tmp0 > tmp1 || gjkEq(tmp0, tmp1)){
                    s = 1.;
                }else{
                    s = tmp0 / tmp1;
                }
            }

            t = 1 - s;
        }
    }

#if 0
    {
        fprintf(stderr, "s: %lf, t: %lf\n", s, t);
        GJK_PRINT_VEC3(A, "A");
        GJK_PRINT_VEC3(A, "B");
        GJK_PRINT_VEC3(A, "C");
        GJK_PRINT_VEC3(P, "P");
        gjk_vec3_t T, aa, bb;
        gjkVec3Copy(&aa, &E0);
        gjkVec3Scale(&aa, s);
        gjkVec3Copy(&bb, &E1);
        gjkVec3Scale(&bb, t);
        gjkVec3Copy(&T, A);
        gjkVec3Add(&T, &aa);
        gjkVec3Add(&T, &bb);
        GJK_PRINT_VEC3(&T, "T");
    }
#endif
    if (witness){
        gjkVec3Scale(&E0, s);
        gjkVec3Scale(&E1, t);

        gjkVec3Copy(witness, A);
        gjkVec3Add(witness, &E0);
        gjkVec3Add(witness, &E1);
    }

    dist  = a * s * s;
    dist += 2. * b * s * t;
    dist += c * t * t;
    dist += 2. * d * s;
    dist += 2. * e * t;
    dist += f;

    return dist;
}
