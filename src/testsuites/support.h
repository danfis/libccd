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

/***
 * Some support() functions for some convex shapes.
 */

#ifndef __CCDDBL_SUPPORT_H__
#define __CCDDBL_SUPPORT_H__

#include <ccddbl/quat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CCDDBL_OBJ_BOX 1
#define CCDDBL_OBJ_SPHERE 2
#define CCDDBL_OBJ_CYL 3

#define __CCDDBL_OBJ__ \
    int type; \
    ccddbl_vec3_t pos; \
    ccddbl_quat_t quat;

struct _ccddbl_obj_t {
    __CCDDBL_OBJ__
};
typedef struct _ccddbl_obj_t ccddbl_obj_t;

struct _ccddbl_box_t {
    __CCDDBL_OBJ__
    ccddbl_real_t x, y, z; //!< Lengths of box's edges
};
typedef struct _ccddbl_box_t ccddbl_box_t;

struct _ccddbl_sphere_t {
    __CCDDBL_OBJ__
    ccddbl_real_t radius;
};
typedef struct _ccddbl_sphere_t ccddbl_sphere_t;

struct _ccddbl_cyl_t {
    __CCDDBL_OBJ__
    ccddbl_real_t radius;
    ccddbl_real_t height;
};
typedef struct _ccddbl_cyl_t ccddbl_cyl_t;


#define CCDDBL_BOX(name) \
    ccddbl_box_t name = { .type = CCDDBL_OBJ_BOX, \
                       .pos  = { .v = { 0., 0., 0. } }, \
                       .quat = { .q = { 0., 0., 0., 1. } }, \
                       .x = 0., \
                       .y = 0., \
                       .z = 0. }

#define CCDDBL_SPHERE(name) \
    ccddbl_sphere_t name = { .type = CCDDBL_OBJ_SPHERE, \
                          .pos  = { .v = { 0., 0., 0. } }, \
                          .quat = { .q = { 0., 0., 0., 1. } }, \
                          .radius = 0. }

#define CCDDBL_CYL(name) \
    ccddbl_cyl_t name = { .type = CCDDBL_OBJ_CYL, \
                       .pos  = { .v = { 0., 0., 0. } }, \
                       .quat = { .q = { 0., 0., 0., 1. } }, \
                       .radius = 0., \
                       .height = 0. }

/**
 * Returns supporting vertex via v.
 * Supporting vertex is fathest vertex from object in direction dir.
 */
void ccddblSupport(const void *obj, const ccddbl_vec3_t *dir,
                ccddbl_vec3_t *v);

/**
 * Returns center of object.
 */
void ccddblObjCenter(const void *obj, ccddbl_vec3_t *center);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_SUPPORT_H__ */
