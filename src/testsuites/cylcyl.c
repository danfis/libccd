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
    }
}

#define TOSVT() \
    svtObjPen(&cyl1, &cyl2, stdout, "Pen 1", depth, &dir, &pos); \
    gjkVec3Scale(&dir, depth); \
    gjkVec3Add(&cyl2.pos, &dir); \
    svtObjPen(&cyl1, &cyl2, stdout, "Pen 1", depth, &dir, &pos)

TEST(cylcylPenetrationEPA)
{
    gjk_t gjk;
    GJK_CYL(cyl1);
    GJK_CYL(cyl2);
    int res;
    gjk_vec3_t axis;
    double depth;
    gjk_vec3_t dir, pos;

    fprintf(stderr, "\n\n\n---- cylcylPenetration ----\n\n\n");

    cyl1.radius = 0.35;
    cyl1.height = 0.5;
    cyl2.radius = 0.5;
    cyl2.height = 1.;

    GJK_INIT(&gjk);
    gjk.support = gjkSupport;

    gjkVec3Set(&cyl2.pos, 0., 0., 0.3);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();

    gjkVec3Set(&cyl1.pos, 0.3, 0.1, 0.1);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT(); <<<

    gjkVec3Set(&axis, 0., 1., 1.);
    gjkQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    gjkVec3Set(&cyl2.pos, 0., 0., 0.);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();

    gjkVec3Set(&axis, 0., 1., 1.);
    gjkQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    gjkVec3Set(&cyl2.pos, -0.2, 0.7, 0.2);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    //TOSVT();

    gjkVec3Set(&axis, 0.567, 1.2, 1.);
    gjkQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    gjkVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();

    gjkVec3Set(&axis, -4.567, 1.2, 0.);
    gjkQuatSetAngleAxis(&cyl2.quat, M_PI / 3., &axis);
    gjkVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    res = gjkPenetrationEPA(&cyl1, &cyl2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT();
}
