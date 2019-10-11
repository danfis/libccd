#include <stdio.h>
#include <cu/cu.h>
#include <ccddbl/ccddbl.h>
#include "support.h"
#include "common.h"


TEST(mprCylcylAlignedX)
{
    ccddbl_t ccddbl;
    CCDDBL_CYL(c1);
    CCDDBL_CYL(c2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    ccddblVec3Set(&c1.pos, -5., 0., 0.);
    for (i = 0; i < 100; i++){
        res = ccddblMPRIntersect(&c1, &c2, &ccddbl);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        c1.pos.v[0] += 0.1;
    }
}

TEST(mprCylcylAlignedY)
{
    ccddbl_t ccddbl;
    CCDDBL_CYL(c1);
    CCDDBL_CYL(c2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    ccddblVec3Set(&c1.pos, 0., -5., 0.);
    for (i = 0; i < 100; i++){
        res = ccddblMPRIntersect(&c1, &c2, &ccddbl);

        if (i < 42 || i > 58){
            assertFalse(res);
        }else{
            assertTrue(res);
        }

        c1.pos.v[1] += 0.1;
    }
}

TEST(mprCylcylAlignedZ)
{
    ccddbl_t ccddbl;
    CCDDBL_CYL(c1);
    CCDDBL_CYL(c2);
    size_t i;
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    c1.radius = 0.35;
    c1.height = 0.5;
    c2.radius = 0.5;
    c2.height = 1.;

    ccddblVec3Set(&c1.pos, 0., 0., -5.);
    for (i = 0; i < 100; i++){
        res = ccddblMPRIntersect(&c1, &c2, &ccddbl);

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
    ccddblVec3Scale(&dir, depth); \
    ccddblVec3Add(&cyl2.pos, &dir); \
    svtObjPen(&cyl1, &cyl2, stdout, "Pen 1", depth, &dir, &pos)

TEST(mprCylcylPenetration)
{
    ccddbl_t ccddbl;
    CCDDBL_CYL(cyl1);
    CCDDBL_CYL(cyl2);
    int res;
    ccddbl_vec3_t axis;
    ccddbl_real_t depth;
    ccddbl_vec3_t dir, pos;

    fprintf(stderr, "\n\n\n---- mprCylcylPenetration ----\n\n\n");

    cyl1.radius = 0.35;
    cyl1.height = 0.5;
    cyl2.radius = 0.5;
    cyl2.height = 1.;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    ccddbl.center1  = ccddblObjCenter;
    ccddbl.center2  = ccddblObjCenter;

    ccddblVec3Set(&cyl2.pos, 0., 0., 0.3);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();

    ccddblVec3Set(&cyl1.pos, 0.3, 0.1, 0.1);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT();

    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, 0., 0., 0.);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();

    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, -0.2, 0.7, 0.2);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    //TOSVT();

    ccddblVec3Set(&axis, 0.567, 1.2, 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();

    ccddblVec3Set(&axis, -4.567, 1.2, 0.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 3., &axis);
    ccddblVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    res = ccddblMPRPenetration(&cyl1, &cyl2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT();
}
