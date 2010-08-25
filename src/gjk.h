#ifndef __GJK_H__
#define __GJK_H__

#include <gjk/vec3.h>


/**
 * Type of *support* function that takes pointer to 3D object and direction
 * and returns (via vec argument) furthest point from object in specified
 * direction.
 */
typedef void (*gjk_support)(const void *obj, const gjk_vec3_t *dir,
                           gjk_vec3_t *vec);

/**
 * Returns (via dir argument) first direction vector that will be used in
 * initialization of algorithm.
 */
typedef void (*gjk_first_dir)(const void *obj1, const void *obj2,
                              gjk_vec3_t *dir);

/**
 * TODO
 */
struct _gjk_t {
    gjk_first_dir first_dir;
    gjk_support support;

    unsigned long max_iterations;
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
    } while(0)


/**
 * Returns true if two given objects interest.
 */
int gjkIntersect(const void *obj1, const void *obj2, gjk_t *gjk);

#endif /* __GJK_H__ */
