#include <stdio.h>
#include <cu/cu.h>

#include "../gjk.h"
#include "../gjk_support.h"
#include "../vec3.h"
#include "../dbg.h"
#include "common.h"


TEST(mprBoxboxAlignedX)
{
    size_t i;
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    gjkVec3Set(&box1.pos, -5., 0., 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);
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

    gjkVec3Set(&box1.pos, -0.5, 0., 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

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

    gjkVec3Set(&box1.pos, -5., -0.1, 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[0] += 0.1;
    }
}

TEST(mprBoxboxAlignedY)
{
    size_t i;
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    gjkVec3Set(&box1.pos, 0., -5., 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[1] += 0.1;
    }
}

TEST(mprBoxboxAlignedZ)
{
    size_t i;
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    gjkVec3Set(&box1.pos, 0., 0., -5.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

        if (i < 35 || i > 65){
            assertFalse(res);
        }else if (i != 35 && i != 65){
            assertTrue(res);
        }

        box1.pos.v[2] += 0.1;
    }
}


TEST(mprBoxboxRot)
{
    size_t i;
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;
    gjk_vec3_t axis;
    gjk_real_t angle;

    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    gjkVec3Set(&box1.pos, -5., 0.5, 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);
    gjkVec3Set(&axis, 0., 1., 0.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);

    for (i = 0; i < 100; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

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

    gjkVec3Set(&box1.pos, -1.01, 0., 0.);
    gjkVec3Set(&box2.pos, 0., 0., 0.);
    gjkQuatSet(&box1.quat, 0., 0., 0., 1.);
    gjkQuatSet(&box2.quat, 0., 0., 0., 1.);

    gjkVec3Set(&axis, 0., 1., 0.);
    angle = 0.;
    for (i = 0; i < 30; i++){
        res = gjkMPRIntersect(&box1, &box2, &gjk);

        if (i != 0 && i != 10 && i != 20){
            assertTrue(res);
        }else{
            assertFalse(res);
        }

        angle += M_PI / 20.;
        gjkQuatSetAngleAxis(&box1.quat, angle, &axis);
    }

}



static void pConf(gjk_box_t *box1, gjk_box_t *box2, const gjk_vec3_t *v)
{
    fprintf(stdout, "# box1.pos: [%lf %lf %lf]\n",
            gjkVec3X(&box1->pos), gjkVec3Y(&box1->pos), gjkVec3Z(&box1->pos));
    fprintf(stdout, "# box1->quat: [%lf %lf %lf %lf]\n",
            box1->quat.q[0], box1->quat.q[1], box1->quat.q[2], box1->quat.q[3]);
    fprintf(stdout, "# box2->pos: [%lf %lf %lf]\n",
            gjkVec3X(&box2->pos), gjkVec3Y(&box2->pos), gjkVec3Z(&box2->pos));
    fprintf(stdout, "# box2->quat: [%lf %lf %lf %lf]\n",
            box2->quat.q[0], box2->quat.q[1], box2->quat.q[2], box2->quat.q[3]);
    fprintf(stdout, "# sep: [%lf %lf %lf]\n",
            gjkVec3X(v), gjkVec3Y(v), gjkVec3Z(v));
    fprintf(stdout, "#\n");
}

TEST(mprBoxboxSeparate)
{
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;
    gjk_vec3_t sep, expsep, expsep2, axis;

    fprintf(stderr, "\n\n\n---- boxboxSeparate ----\n\n\n");

    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.5;
    box2.y = 1.;
    box2.z = 1.5;


    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;

    gjkVec3Set(&box1.pos, -0.5, 0.5, 0.2);
    res = gjkMPRIntersect(&box1, &box2, &gjk);
    assertTrue(res);

    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    gjkVec3Set(&expsep, 0.25, 0., 0.);
    assertTrue(gjkVec3Eq(&sep, &expsep));

    gjkVec3Scale(&sep, -1.);
    gjkVec3Add(&box1.pos, &sep);
    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    gjkVec3Set(&expsep, 0., 0., 0.);
    assertTrue(gjkVec3Eq(&sep, &expsep));


    gjkVec3Set(&box1.pos, -0.3, 0.5, 1.);
    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    gjkVec3Set(&expsep, 0., 0., -0.25);
    assertTrue(gjkVec3Eq(&sep, &expsep));



    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, 0., 0., 0.);

    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    gjkVec3Set(&expsep, 0., 0., 1.);
    gjkVec3Set(&expsep2, 0., 0., -1.);
    assertTrue(gjkVec3Eq(&sep, &expsep) || gjkVec3Eq(&sep, &expsep2));



    box1.x = box1.y = box1.z = 1.;
    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, -0.5, 0., 0.);

    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    pConf(&box1, &box2, &sep);



    box1.x = box1.y = box1.z = 1.;
    gjkVec3Set(&axis, 0., 1., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = gjkSeparateEPA(&box1, &box2, &gjk, &sep);
    assertTrue(res == 0);
    pConf(&box1, &box2, &sep);
}


#define TOSVT() \
    svtObjPen(&box1, &box2, stdout, "Pen 1", depth, &dir, &pos); \
    gjkVec3Scale(&dir, depth); \
    gjkVec3Add(&box2.pos, &dir); \
    svtObjPen(&box1, &box2, stdout, "Pen 1", depth, &dir, &pos)

TEST(mprBoxboxPenetration)
{
    gjk_t gjk;
    GJK_BOX(box1);
    GJK_BOX(box2);
    int res;
    gjk_vec3_t axis;
    gjk_quat_t rot;
    gjk_real_t depth;
    gjk_vec3_t dir, pos;

    fprintf(stderr, "\n\n\n---- boxboxPenetration ----\n\n\n");

    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.5;
    box2.y = 1.;
    box2.z = 1.5;


    GJK_INIT(&gjk);
    gjk.support1 = gjkSupport;
    gjk.support2 = gjkSupport;
    gjk.center1  = gjkObjCenter;
    gjk.center2  = gjkObjCenter;

    gjkVec3Set(&box2.pos, 0., 0., 0.);
    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    TOSVT();

    /*
    gjkVec3Set(&box2.pos, 0.1, 0., 0.);
    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 1");
    //TOSVT();
    */


    /*
    gjkVec3Set(&box1.pos, -0.3, 0.5, 1.);
    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 2");
    //TOSVT();
    */


    /*
    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, 0.1, 0., 0.1);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 3");
    //TOSVT();
    */


    /*
    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, -0.5, 0., 0.);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 4");
    TOSVT();
    */


    /*
    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, -0.5, 0.5, 0.);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 5");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    gjkVec3Set(&box2.pos, 0.1, 0., 0.);

    box1.x = box1.y = box1.z = 1.;
    gjkVec3Set(&axis, 0., 1., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 6");
    //TOSVT(); <<


    box1.x = box1.y = box1.z = 1.;
    gjkVec3Set(&axis, 0., 1., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&axis, 1., 1., 1.);
    gjkQuatSetAngleAxis(&rot, M_PI / 4., &axis);
    gjkQuatMul(&box1.quat, &rot);
    gjkVec3Set(&box1.pos, -0.5, 0.1, 0.4);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 7");
    //TOSVT();


    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.2; box2.y = 0.5; box2.z = 1.;
    box2.x = box2.y = box2.z = 1.;

    gjkVec3Set(&axis, 0., 0., 1.);
    gjkQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    gjkVec3Set(&axis, 1., 0., 0.);
    gjkQuatSetAngleAxis(&rot, M_PI / 4., &axis);
    gjkQuatMul(&box1.quat, &rot);
    gjkVec3Set(&box1.pos, -1.3, 0., 0.);

    gjkVec3Set(&box2.pos, 0., 0., 0.);

    res = gjkMPRPenetration(&box1, &box2, &gjk, &depth, &dir, &pos);
    assertTrue(res == 0);
    recPen(depth, &dir, &pos, stdout, "Pen 8");
    //TOSVT(); <<<
    */
}
