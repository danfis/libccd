#include <stdio.h>
#include <cu/cu.h>

#include <ccddbl/ccddbl.h>
#include "support.h"
#include "../dbg.h"
#include "common.h"


TEST(boxboxSetUp)
{
}

TEST(boxboxTearDown)
{
}

TEST(boxboxAlignedX)
{
    size_t i;
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;
    //ccddbl.max_iterations = 20;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    ccddblVec3Set(&box1.pos, -5., 0., 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);
        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[0] += 0.1;
    }


    box1.x = 0.1;
    box1.y = 0.2;
    box1.z = 0.1;
    box2.x = 0.2;
    box2.y = 0.1;
    box2.z = 0.2;

    ccddblVec3Set(&box1.pos, -0.5, 0., 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[0] += 0.01;
    }


    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    ccddblVec3Set(&box1.pos, -5., -0.1, 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[0] += 0.1;
    }
}

TEST(boxboxAlignedY)
{
    size_t i;
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    ccddblVec3Set(&box1.pos, 0., -5., 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[1] += 0.1;
    }
}

TEST(boxboxAlignedZ)
{
    size_t i;
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    ccddblVec3Set(&box1.pos, 0., 0., -5.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[2] += 0.1;
    }
}


TEST(boxboxRot)
{
    size_t i;
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;
    ccddbl_vec3_t axis;
    ccddbl_real_t angle;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    ccddblVec3Set(&box1.pos, -5., 0.5, 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);
    ccddblVec3Set(&axis, 0., 1., 0.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);

    for (i = 0; i < 100; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i < 33 || i > 67){
            assertFalse(res);
        }else if (i != 33 && i != 67){
            assertTrue(res);
        }

        box1.pos.v[0] += 0.1;
    }

    box1.x = 1;
    box1.y = 1;
    box1.z = 1;
    box2.x = 1;
    box2.y = 1;
    box2.z = 1;

    ccddblVec3Set(&box1.pos, -1.01, 0., 0.);
    ccddblVec3Set(&box2.pos, 0., 0., 0.);
    ccddblQuatSet(&box1.quat, 0., 0., 0., 1.);
    ccddblQuatSet(&box2.quat, 0., 0., 0., 1.);

    ccddblVec3Set(&axis, 0., 1., 0.);
    angle = 0.;
    for (i = 0; i < 30; i++){
        res = ccddblGJKIntersect(&box1, &box2, &ccddbl);

        if (i != 0 && i != 10 && i != 20){
            assertTrue(res);
        }else{
            assertFalse(res);
        }

        angle += M_PI / 20.;
        ccddblQuatSetAngleAxis(&box1.quat, angle, &axis);
    }

}



static void pConf(ccddbl_box_t *box1, ccddbl_box_t *box2, const ccddbl_vec3_t *v)
{
    fprintf(stdout, "# box1.pos: [%lf %lf %lf]\n",
            ccddblVec3X(&box1->pos), ccddblVec3Y(&box1->pos), ccddblVec3Z(&box1->pos));
    fprintf(stdout, "# box1->quat: [%lf %lf %lf %lf]\n",
            box1->quat.q[0], box1->quat.q[1], box1->quat.q[2], box1->quat.q[3]);
    fprintf(stdout, "# box2->pos: [%lf %lf %lf]\n",
            ccddblVec3X(&box2->pos), ccddblVec3Y(&box2->pos), ccddblVec3Z(&box2->pos));
    fprintf(stdout, "# box2->quat: [%lf %lf %lf %lf]\n",
            box2->quat.q[0], box2->quat.q[1], box2->quat.q[2], box2->quat.q[3]);
    fprintf(stdout, "# sep: [%lf %lf %lf]\n",
            ccddblVec3X(v), ccddblVec3Y(v), ccddblVec3Z(v));
    fprintf(stdout, "#\n");
}

