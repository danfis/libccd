#include <stdio.h>
#include "vec3.h"

double gjkVec3PointTriDist2(const gjk_vec3_t *P,
                            const gjk_vec3_t *A, const gjk_vec3_t *B,
                            const gjk_vec3_t *C,
                            gjk_vec3_t *witness)
{
    gjk_vec3_t E0, E1, D;
    double a, b, c, d, e, f, det, s, t;
    double tmp0, tmp1;
    double dist;

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
