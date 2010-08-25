#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "gjk_support.h"

void gjkSupport(const void *_obj, const gjk_vec3_t *_dir,
                gjk_vec3_t *v)
{
    gjk_obj_t *obj = (gjk_obj_t *)_obj;
    gjk_vec3_t dir;
    gjk_quat_t qinv;

    gjkVec3Copy(&dir, _dir);
    gjkQuatInvert2(&qinv, &obj->quat);

    gjkQuatRotVec(&dir, &qinv);

    if (obj->type == GJK_OBJ_BOX){
        gjk_box_t *box = (gjk_box_t *)obj;
        gjkVec3Set(v, sign(gjkVec3X(&dir)) * box->x * 0.5,
                      sign(gjkVec3Y(&dir)) * box->y * 0.5,
                      sign(gjkVec3Z(&dir)) * box->z * 0.5);
    }

    // transform support vertex
    gjkQuatRotVec(v, &obj->quat);
    gjkVec3Add(v, &obj->pos);
}