TEST(boxboxSeparate)
{
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;
    ccddbl_vec3_t sep, expsep, expsep2, axis;

    fprintf(stderr, "\n\n\n---- boxboxSeparate ----\n\n\n");

    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.5;
    box2.y = 1.;
    box2.z = 1.5;


    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    ccddblVec3Set(&box1.pos, -0.5, 0.5, 0.2);
    res = ccddblGJKIntersect(&box1, &box2, &ccddbl);
    assertTrue(res);

    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    ccddblVec3Set(&expsep, 0.25, 0., 0.);
    assertTrue(ccddblVec3Eq(&sep, &expsep));

    ccddblVec3Scale(&sep, -1.);
    ccddblVec3Add(&box1.pos, &sep);
    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    ccddblVec3Set(&expsep, 0., 0., 0.);
    assertTrue(ccddblVec3Eq(&sep, &expsep));


    ccddblVec3Set(&box1.pos, -0.3, 0.5, 1.);
    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    ccddblVec3Set(&expsep, 0., 0., -0.25);
    assertTrue(ccddblVec3Eq(&sep, &expsep));



    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, 0., 0., 0.);

    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    ccddblVec3Set(&expsep, 0., 0., 1.);
    ccddblVec3Set(&expsep2, 0., 0., -1.);
    assertTrue(ccddblVec3Eq(&sep, &expsep) || ccddblVec3Eq(&sep, &expsep2));



    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0., 0.);

    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    pConf(&box1, &box2, &sep);



    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = ccddblGJKSeparate(&box1, &box2, &ccddbl, &sep);
    assertTrue(res == 0);
    pConf(&box1, &box2, &sep);
}


#define TOSVT() \
    svtObjPen(&box1, &box2, stdout, "Pen 1", depth, &dir, &pos); \
    ccddblVec3Scale(&dir, depth); \
    ccddblVec3Add(&box2.pos, &dir); \
    svtObjPen(&box1, &box2, stdout, "Pen 1", depth, &dir, &pos)

TEST(boxboxPenetration)
{
    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;
    ccddbl_vec3_t axis;
    ccddbl_quat_t rot;
    ccddbl_real_t depth;
    ccddbl_vec3_t dir, pos;

    fprintf(stderr, "\n\n\n---- boxboxPenetration ----\n\n\n");

    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.5;
    box2.y = 1.;
    box2.z = 1.5;


    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    ccddblVec3Set(&box2.pos, 0.1, 0., 0.);
    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();


    ccddblVec3Set(&box1.pos, -0.3, 0.5, 1.);
    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT(); <<<


    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, 0.1, 0., 0.1);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0., 0.);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0.5, 0.);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&box2.pos, 0.1, 0., 0.);

    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&axis, 1., 1., 1.);
    ccddblQuatSetAngleAxis(&rot, M_PI / 4., &axis);
    ccddblQuatMul(&box1.quat, &rot);
    ccddblVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 7");
    //TOSVT(); <<<


    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.2; box2.y = 0.5; box2.z = 1.;
    box2.x = box2.y = box2.z = 1.;

    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&axis, 1., 0., 0.);
    ccddblQuatSetAngleAxis(&rot, M_PI / 4., &axis);
    ccddblQuatMul(&box1.quat, &rot);
    ccddblVec3Set(&box1.pos, -1.3, 0., 0.);

    ccddblVec3Set(&box2.pos, 0., 0., 0.);

    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 8");
    //TOSVT();
}

TEST(boxboxPenetration2)
{

    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    int res;
    ccddbl_real_t depth;
    ccddbl_vec3_t dir, pos;

    fprintf(stderr, "\n\n\n---- boxboxPenetration ----\n\n\n");

    box1.x = 20;
    box1.y = 0.1;
    box1.z = 20;
    box2.x = 0.2;
    box2.y = 0.15;
    box2.z = 0.2;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    ccddblVec3Set(&box2.pos, 0, 0.1, 0.);
    res = ccddblGJKPenetration(&box1, &box2, &ccddbl, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen largeBox");
    assertTrue(-0.2 < pos.v[0] && pos.v[0] <0.2);
    //TOSVT();

}
