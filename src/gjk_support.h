/***
 * Some support() functions for some convex shapes.
 */

#ifndef __GJK_SUPPORT_H__
#define __GJK_SUPPORT_H__

#include <gjk/quat.h>

#define GJK_OBJ_BOX 1
#define GJK_OBJ_SPHERE 2
#define GJK_OBJ_CYL 3

#define __GJK_OBJ__ \
    int type; \
    gjk_vec3_t pos; \
    gjk_quat_t quat;

struct _gjk_obj_t {
    __GJK_OBJ__
};
typedef struct _gjk_obj_t gjk_obj_t;

struct _gjk_box_t {
    __GJK_OBJ__
    double x, y, z; //!< Lengths of box's edges
};
typedef struct _gjk_box_t gjk_box_t;

struct _gjk_sphere_t {
    __GJK_OBJ__
    double radius;
};
typedef struct _gjk_sphere_t gjk_sphere_t;

struct _gjk_cyl_t {
    __GJK_OBJ__
    double radius;
    double height;
};
typedef struct _gjk_cyl_t gjk_cyl_t;


#define GJK_BOX(name) \
    gjk_box_t name = { .type = GJK_OBJ_BOX, \
                       .pos  = { .v = { 0., 0., 0. } }, \
                       .quat = { .q = { 0., 0., 0., 1. } }, \
                       .x = 0., \
                       .y = 0., \
                       .z = 0. }

#define GJK_SPHERE(name) \
    gjk_sphere_t name = { .type = GJK_OBJ_SPHERE, \
                          .pos  = { .v = { 0., 0., 0. } }, \
                          .quat = { .q = { 0., 0., 0., 1. } }, \
                          .radius = 0. }

#define GJK_CYL(name) \
    gjk_cyl_t name = { .type = GJK_OBJ_CYL, \
                       .pos  = { .v = { 0., 0., 0. } }, \
                       .quat = { .q = { 0., 0., 0., 1. } }, \
                       .radius = 0., \
                       .height = 0. }

/**
 * Returns supporting vertex via v.
 * Supporting vertex is fathest vertex from object in direction dir.
 */
void gjkSupport(const void *obj, const gjk_vec3_t *dir,
                gjk_vec3_t *v);

#endif /* __GJK_SUPPORT_H__ */
