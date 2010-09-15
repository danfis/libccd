/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  libgjk is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  libgjk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include "gjk.h"
#include "vec3.h"
#include "gjk_support.h"

void gjkSupport(const void *_obj, const gjk_vec3_t *_dir,
                gjk_vec3_t *v)
{
    // Support function is made according to Gino van den Bergen's paper
    //  A Fast and Robust GJK Implementation for Collision Detection of
    //  Convex Objects

    gjk_obj_t *obj = (gjk_obj_t *)_obj;
    gjk_vec3_t dir;
    gjk_quat_t qinv;

    gjkVec3Copy(&dir, _dir);
    gjkQuatInvert2(&qinv, &obj->quat);

    gjkQuatRotVec(&dir, &qinv);

    if (obj->type == GJK_OBJ_BOX){
        gjk_box_t *box = (gjk_box_t *)obj;
        gjkVec3Set(v, gjkSign(gjkVec3X(&dir)) * box->x * GJK_REAL(0.5),
                      gjkSign(gjkVec3Y(&dir)) * box->y * GJK_REAL(0.5),
                      gjkSign(gjkVec3Z(&dir)) * box->z * GJK_REAL(0.5));
    }else if (obj->type == GJK_OBJ_SPHERE){
        gjk_sphere_t *sphere = (gjk_sphere_t *)obj;
        gjk_real_t len;

        len = gjkVec3Len2(&dir);
        if (len - GJK_EPS > GJK_ZERO){
            gjkVec3Copy(v, &dir);
            gjkVec3Scale(v, sphere->radius / GJK_SQRT(len));
        }else{
            gjkVec3Set(v, GJK_ZERO, GJK_ZERO, GJK_ZERO);
        }
    }else if (obj->type == GJK_OBJ_CYL){
        gjk_cyl_t *cyl = (gjk_cyl_t *)obj;
        gjk_real_t zdist, rad;

        zdist = dir.v[0] * dir.v[0] + dir.v[1] * dir.v[1];
        zdist = GJK_SQRT(zdist);
        if (gjkIsZero(zdist)){
            gjkVec3Set(v, GJK_ZERO, GJK_ZERO,
                          gjkSign(gjkVec3Z(&dir)) * cyl->height * GJK_REAL(0.5));
        }else{
            rad = cyl->radius / zdist;

            gjkVec3Set(v, rad * gjkVec3X(&dir),
                          rad * gjkVec3Y(&dir),
                          gjkSign(gjkVec3Z(&dir)) * cyl->height * GJK_REAL(0.5));
        }
    }

    // transform support vertex
    gjkQuatRotVec(v, &obj->quat);
    gjkVec3Add(v, &obj->pos);
}
