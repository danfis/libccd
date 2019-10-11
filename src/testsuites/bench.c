#ifndef CU_ENABLE_TIMER
#define CU_ENABLE_TIMER
#endif

#include <cu/cu.h>
#include <stdio.h>
#include <stdlib.h>
#include <ccddbl/ccddbl.h>
#include "support.h"

TEST_SUITES {
    TEST_SUITES_CLOSURE
};

static int bench_num = 1;
static size_t cycles = 10000;

static void runBench(const void *o1, const void *o2, const ccddbl_t *ccddbl)
{
    ccddbl_real_t depth;
    ccddbl_vec3_t dir, pos;
    size_t i;
    const struct timespec *timer;

    cuTimerStart();
    for (i = 0; i < cycles; i++){
        ccddblGJKPenetration(o1, o2, ccddbl, &depth, &dir, &pos);
    }
    timer = cuTimerStop();
    fprintf(stdout, "%02d: %ld %ld\n", bench_num,
                    (long)timer->tv_sec, (long)timer->tv_nsec);
    fflush(stdout);

    bench_num++;
}

static void boxbox(void)
{
    fprintf(stdout, "%s:\n", __func__);

    ccddbl_t ccddbl;
    CCDDBL_BOX(box1);
    CCDDBL_BOX(box2);
    ccddbl_vec3_t axis;
    ccddbl_quat_t rot;

    box1.x = box1.y = box1.z = 1.;
    box2.x = 0.5;
    box2.y = 1.;
    box2.z = 1.5;

    bench_num = 1;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    ccddblVec3Set(&box1.pos, -0.3, 0.5, 1.);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, 0., 0., 0.);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0., 0.);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    box1.x = box1.y = box1.z = 1.;
    box2.x = box2.y = box2.z = 1.;
    ccddblVec3Set(&axis, 0., 0., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0.5, 0.);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&box1.pos, -0.5, 0.1, 0.4);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);

    box1.x = box1.y = box1.z = 1.;
    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&box1.quat, M_PI / 4., &axis);
    ccddblVec3Set(&axis, 1., 1., 1.);
    ccddblQuatSetAngleAxis(&rot, M_PI / 4., &axis);
    ccddblQuatMul(&box1.quat, &rot);
    ccddblVec3Set(&box1.pos, -0.5, 0.1, 0.4);
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);


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
    runBench(&box1, &box2, &ccddbl);
    runBench(&box2, &box1, &ccddbl);


    fprintf(stdout, "\n----\n\n");
}

void cylcyl(void)
{
    fprintf(stdout, "%s:\n", __func__);

    ccddbl_t ccddbl;
    CCDDBL_CYL(cyl1);
    CCDDBL_CYL(cyl2);
    ccddbl_vec3_t axis;

    cyl1.radius = 0.35;
    cyl1.height = 0.5;
    cyl2.radius = 0.5;
    cyl2.height = 1.;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    ccddblVec3Set(&cyl1.pos, 0.3, 0.1, 0.1);
    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, 0., 0., 0.);
    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    ccddblVec3Set(&axis, 0., 1., 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, -0.2, 0.7, 0.2);
    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    ccddblVec3Set(&axis, 0.567, 1.2, 1.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    ccddblVec3Set(&axis, -4.567, 1.2, 0.);
    ccddblQuatSetAngleAxis(&cyl2.quat, M_PI / 3., &axis);
    ccddblVec3Set(&cyl2.pos, 0.6, -0.7, 0.2);
    runBench(&cyl1, &cyl2, &ccddbl);
    runBench(&cyl2, &cyl1, &ccddbl);

    fprintf(stdout, "\n----\n\n");
}

void boxcyl(void)
{
    fprintf(stdout, "%s:\n", __func__);

    ccddbl_t ccddbl;
    CCDDBL_BOX(box);
    CCDDBL_CYL(cyl);
    ccddbl_vec3_t axis;

    box.x = 0.5;
    box.y = 1.;
    box.z = 1.5;
    cyl.radius = 0.4;
    cyl.height = 0.7;

    CCDDBL_INIT(&ccddbl);
    ccddbl.support1 = ccddblSupport;
    ccddbl.support2 = ccddblSupport;

    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&cyl.pos, .6, 0., 0.);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&axis, 0., 1., 0.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 3., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0.6, 0.5);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&axis, 0.67, 1.1, 0.12);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 4., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .6, 0., 0.5);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    ccddblVec3Set(&axis, -0.1, 2.2, -1.);
    ccddblQuatSetAngleAxis(&cyl.quat, M_PI / 5., &axis);
    ccddblVec3Set(&cyl.pos, .6, 0., 0.5);
    ccddblVec3Set(&axis, 1., 1., 0.);
    ccddblQuatSetAngleAxis(&box.quat, -M_PI / 4., &axis);
    ccddblVec3Set(&box.pos, .9, 0.8, 0.5);
    runBench(&box, &cyl, &ccddbl);
    runBench(&cyl, &box, &ccddbl);

    fprintf(stdout, "\n----\n\n");
}

int main(int argc, char *argv[])
{
    if (argc > 1){
        cycles = atol(argv[1]);
    }

    fprintf(stdout, "Cycles: %u\n", (unsigned int)cycles);
    fprintf(stdout, "\n");

    boxbox();
    cylcyl();
    boxcyl();

    return 0;
}
