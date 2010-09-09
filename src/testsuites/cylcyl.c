#include <stdio.h>
#include <cu/cu.h>
#include "../gjk.h"
#include "../gjk_support.h"
#include "common.h"


TEST(cylcylSetUp)
{
}

TEST(cylcylTearDown)
{
}


TEST(cylcylAlignedX)
{
    gjk_t gjk;
    GJK_CYL(c1);
    GJK_CYL(c2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    gjkVec3Set(&c1.pos, -5., 0., 0.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&c1, &c2, &gjk);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        c1.pos.v[0] += 0.1;
    }
}

TEST(cylcylAlignedY)
{
    gjk_t gjk;
    GJK_CYL(c1);
    GJK_CYL(c2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    gjkVec3Set(&c1.pos, 0., -5., 0.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&c1, &c2, &gjk);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        c1.pos.v[1] += 0.1;
    }
}

TEST(cylcylAlignedZ)
{
    gjk_t gjk;
    GJK_CYL(c1);
    GJK_CYL(c2);
    size_t i;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    gjkVec3Set(&c1.pos, 0., 0., -5.);
    for (i = 0; i < 100; i++){
        res = gjkIntersect(&c1, &c2, &gjk);

        if (i < 43 || i > 57){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        c1.pos.v[2] += 0.1;

        svtObj(&c1, stdout, "0. 0. 1.", NULL);
        svtObj(&c2, stdout, "0. 0. 1.", NULL);
    }
}

