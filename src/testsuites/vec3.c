#include <stdio.h>
#include <cu/cu.h>
#include "../vec3.h"

TEST(vec3SetUp)
{
}

TEST(vec3TearDown)
{
}

TEST(vec3PointTriDist)
{
    gjk_vec3_t P, a, b, c, P2;
    double dist;

    gjkVec3Set(&a, -1., 0., 0.);
    gjkVec3Set(&b, 0., 1., 1.);
    gjkVec3Set(&c, -1., 0., 1.);

    gjkVec3Set(&P, -1., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));

    gjkVec3Set(&P, 0., 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));

    gjkVec3Set(&P, -1., 0., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));

    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 2./3.));


    // region 4
    gjkVec3Set(&P, -2., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &a)));
    gjkVec3Set(&P, -2., 0.2, -1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &a)));

    // region 2
    gjkVec3Set(&P, -1.3, 0., 1.2);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &c)));
    gjkVec3Set(&P, -1.2, 0.2, 1.1);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &c)));

    // region 6
    gjkVec3Set(&P, 0.3, 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &b)));
    gjkVec3Set(&P, .1, 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &b)));

    // region 1
    gjkVec3Set(&P, 0., 1., 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 1.));
    gjkVec3Set(&P, -1., 0., 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 1.));
    gjkVec3Set(&P, -0.5, 0.5, 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 1.));

    // region 3
    gjkVec3Set(&P, -2., -1., 0.6);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 2.));

    // region 5
    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 2./3.));

    // region 0
    gjkVec3Set(&P, -0.5, 0.5, 0.5);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));
    gjkVec3Set(&P, -0.5, 0.5, 0.7);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));
    gjkVec3Set(&P, -0.5, 0.5, 0.9);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.));

    gjkVec3Set(&P, 0., 0., 0.5);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.5));

    gjkVec3Set(&a, -1., 0., 0.);
    gjkVec3Set(&b, 0., 1., -1.);
    gjkVec3Set(&c, 0., 1., 1.);
    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c);
    assertTrue(gjkEq(dist, 0.5));
    //fprintf(stderr, "dist: %lf\n", dist);
}
