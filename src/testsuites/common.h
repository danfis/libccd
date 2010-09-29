#ifndef TEST_COMMON
#define TEST_COMMON

#include <stdio.h>
#include <gjk/vec3.h>

void svtObj(void *o, FILE *out, const char *color, const char *name);
void svtObjPen(void *o1, void *o2,
               FILE *out, const char *name,
               gjk_real_t depth, const gjk_vec3_t *dir, const gjk_vec3_t *pos);
void recPen(gjk_real_t depth, const gjk_vec3_t *dir, const gjk_vec3_t *pos,
            FILE *out, const char *note);

#endif
