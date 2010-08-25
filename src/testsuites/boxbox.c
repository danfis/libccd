#include <stdio.h>
#include <cu/cu.h>

#include "../gjk.h"
#include "../gjk_support.h"
#include "../vec3.h"

TEST(boxboxSetUp)
{
}

TEST(boxboxTearDown)
{
}

TEST(boxboxAlignedX)
{
    size_t i;
    gjk_t gjk;
    gjk_box_t box1, box2;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupportBox;
    //gjk.max_iterations = 20;

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
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));
        printf("%d:\n", i);
        printf("  box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        //printf("%d\n", res);

        printf("  %d\n", res);
        fprintf(stderr, "  %d\n", res);
        if (i < 35 || i > 65){
            printf("  false - %d\n", res == 0);
            assertFalse(res);
        }else if (i != 35 && i != 65){
            printf("  true - %d\n", res == 1);
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
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        //printf("%d\n", res);

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
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));
        printf("%d:\n", i);
        printf("  box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        //printf("%d\n", res);

        printf("  %d\n", res);
        if (i < 35 || i > 65){
            printf("  false - %d\n", res == 0);
            assertFalse(res);
        }else if (i != 35 && i != 65){
            printf("  true - %d\n", res == 1);
            assertTrue(res);
        }

        box1.pos.v[0] += 0.1;
    }
}

TEST(boxboxAlignedY)
{
    size_t i;
    gjk_t gjk;
    gjk_box_t box1, box2;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupportBox;

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
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        //printf("%d\n", res);

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
    gjk_t gjk;
    gjk_box_t box1, box2;
    int res;

    GJK_INIT(&gjk);
    gjk.support = gjkSupportBox;

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
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        //printf("%d\n", res);

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
    gjk_t gjk;
    gjk_box_t box1, box2;
    int res;
    gjk_vec3_t axis;
    double angle;

    GJK_INIT(&gjk);
    gjk.support = gjkSupportBox;

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
        fprintf(stderr, "%d:\n", i);
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        fprintf(stderr, "  %d\n", res);

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
        res = gjkIntersect(&box1, &box2, &gjk);
        printf("%d - %d - %lf\n", i, res, angle);

        if (i != 0 && i != 10 && i != 20){
            assertTrue(res);
        }else{
            assertFalse(res);
        }

        angle += M_PI / 20.;
        gjkQuatSetAngleAxis(&box1.quat, angle, &axis);
    }

}
