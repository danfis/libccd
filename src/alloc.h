#ifndef _GJK_ALLOC_H_
#define _GJK_ALLOC_H_

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

#endif /* _GJK_ALLOC_H_ */
