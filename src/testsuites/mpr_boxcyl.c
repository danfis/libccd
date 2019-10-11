#include <cu/cu.h>
#include <ccddbl/ccddbl.h>
#include "common.h"
#include "support.h"

#define TOSVT() \
    svtObjPen(&box, &cyl, stdout, "Pen 1", depth, &dir, &pos); \
    ccddblVec3Scale(&dir, depth); \
    ccddblVec3Add(&cyl.pos, &dir); \
    svtObjPen(&box, &cyl, stdout, "Pen 1", depth, &dir, &pos)

TEST(mprBoxcylIntersect)
{
    ccddbl_t ccddbl;
    CCDDBL_BOX(box);
    CCDDBL_CYL(cyl);
    int res;
    ccddbl_vec3_t axis;

    box.x = 0.5;
    box.y = 1.;
    box.z = 1.5;
    cyl.radius = 0.4;
    cyl.height = 0.7;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    ccddblVec3Set(&cyl.pos, 0.1, 0., 0.);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&cyl.pos, .6, 0., 0.);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&axis, 0., 1., 0.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 3., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&axis, 0.67, 1.1, 0.12);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .6, 0., 0.5);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .9, 0.8, 0.5);
    res = ccddblMPRIntersect(&box, &cyl, &ccddbl);
    assertTrue(res);
}



TEST(mprBoxcylPen)
{
    ccddbl_t ccddbl;
    CCDDBL_BOX(box);
    CCDDBL_CYL(cyl);
    int res;
    ccddbl_vec3_t axis;
    ccddbl_real_t depth;
    ccddbl_vec3_t dir, pos;

    box.x = 0.5;
    box.y = 1.;
    box.z = 1.5;
    cyl.radius = 0.4;
    cyl.height = 0.7;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    ccddblVec3Set(&cyl.pos, 0.1, 0., 0.);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();

    ccddblVec3Set(&cyl.pos, .6, 0., 0.);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT();

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    //TOSVT();

    ccddblVec3Set(&axis, 0., 1., 0.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 3., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();

    ccddblVec3Set(&axis, 0.67, 1.1, 0.12);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT();

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .6, 0., 0.5);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 7");
    //TOSVT();

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .9, 0.8, 0.5);
    res = ccddblMPRPenetration(&box, &cyl, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 8");
    //TOSVT();
}

