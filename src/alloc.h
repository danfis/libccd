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

#ifndef __GJK_ALLOC_H__
#define __GJK_ALLOC_H__

#include <stdlib.h>

/**
 * Functions and macros required for memory allocation.
 */

/* Memory allocation: */
#define __GJK_ALLOC_MEMORY(type, ptr_old, size) \
    (type *)gjkRealloc((void *)ptr_old, (size))

/** Allocate memory for one element of type.  */
#define GJK_ALLOC(type) \
    __GJK_ALLOC_MEMORY(type, NULL, sizeof(type))

/** Allocate memory for array of elements of type type.  */
#define GJK_ALLOC_ARR(type, num_elements) \
    __GJK_ALLOC_MEMORY(type, NULL, sizeof(type) * (num_elements))

#define GJK_REALLOC_ARR(ptr, type, num_elements) \
    __GJK_ALLOC_MEMORY(type, ptr, sizeof(type) * (num_elements))

void *gjkRealloc(void *ptr, size_t size);

#endif /* __GJK_ALLOC_H__ */
