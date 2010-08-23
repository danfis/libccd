#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "gjk_support.h"

_gjk_inline int sign(double val)
{
    if (val < 0.)
        return -1;
    return 1;
}

void gjkSupportBox(const void *_box, const gjk_vec3_t *dir,
                   gjk_vec3_t *vec)
{
    gjk_box_t *box = (gjk_box_t *)_box;
    gjk_vec3_t d;

    gjkVec3Copy(&d, dir);
    // TODO: apply rotation

    gjkVec3Set3(vec, sign(gjkVec3X(&d)) * box->x * 0.5,
                     sign(gjkVec3Y(&d)) * box->y * 0.5,
                     sign(gjkVec3Z(&d)) * box->z * 0.5);

    // apply tranlsation
    gjkVec3Add(vec, &box->pos);

    fprintf(stderr, "gjkSupportBox -> [%lf %lf %lf]\n",
            gjkVec3X(vec), gjkVec3Y(vec), gjkVec3Z(vec));
}
