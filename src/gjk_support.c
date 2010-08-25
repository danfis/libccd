#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "gjk_support.h"

#define EPS 1E-10

_gjk_inline int isZero(double val)
{
    return fabs(val) < EPS;
}

_gjk_inline int sign(double val)
{
    if (isZero(val)){
        return 0;
    }else if (val < 0.){
        return -1;
    }
    return 1;
}

void gjkSupportBox(const void *_box, const gjk_vec3_t *dir,
                   gjk_vec3_t *vec)
{
    gjk_box_t *box = (gjk_box_t *)_box;
    gjk_vec3_t d;

    gjkVec3Copy(&d, dir);
    // TODO: apply rotation
    fprintf(stderr, "gjkSupportBox dir: [%lf %lf %lf]\n",
            gjkVec3X(dir), gjkVec3Y(dir), gjkVec3Z(dir));
    fprintf(stderr, "gjkSupportBox &d: [%lf %lf %lf]\n",
            gjkVec3X(&d), gjkVec3Y(&d), gjkVec3Z(&d));
    fprintf(stderr, "gjkSupportBox box: [%lf %lf %lf]\n",
            box->x, box->y, box->z);
    fprintf(stderr, "gjkSupportBox &box->pos: [%lf %lf %lf]\n",
            gjkVec3X(&box->pos), gjkVec3Y(&box->pos), gjkVec3Z(&box->pos));

    gjkVec3Set3(vec, sign(gjkVec3X(&d)) * box->x * 0.5,
                     sign(gjkVec3Y(&d)) * box->y * 0.5,
                     sign(gjkVec3Z(&d)) * box->z * 0.5);
    fprintf(stderr, "gjkSupportBox -> [%lf %lf %lf]\n",
            gjkVec3X(vec), gjkVec3Y(vec), gjkVec3Z(vec));

    // apply tranlsation
    gjkVec3Add(vec, &box->pos);

    fprintf(stderr, "gjkSupportBox -> [%lf %lf %lf]\n",
            gjkVec3X(vec), gjkVec3Y(vec), gjkVec3Z(vec));
}
