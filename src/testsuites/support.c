/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libccddbl.
 *
 *  Distributed under the OSI-approved BSD License (the "License");
 *  see accompanying file BDS-LICENSE for details or see
 *  <http://www.opensource.org/licenses/bsd-license.php>.
 *
 *  This software is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the License for more information.
 */

#include <stdio.h>
#include <ccddbl/ccddbl.h>
#include "support.h"

void ccddblSupport(const void *_obj, const ccddbl_vec3_t *_dir,
                ccddbl_vec3_t *v)
{
    // Support function is made according to Gino van den Bergen's paper
    //  A Fast and Robust CCDDBL Implementation for Collision Detection of
    //  Convex Objects

    ccddbl_obj_t *obj = (ccddbl_obj_t *)_obj;
    ccddbl_vec3_t dir;
    ccddbl_quat_t qinv;

    ccddblVec3Copy(&dir, _dir);
    ccddblQuatInvert2(&qinv, &obj->quat);

    ccddblQuatRotVec(&dir, &qinv);

    if (obj->type == CCDDBL_OBJ_BOX){
        ccddbl_box_t *box = (ccddbl_box_t *)obj;
        ccddblVec3Set(v, ccddblSign(ccddblVec3X(&dir)) * box->x * CCDDBL_REAL(0.5),
                      ccddblSign(ccddblVec3Y(&dir)) * box->y * CCDDBL_REAL(0.5),
                      ccddblSign(ccddblVec3Z(&dir)) * box->z * CCDDBL_REAL(0.5));
    }else if (obj->type == CCDDBL_OBJ_SPHERE){
        ccddbl_sphere_t *sphere = (ccddbl_sphere_t *)obj;
        ccddbl_real_t len;

        len = ccddblVec3Len2(&dir);
        if (len - CCDDBL_EPS > CCDDBL_ZERO){
            ccddblVec3Copy(v, &dir);
            ccddblVec3Scale(v, sphere->radius / CCDDBL_SQRT(len));
        }else{
            ccddblVec3Set(v, CCDDBL_ZERO, CCDDBL_ZERO, CCDDBL_ZERO);
        }
    }else if (obj->type == CCDDBL_OBJ_CYL){
        ccddbl_cyl_t *cyl = (ccddbl_cyl_t *)obj;
        ccddbl_real_t zdist, rad;

        zdist = dir.v[0] * dir.v[0] + dir.v[1] * dir.v[1];
        zdist = CCDDBL_SQRT(zdist);
        if (ccddblIsZero(zdist)){
            ccddblVec3Set(v, CCDDBL_ZERO, CCDDBL_ZERO,
                          ccddblSign(ccddblVec3Z(&dir)) * cyl->height * CCDDBL_REAL(0.5));
        }else{
            rad = cyl->radius / zdist;

            ccddblVec3Set(v, rad * ccddblVec3X(&dir),
                          rad * ccddblVec3Y(&dir),
                          ccddblSign(ccddblVec3Z(&dir)) * cyl->height * CCDDBL_REAL(0.5));
        }
    }

    // transform support vertex
    ccddblQuatRotVec(v, &obj->quat);
    ccddblVec3Add(v, &obj->pos);
}

void ccddblObjCenter(const void *_obj, ccddbl_vec3_t *center)
{
    ccddbl_obj_t *obj = (ccddbl_obj_t *)_obj;

    ccddblVec3Set(center, CCDDBL_ZERO, CCDDBL_ZERO, CCDDBL_ZERO);
    // rotation is not needed
    ccddblVec3Add(center, &obj->pos);
}
