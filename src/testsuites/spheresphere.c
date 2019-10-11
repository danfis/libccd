#include <stdio.h>
#include <cu/cu.h>
#include <ccddbl/ccddbl.h>
#include "support.h"

TEST(spheresphereSetUp)
{
}

TEST(spheresphereTearDown)
{
}

TEST(spheresphereAlignedX)
{
    ccddbl_t ccddbl;
    CCDDBL_SPHERE(s1);
    CCDDBL_SPHERE(s2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    ccddblVec3Set(&s1.pos, -5., 0., 0.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&s1, &s2, &ccddbl);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        s1.pos.v[0] += 0.1;
    }
}

TEST(spheresphereAlignedY)
{
    ccddbl_t ccddbl;
    CCDDBL_SPHERE(s1);
    CCDDBL_SPHERE(s2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    ccddblVec3Set(&s1.pos, 0., -5., 0.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&s1, &s2, &ccddbl);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        s1.pos.v[1] += 0.1;
    }
}

TEST(spheresphereAlignedZ)
{
    ccddbl_t ccddbl;
    CCDDBL_SPHERE(s1);
    CCDDBL_SPHERE(s2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    ccddblVec3Set(&s1.pos, 0., 0., -5.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&s1, &s2, &ccddbl);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        s1.pos.v[2] += 0.1;
    }
}
