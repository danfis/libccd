#ifndef TEST_COMMON
#define TEST_COMMON

#include <stdio.h>
#include "../vec3.h"

void svtObj(void *o, FILE *out, const char *color, const char *name);
void svtObjPen(void *o1, void *o2,
               FILE *out, const char *name,
               double depth, const gjk_vec3_t *dir, const gjk_vec3_t *pos);

#endif
