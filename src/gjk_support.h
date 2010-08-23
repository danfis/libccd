/***
 * Some support() functions for some convex shapes.
 */

#ifndef __GJK_SUPPORT_H__
#define __GJK_SUPPORT_H__

struct _gjk_box_t {
    gjk_vec3_t pos; //!< Position in 3D space
    // TODO: rotation
    double x, y, z; //!< Lengths of box's edges
};
typedef struct _gjk_box_t gjk_box_t;


/**
 * Returns supporting vertex of box.
 */
void gjkSupportBox(const void *box, const gjk_vec3_t *dir,
                   gjk_vec3_t *vec);

#endif /* __GJK_SUPPORT_H__ */
