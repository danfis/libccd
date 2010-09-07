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

#ifndef __GJK_H__
#define __GJK_H__

#include <gjk/vec3.h>


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
 * Main structure of GJK algorithm.
 */
struct _gjk_t {
    gjk_first_dir_fn first_dir; //!< Returns initial direction where first
                                //   support point will be searched
    gjk_support_fn support; //!< Function that returns support point of object

    unsigned long max_iterations; //!< Maximal number of iterations
    double epa_tolerance;
};
typedef struct _gjk_t gjk_t;

/**
 * Default first direction.
 */
void gjkFirstDirDefault(const void *o1, const void *o2, gjk_vec3_t *dir);

#define GJK_INIT(gjk) \
    do { \
        (gjk)->first_dir = gjkFirstDirDefault; \
        (gjk)->support = NULL; \
        \
        (gjk)->max_iterations = (unsigned long)-1; \
        (gjk)->epa_tolerance = 0.0001; \
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

#endif /* __GJK_H__ */
