/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#include "support.h"

void __gjkSupport(const void *obj1, const void *obj2,
                  const gjk_vec3_t *_dir, const gjk_t *gjk,
                  gjk_support_t *supp)
{
    gjk_vec3_t dir;

    gjkVec3Copy(&dir, _dir);

    gjk->support(obj1, &dir, &supp->v1);

    gjkVec3Scale(&dir, -GJK_ONE);
    gjk->support(obj2, &dir, &supp->v2);

    gjkVec3Sub2(&supp->v, &supp->v1, &supp->v2);
}
