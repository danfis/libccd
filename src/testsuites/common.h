#ifndef TEST_COMMON
#define TEST_COMMON

#include <stdio.h>
#include <ccddbl/vec3.h>

void svtObj(void *o, FILE *out, const char *color, const char *name);
void svtObjPen(void *o1, void *o2,
               FILE *out, const char *name,
               ccddbl_real_t depth, const ccddbl_vec3_t *dir, const ccddbl_vec3_t *pos);
void recPen(ccddbl_real_t depth, const ccddbl_vec3_t *dir, const ccddbl_vec3_t *pos,
            FILE *out, const char *note);

#endif
