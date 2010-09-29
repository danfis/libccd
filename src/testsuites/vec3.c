#include <stdio.h>
#include <cu/cu.h>
#include <gjk/vec3.h>

TEST(vec3SetUp)
{
}

TEST(vec3TearDown)
{
}


TEST(vec3PointSegmentDist)
{
    gjk_vec3_t P, a, b, w, ew;
    gjk_real_t dist;

    gjkVec3Set(&a, 0., 0., 0.);
    gjkVec3Set(&b, 1., 0., 0.);

    // extereme w == a
    gjkVec3Set(&P, -1., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 1.));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -0.5, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.5 * 0.5));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -0.1, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, .1 * .1));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -1., 1., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 2.));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -0.5, 0.5, 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.5));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -0.1, -1., 2.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 5.01));
    assertTrue(gjkVec3Eq(&w, &a));


    // extereme w == b
    gjkVec3Set(&P, 2., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 1.));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 1.5, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.5 * 0.5));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 1.1, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, .1 * .1));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 1., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 2., 1., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 2.));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 1.5, 0.5, 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.5));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 1.1, -1., 2.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 5.01));
    assertTrue(gjkVec3Eq(&w, &b));

    // inside segment
    gjkVec3Set(&P, .5, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &P));

    gjkVec3Set(&P, .9, 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &P));

    gjkVec3Set(&P, .5, 1., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 1.));
    gjkVec3Set(&ew, 0.5, 0., 0.);
    assertTrue(gjkVec3Eq(&w, &ew));

    gjkVec3Set(&P, .5, 1., 1.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 2.));
    gjkVec3Set(&ew, 0.5, 0., 0.);
    assertTrue(gjkVec3Eq(&w, &ew));



    gjkVec3Set(&a, -.5, 2., 1.);
    gjkVec3Set(&b, 1., 1.5, 0.5);

    // extereme w == a
    gjkVec3Set(&P, -10., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 9.5 * 9.5 + 2. * 2. + 1.));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, -10., 9.2, 3.4);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 9.5 * 9.5 + 7.2 * 7.2 + 2.4 * 2.4));
    assertTrue(gjkVec3Eq(&w, &a));

    // extereme w == b
    gjkVec3Set(&P, 10., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 9. * 9. + 1.5 * 1.5 + 0.5 * 0.5));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, 10., 9.2, 3.4);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 9. * 9. + 7.7 * 7.7 + 2.9 * 2.9));
    assertTrue(gjkVec3Eq(&w, &b));

    // inside ab
    gjkVec3Set(&a, -.1, 1., 1.);
    gjkVec3Set(&b, 1., 1., 1.);
    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(gjkEq(dist, 2.));
    gjkVec3Set(&ew, 0., 1., 1.);
    assertTrue(gjkVec3Eq(&w, &ew));
}


TEST(vec3PointTriDist)
{
    gjk_vec3_t P, a, b, c, w, P0;
    gjk_real_t dist;

    gjkVec3Set(&a, -1., 0., 0.);
    gjkVec3Set(&b, 0., 1., 1.);
    gjkVec3Set(&c, -1., 0., 1.);

    gjkVec3Set(&P, -1., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &a));

    gjkVec3Set(&P, 0., 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &b));

    gjkVec3Set(&P, -1., 0., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &c));

    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, NULL);
    assertTrue(gjkEq(dist, 2./3.));


    // region 4
    gjkVec3Set(&P, -2., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &a)));
    assertTrue(gjkVec3Eq(&w, &a));
    gjkVec3Set(&P, -2., 0.2, -1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &a)));
    assertTrue(gjkVec3Eq(&w, &a));

    // region 2
    gjkVec3Set(&P, -1.3, 0., 1.2);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &c)));
    assertTrue(gjkVec3Eq(&w, &c));
    gjkVec3Set(&P, -1.2, 0.2, 1.1);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &c)));
    assertTrue(gjkVec3Eq(&w, &c));

    // region 6
    gjkVec3Set(&P, 0.3, 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &b)));
    assertTrue(gjkVec3Eq(&w, &b));
    gjkVec3Set(&P, .1, 1., 1.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, gjkVec3Dist2(&P, &b)));
    assertTrue(gjkVec3Eq(&w, &b));

    // region 1
    gjkVec3Set(&P, 0., 1., 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 1.));
    assertTrue(gjkVec3Eq(&w, &b));
    gjkVec3Set(&P, -1., 0., 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 1.));
    assertTrue(gjkVec3Eq(&w, &c));
    gjkVec3Set(&P, -0.5, 0.5, 2.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 1.));
    gjkVec3Set(&P0, -0.5, 0.5, 1.);
    assertTrue(gjkVec3Eq(&w, &P0));

    // region 3
    gjkVec3Set(&P, -2., -1., 0.7);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 2.));
    gjkVec3Set(&P0, -1., 0., 0.7);
    assertTrue(gjkVec3Eq(&w, &P0));

    // region 5
    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 2./3.));
    gjkVec3Set(&P0, -2./3., 1./3., 1./3.);
    assertTrue(gjkVec3Eq(&w, &P0));

    // region 0
    gjkVec3Set(&P, -0.5, 0.5, 0.5);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &P));
    gjkVec3Set(&P, -0.5, 0.5, 0.7);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &P));
    gjkVec3Set(&P, -0.5, 0.5, 0.9);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.));
    assertTrue(gjkVec3Eq(&w, &P));

    gjkVec3Set(&P, 0., 0., 0.5);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.5));
    gjkVec3Set(&P0, -.5, .5, .5);
    assertTrue(gjkVec3Eq(&w, &P0));

    gjkVec3Set(&a, -1., 0., 0.);
    gjkVec3Set(&b, 0., 1., -1.);
    gjkVec3Set(&c, 0., 1., 1.);
    gjkVec3Set(&P, 0., 0., 0.);
    dist = gjkVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(gjkEq(dist, 0.5));
    gjkVec3Set(&P0, -.5, .5, 0.);
    assertTrue(gjkVec3Eq(&w, &P0));
    //fprintf(stderr, "dist: %lf\n", dist);
}
