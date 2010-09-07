#include "support.h"

void __gjkSupport(const void *obj1, const void *obj2,
                  const gjk_vec3_t *_dir, const gjk_t *gjk,
                  gjk_support_t *supp)
{
    gjk_vec3_t dir;

    gjkVec3Copy(&dir, _dir);

    gjk->support(obj1, &dir, &supp->v1);

    gjkVec3Scale(&dir, -1.);
    gjk->support(obj2, &dir, &supp->v2);

    gjkVec3Sub2(&supp->v, &supp->v1, &supp->v2);
}
