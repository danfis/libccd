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

#ifndef __GJK_SUPPORT_H__
#define __GJK_SUPPORT_H__

#include <gjk/compiler.h>
#include <gjk/vec3.h>
#include <gjk/gjk.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _gjk_support_t {
    gjk_vec3_t v;  //!< Support point in minkowski sum
    gjk_vec3_t v1; //!< Support point in obj1
    gjk_vec3_t v2; //!< Support point in obj2
};
typedef struct _gjk_support_t gjk_support_t;

_gjk_inline void gjkSupportCopy(gjk_support_t *, const gjk_support_t *s);

/**
 * Computes support point of obj1 and obj2 in direction dir.
 * Support point is returned via supp.
 */
void __gjkSupport(const void *obj1, const void *obj2,
                  const gjk_vec3_t *dir, const gjk_t *gjk,
                  gjk_support_t *supp);


/**** INLINES ****/
_gjk_inline void gjkSupportCopy(gjk_support_t *d, const gjk_support_t *s)
{
    *d = *s;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_SUPPORT_H__ */
