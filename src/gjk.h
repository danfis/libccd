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

#ifndef __GJK_H__
#define __GJK_H__

#include <gjk/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Type of *support* function that takes pointer to 3D object and direction
 * and returns (via vec argument) furthest point from object in specified
 * direction.
 */
typedef void (*gjk_support_fn)(const void *obj, const gjk_vec3_t *dir,
                               gjk_vec3_t *vec);

/**
 * Returns (via dir argument) first direction vector that will be used in
 * initialization of algorithm.
 */
typedef void (*gjk_first_dir_fn)(const void *obj1, const void *obj2,
                                 gjk_vec3_t *dir);


/**
 * Returns (via center argument) geometric center (some point near center)
 * of given object.
 */
typedef void (*gjk_center_fn)(const void *obj1, gjk_vec3_t *center);

/**
 * Main structure of GJK algorithm.
 */
struct _gjk_t {
    gjk_first_dir_fn first_dir; //!< Returns initial direction where first
                                //!< support point will be searched
    gjk_support_fn support1; //!< Function that returns support point of
                             //!< first object
    gjk_support_fn support2; //!< Function that returns support point of
                             //!< second object

    gjk_center_fn center1; //!< Function that returns geometric center of
                           //!< first object
    gjk_center_fn center2; //!< Function that returns geometric center of
                           //!< second object

    unsigned long max_iterations; //!< Maximal number of iterations
    gjk_real_t epa_tolerance;
    gjk_real_t mpr_tolerance; //!< Boundary tolerance for MPR algorithm
};
typedef struct _gjk_t gjk_t;

/**
 * Default first direction.
 */
void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir);

#define GJK_INIT(gjk) \
    do { \
        (gjk)->first_dir = gjkFirstDirDefault; \
        (gjk)->support1 = NULL; \
        (gjk)->support2 = NULL; \
        (gjk)->center1  = NULL; \
        (gjk)->center2  = NULL; \
        \
        (gjk)->max_iterations = (unsigned long)-1; \
        (gjk)->epa_tolerance = GJK_REAL(0.0001); \
        (gjk)->mpr_tolerance = GJK_REAL(0.0001); \
    } while(0)


/**
 * Returns true if two given objects interest.
 */
int gjkIntersect(const void *obj1, const void *obj2, const gjk_t *gjk);

/**
 * This function computes separation vector of two objects. Separation
 * vector is minimal translation of obj2 to get obj1 and obj2 speparated
 * (without intersection).
 * Returns 0 if obj1 and obj2 intersect and sep is filled with translation
 * vector. If obj1 and obj2 don't intersect -1 is returned.
 */
int gjkSeparateEPA(const void *obj1, const void *obj2, const gjk_t *gjk,
                   gjk_vec3_t *sep);

/**
 * Computes penetration of obj2 into obj1.
 * Depth of penetration, direction and position is returned. It means that
 * if obj2 is translated by distance depth in direction dir objects will
 * have touching contact, pos should be position in global coordinates
 * where force should take a place.
 *
 * GJK+EPA algorithm is used.
 *
 * Returns 0 if obj1 and obj2 intersect and depth, dir and pos are filled
 * if given non-NULL pointers.
 * If obj1 and obj2 don't intersect -1 is returned.
 */
int gjkPenetrationEPA(const void *obj1, const void *obj2, const gjk_t *gjk,
                      gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos);


/**
 * Returns true if two given objects intersect - MPR algorithm is used.
 */
int gjkMPRIntersect(const void *obj1, const void *obj2, const gjk_t *gjk);

int gjkMPRPenetration(const void *obj1, const void *obj2, const gjk_t *gjk,
                      gjk_real_t *depth, gjk_vec3_t *dir, gjk_vec3_t *pos);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_H__ */
