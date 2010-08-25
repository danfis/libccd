#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "gjk_support.h"

void gjkSupportBox(const void *_box, const gjk_vec3_t *dir,
                   gjk_vec3_t *vec)
{
    gjk_box_t *box = (gjk_box_t *)_box;
    gjk_vec3_t d;
    gjk_quat_t q;

    gjkVec3Copy(&d, dir);
    GJK_PRINT_VEC3(&d, "d");
    gjkQuatInvert2(&q, &box->quat);
    gjkQuatRotVec(&d, &q);
    GJK_PRINT_VEC3(&d, "d");

    gjkVec3Set(vec, sign(gjkVec3X(&d)) * box->x * 0.5,
                    sign(gjkVec3Y(&d)) * box->y * 0.5,
                    sign(gjkVec3Z(&d)) * box->z * 0.5);

    // apply affine transformation
    gjkQuatRotVec(vec, &box->quat);
    gjkVec3Add(vec, &box->pos);
}
