#include <stdio.h>
#include <cu/cu.h>
#include "../gjk_support.h"
#include "../gjk.h"

TEST(spheresphereSetUp)
{
}

TEST(spheresphereTearDown)
{
}

TEST(spheresphereAlignedX)
{
    gjk_t gjk;
    GJK_SPHERE(s1);
    GJK_SPHERE(s2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    gjkVec3Set(&s1.pos, -5., 0., 0.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&s1, &s2, &gjk);

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
    gjk_t gjk;
    GJK_SPHERE(s1);
    GJK_SPHERE(s2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    gjkVec3Set(&s1.pos, 0., -5., 0.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&s1, &s2, &gjk);

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
    gjk_t gjk;
    GJK_SPHERE(s1);
    GJK_SPHERE(s2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    s1.radius = 0.35;
    s2.radius = .5;

    gjkVec3Set(&s1.pos, 0., 0., -5.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&s1, &s2, &gjk);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        s1.pos.v[2] += 0.1;
    }
}
