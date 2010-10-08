#include <cu/cu.h>
#include "common.h"
#include <gjk/gjk.h>
#include "support.h"

#define TOSVT() \
    svtObjPen(&box, &cyl, stdout, "Pen 1", depth, &dir, &pos); \
    gjkVec3Scale(&dir, depth); \
    gjkVec3Add(&cyl.pos, &dir); \
    svtObjPen(&box, &cyl, stdout, "Pen 1", depth, &dir, &pos)

TEST(mprBoxcylIntersect)
{
    gjk_t gjk;
    GJK_BOX(box);
    GJK_CYL(cyl);
    int res;
    gjk_vec3_t axis;

    box.x = 0.5;
    box.y = 1.;
    box.z = 1.5;
    cyl.radius = 0.4;
    cyl.height = 0.7;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    gjkVec3Set(&cyl.pos, 0.1, 0., 0.);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&cyl.pos, .6, 0., 0.);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&cyl.pos, .6, 0.6, 0.);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&axis, 0., 1., 0.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 3., &axis);
    gjkVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&axis, 0.67, 1.1, 0.12);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 4., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&axis, -0.1, 2.2, -1.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    gjkVec3Set(&axis, 1., 1., 0.);
    gjkQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    gjkVec3Set(&box.pos, .6, 0., 0.5);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);

    gjkVec3Set(&axis, -0.1, 2.2, -1.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    gjkVec3Set(&axis, 1., 1., 0.);
    gjkQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    gjkVec3Set(&box.pos, .9, 0.8, 0.5);
    res = gjkMPRIntersect(&box, &cyl, &gjk);
    assertTrue(res);
}



TEST(mprBoxcylPen)
{
    gjk_t gjk;
    GJK_BOX(box);
    GJK_CYL(cyl);
    int res;
    gjk_vec3_t axis;
    gjk_real_t depth;
    gjk_vec3_t dir, pos;

    box.x = 0.5;
    box.y = 1.;
    box.z = 1.5;
    cyl.radius = 0.4;
    cyl.height = 0.7;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    gjkVec3Set(&cyl.pos, 0.1, 0., 0.);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();

    gjkVec3Set(&cyl.pos, .6, 0., 0.);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT();

    gjkVec3Set(&cyl.pos, .6, 0.6, 0.);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();

    gjkVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    //TOSVT();

    gjkVec3Set(&axis, 0., 1., 0.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 3., &axis);
    gjkVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();

    gjkVec3Set(&axis, 0.67, 1.1, 0.12);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 4., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT();

    gjkVec3Set(&axis, -0.1, 2.2, -1.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    gjkVec3Set(&axis, 1., 1., 0.);
    gjkQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    gjkVec3Set(&box.pos, .6, 0., 0.5);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 7");
    //TOSVT();

    gjkVec3Set(&axis, -0.1, 2.2, -1.);
    gjkQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    gjkVec3Set(&cyl.pos, .6, 0., 0.5);
    gjkVec3Set(&axis, 1., 1., 0.);
    gjkQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    gjkVec3Set(&box.pos, .9, 0.8, 0.5);
    res = gjkMPRPenetration(&box, &cyl, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 8");
    //TOSVT();
}

