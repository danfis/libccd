/***
 * libccddbl
 * ---------------------------------
 * Copyright (c)2010,2011 Daniel Fiser <danfis@danfis.cz>
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

#ifndef __CCDDBL_H__
#define __CCDDBL_H__

#include <ccddbl/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Type of *support* function that takes pointer to 3D object and direction
 * and returns (via vec argument) furthest point from object in specified
 * direction.
 */
typedef void (*ccddbl_support_fn)(const void *obj, const ccddbl_vec3_t *dir,
                               ccddbl_vec3_t *vec);

/**
 * Returns (via dir argument) first direction vector that will be used in
 * initialization of algorithm.
 */
typedef void (*ccddbl_first_dir_fn)(const void *obj1, const void *obj2,
                                 ccddbl_vec3_t *dir);


/**
 * Returns (via center argument) geometric center (some point near center)
 * of given object.
 */
typedef void (*ccddbl_center_fn)(const void *obj1, ccddbl_vec3_t *center);

/**
 * Main structure of CCDDBL algorithm.
 */
struct _ccddbl_t {
    ccddbl_first_dir_fn first_dir; //!< Returns initial direction where first
                                //!< support point will be searched
    ccddbl_support_fn support1; //!< Function that returns support point of
                             //!< first object
    ccddbl_support_fn support2; //!< Function that returns support point of
                             //!< second object

    ccddbl_center_fn center1; //!< Function that returns geometric center of
                           //!< first object
    ccddbl_center_fn center2; //!< Function that returns geometric center of
                           //!< second object

    unsigned long max_iterations; //!< Maximal number of iterations
    ccddbl_real_t epa_tolerance;
    ccddbl_real_t mpr_tolerance; //!< Boundary tolerance for MPR algorithm
    ccddbl_real_t dist_tolerance;
};
typedef struct _ccddbl_t ccddbl_t;

/**
 * Default first direction.
 */
CCDDBL_EXPORT void ccddblFirstDirDefault(const void *o1, const void *o2,
                                    ccddbl_vec3_t *dir);

#define CCDDBL_INIT(ccddbl) \
    do { \
        (ccddbl)->first_dir = ccddblFirstDirDefault; \
        (ccddbl)->support1 = NULL; \
        (ccddbl)->support2 = NULL; \
        (ccddbl)->center1  = NULL; \
        (ccddbl)->center2  = NULL; \
        \
        (ccddbl)->max_iterations = (unsigned long)-1; \
        (ccddbl)->epa_tolerance = CCDDBL_REAL(0.0001); \
        (ccddbl)->mpr_tolerance = CCDDBL_REAL(0.0001); \
        (ccddbl)->dist_tolerance = CCDDBL_REAL(1E-6); \
    } while(0)


/**
 * Returns true if two given objects interest.
 */
CCDDBL_EXPORT int ccddblGJKIntersect(const void *obj1, const void *obj2,
                                const ccddbl_t *ccddbl);

/**
 * This function computes separation vector of two objects. Separation
 * vector is minimal translation of obj2 to get obj1 and obj2 speparated
 * (without intersection).
 * Returns 0 if obj1 and obj2 intersect and sep is filled with translation
 * vector. If obj1 and obj2 don't intersect -1 is returned.
 * If memory allocation fails -2 is returned.
 */
CCDDBL_EXPORT int ccddblGJKSeparate(const void *obj1, const void *obj2,
                               const ccddbl_t *ccddbl, ccddbl_vec3_t *sep);

/**
 * Computes penetration of obj2 into obj1.
 * Depth of penetration, direction and position is returned. It means that
 * if obj2 is translated by distance depth in direction dir objects will
 * have touching contact, pos should be position in global coordinates
 * where force should take a place.
 *
 * CCDDBL+EPA algorithm is used.
 *
 * Returns 0 if obj1 and obj2 intersect and depth, dir and pos are filled
 * if given non-NULL pointers.
 * If obj1 and obj2 don't intersect -1 is returned.
 * If memory allocation fails -2 is returned.
 */
CCDDBL_EXPORT int ccddblGJKPenetration(const void *obj1, const void *obj2,
                                  const ccddbl_t *ccddbl, ccddbl_real_t *depth,
                                  ccddbl_vec3_t *dir, ccddbl_vec3_t *pos);

/**
 * Returns true if two given objects intersect - MPR algorithm is used.
 */
CCDDBL_EXPORT int ccddblMPRIntersect(const void *obj1, const void *obj2,
                                const ccddbl_t *ccddbl);

/**
 * Computes penetration of obj2 into obj1.
 * Depth of penetration, direction and position is returned, i.e. if obj2
 * is translated by computed depth in resulting direction obj1 and obj2
 * would have touching contact. Position is point in global coordinates
 * where force should take a place.
 *
 * Minkowski Portal Refinement algorithm is used (MPR, a.k.a. XenoCollide,
 * see Game Programming Gem 7).
 *
 * Returns 0 if obj1 and obj2 intersect, otherwise -1 is returned.
 */
CCDDBL_EXPORT int ccddblMPRPenetration(const void *obj1, const void *obj2,
                                  const ccddbl_t *ccddbl, ccddbl_real_t *depth,
                                  ccddbl_vec3_t *dir, ccddbl_vec3_t *pos);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_H__ */
