#include <stdio.h>
#include <cu/cu.h>
#include <ccddbl/vec3.h>

TEST(vec3SetUp)
{
}

TEST(vec3TearDown)
{
}


TEST(vec3PointSegmentDist)
{
    ccddbl_vec3_t P, a, b, w, ew;
    ccddbl_real_t dist;

    ccddblVec3Set(&a, 0., 0., 0.);
    ccddblVec3Set(&b, 1., 0., 0.);

    // extereme w == a
    ccddblVec3Set(&P, -1., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 1.));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -0.5, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.5 * 0.5));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -0.1, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, .1 * .1));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -1., 1., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 2.));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -0.5, 0.5, 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.5));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -0.1, -1., 2.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 5.01));
    assertTrue(ccddblVec3Eq(&w, &a));


    // extereme w == b
    ccddblVec3Set(&P, 2., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 1.));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 1.5, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.5 * 0.5));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 1.1, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, .1 * .1));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 1., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 2., 1., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 2.));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 1.5, 0.5, 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.5));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 1.1, -1., 2.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 5.01));
    assertTrue(ccddblVec3Eq(&w, &b));

    // inside segment
    ccddblVec3Set(&P, .5, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &P));

    ccddblVec3Set(&P, .9, 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &P));

    ccddblVec3Set(&P, .5, 1., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 1.));
    ccddblVec3Set(&ew, 0.5, 0., 0.);
    assertTrue(ccddblVec3Eq(&w, &ew));

    ccddblVec3Set(&P, .5, 1., 1.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 2.));
    ccddblVec3Set(&ew, 0.5, 0., 0.);
    assertTrue(ccddblVec3Eq(&w, &ew));



    ccddblVec3Set(&a, -.5, 2., 1.);
    ccddblVec3Set(&b, 1., 1.5, 0.5);

    // extereme w == a
    ccddblVec3Set(&P, -10., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 9.5 * 9.5 + 2. * 2. + 1.));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, -10., 9.2, 3.4);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 9.5 * 9.5 + 7.2 * 7.2 + 2.4 * 2.4));
    assertTrue(ccddblVec3Eq(&w, &a));

    // extereme w == b
    ccddblVec3Set(&P, 10., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 9. * 9. + 1.5 * 1.5 + 0.5 * 0.5));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, 10., 9.2, 3.4);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 9. * 9. + 7.7 * 7.7 + 2.9 * 2.9));
    assertTrue(ccddblVec3Eq(&w, &b));

    // inside ab
    ccddblVec3Set(&a, -.1, 1., 1.);
    ccddblVec3Set(&b, 1., 1., 1.);
    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointSegmentDist2(&P, &a, &b, &w);
    assertTrue(ccddblEq(dist, 2.));
    ccddblVec3Set(&ew, 0., 1., 1.);
    assertTrue(ccddblVec3Eq(&w, &ew));
}


TEST(vec3PointTriDist)
{
    ccddbl_vec3_t P, a, b, c, w, P0;
    ccddbl_real_t dist;

    ccddblVec3Set(&a, -1., 0., 0.);
    ccddblVec3Set(&b, 0., 1., 1.);
    ccddblVec3Set(&c, -1., 0., 1.);

    ccddblVec3Set(&P, -1., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &a));

    ccddblVec3Set(&P, 0., 1., 1.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &b));

    ccddblVec3Set(&P, -1., 0., 1.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &c));

    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, NULL);
    assertTrue(ccddblEq(dist, 2./3.));


    // region 4
    ccddblVec3Set(&P, -2., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &a)));
    assertTrue(ccddblVec3Eq(&w, &a));
    ccddblVec3Set(&P, -2., 0.2, -1.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &a)));
    assertTrue(ccddblVec3Eq(&w, &a));

    // region 2
    ccddblVec3Set(&P, -1.3, 0., 1.2);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &c)));
    assertTrue(ccddblVec3Eq(&w, &c));
    ccddblVec3Set(&P, -1.2, 0.2, 1.1);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &c)));
    assertTrue(ccddblVec3Eq(&w, &c));

    // region 6
    ccddblVec3Set(&P, 0.3, 1., 1.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &b)));
    assertTrue(ccddblVec3Eq(&w, &b));
    ccddblVec3Set(&P, .1, 1., 1.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, ccddblVec3Dist2(&P, &b)));
    assertTrue(ccddblVec3Eq(&w, &b));

    // region 1
    ccddblVec3Set(&P, 0., 1., 2.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 1.));
    assertTrue(ccddblVec3Eq(&w, &b));
    ccddblVec3Set(&P, -1., 0., 2.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 1.));
    assertTrue(ccddblVec3Eq(&w, &c));
    ccddblVec3Set(&P, -0.5, 0.5, 2.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 1.));
    ccddblVec3Set(&P0, -0.5, 0.5, 1.);
    assertTrue(ccddblVec3Eq(&w, &P0));

    // region 3
    ccddblVec3Set(&P, -2., -1., 0.7);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 2.));
    ccddblVec3Set(&P0, -1., 0., 0.7);
    assertTrue(ccddblVec3Eq(&w, &P0));

    // region 5
    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 2./3.));
    ccddblVec3Set(&P0, -2./3., 1./3., 1./3.);
    assertTrue(ccddblVec3Eq(&w, &P0));

    // region 0
    ccddblVec3Set(&P, -0.5, 0.5, 0.5);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &P));
    ccddblVec3Set(&P, -0.5, 0.5, 0.7);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &P));
    ccddblVec3Set(&P, -0.5, 0.5, 0.9);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.));
    assertTrue(ccddblVec3Eq(&w, &P));

    ccddblVec3Set(&P, 0., 0., 0.5);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.5));
    ccddblVec3Set(&P0, -.5, .5, .5);
    assertTrue(ccddblVec3Eq(&w, &P0));

    ccddblVec3Set(&a, -1., 0., 0.);
    ccddblVec3Set(&b, 0., 1., -1.);
    ccddblVec3Set(&c, 0., 1., 1.);
    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(ccddblEq(dist, 0.5));
    ccddblVec3Set(&P0, -.5, .5, 0.);
    assertTrue(ccddblVec3Eq(&w, &P0));
    //fprintf(stderr, "dist: %lf\n", dist);

    ccddblVec3Set(&a, -0.36715889, 0.288464308, 0.000100158155);
    ccddblVec3Set(&b, -0.0222680569, 0.0171524286, -2.88337469e-06);
    ccddblVec3Set(&c, 3.0792e-01, -2.4249e-01, 3.8363e-05);
    ccddblVec3Set(&P, 0., 0., 0.);
    dist = ccddblVec3PointTriDist2(&P, &a, &b, &c, &w);
    assertTrue(dist < 0.0000001);
}
