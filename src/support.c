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

#include <ccddbl/support.h>

void __ccddblSupport(const void *obj1, const void *obj2,
                  const ccddbl_vec3_t *_dir, const ccddbl_t *ccddbl,
                  ccddbl_support_t *supp)
{
    ccddbl_vec3_t dir;

    ccddblVec3Copy(&dir, _dir);

    ccddbl->support1(obj1, &dir, &supp->v1);

    ccddblVec3Scale(&dir, -CCDDBL_ONE);
    ccddbl->support2(obj2, &dir, &supp->v2);

    ccddblVec3Sub2(&supp->v, &supp->v1, &supp->v2);
}
