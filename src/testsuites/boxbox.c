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

    box1.x = 1;
    box1.y = 2;
    box1.z = 1;
    box2.x = 2;
    box2.y = 1;
    box2.z = 2;

    gjkVec3Set3(&box1.pos, -5., 0., 0.);
    for (i = 0; i < 100; i++){
        fprintf(stderr, "box1.pos: [%lf, %lf %lf]\n",
                gjkVec3X(&box1.pos),
                gjkVec3Y(&box1.pos),
                gjkVec3Z(&box1.pos));

        res = gjkIntersect(&box1, &box2, &gjk);
        printf("%d\n", res);

        box1.pos.v[0] += 0.1;
    }
}
