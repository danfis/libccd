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

#ifndef __CCDDBL_SUPPORT_H__
#define __CCDDBL_SUPPORT_H__

#include <ccddbl/ccddbl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _ccddbl_support_t {
    ccddbl_vec3_t v;  //!< Support point in minkowski sum
    ccddbl_vec3_t v1; //!< Support point in obj1
    ccddbl_vec3_t v2; //!< Support point in obj2
};
typedef struct _ccddbl_support_t ccddbl_support_t;

_ccddbl_inline void ccddblSupportCopy(ccddbl_support_t *, const ccddbl_support_t *s);

/**
 * Computes support point of obj1 and obj2 in direction dir.
 * Support point is returned via supp.
 */
CCDDBL_EXPORT void __ccddblSupport(const void *obj1, const void *obj2,
                  const ccddbl_vec3_t *dir, const ccddbl_t *ccddbl,
                  ccddbl_support_t *supp);


/**** INLINES ****/
_ccddbl_inline void ccddblSupportCopy(ccddbl_support_t *d, const ccddbl_support_t *s)
{
    *d = *s;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_SUPPORT_H__ */
