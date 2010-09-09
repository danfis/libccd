#include "common.h"
#include "../vec3.h"
#include "../quat.h"
#include "../gjk_support.h"

static void svtCyl(gjk_cyl_t *c, FILE *out, const char *color, const char *name)
{
    gjk_vec3_t v[32];
    gjk_quat_t rot;
    gjk_vec3_t axis, vpos, vpos2;
    double angle, x, y;
    size_t i;

    gjkVec3Set(&axis, 0., 0., 1.);
    gjkVec3Set(&vpos, 0., c->radius, 0.);
    angle = 0.;
    for (i = 0; i < 16; i++){
        angle = (double)i * (2. * M_PI / 16.);

        gjkQuatSetAngleAxis(&rot, angle, &axis);
        gjkVec3Copy(&vpos2, &vpos);
        gjkQuatRotVec(&vpos2, &rot);
        x = gjkVec3X(&vpos2);
        y = gjkVec3Y(&vpos2);

        gjkVec3Set(&v[i], x, y, c->height / 2.);
        gjkVec3Set(&v[i + 16], x, y, -c->height / 2.);
    }

    for (i = 0; i < 32; i++){
        gjkQuatRotVec(&v[i], &c->quat);
        gjkVec3Add(&v[i], &c->pos);
    }

    fprintf(out, "-----\n");
    if (name)
        fprintf(out, "Name: %s\n", name);

    fprintf(out, "Face color: %s\n", color);
    fprintf(out, "Edge color: %s\n", color);
    fprintf(out, "Point color: %s\n", color);
    fprintf(out, "Points:\n");
    for (i = 0; i < 32; i++){
        fprintf(out, "%lf %lf %lf\n", gjkVec3X(&v[i]), gjkVec3Y(&v[i]), gjkVec3Z(&v[i]));
    }

    fprintf(out, "Edges:\n");
    fprintf(out, "0 16\n");
    fprintf(out, "0 31\n");
    for (i = 1; i < 16; i++){
        fprintf(out, "0 %d\n", i);
        fprintf(out, "16 %d\n", i + 16);
        if (i != 0){
            fprintf(out, "%d %d\n", i - 1, i);
            fprintf(out, "%d %d\n", i + 16 - 1, i + 16);
        }

        fprintf(out, "%d %d\n", i, i + 16);
        fprintf(out, "%d %d\n", i, i + 16 - 1);
    }

    fprintf(out, "Faces:\n");
    for (i = 2; i < 16; i++){
        fprintf(out, "0 %d %d\n", i, i -1);
        fprintf(out, "16 %d %d\n", i + 16, i + 16 -1);

    }
    fprintf(out, "0 16 31\n");
    fprintf(out, "0 31 15\n");
    for (i = 1; i < 16; i++){
        fprintf(out, "%d %d %d\n", i, i + 16, i + 16 - 1);
        fprintf(out, "%d %d %d\n", i, i + 16 - 1, i - 1);
    }
    fprintf(out, "-----\n");
}

static void svtBox(gjk_box_t *b, FILE *out, const char *color, const char *name)
{
    gjk_vec3_t v[8];
    size_t i;

    gjkVec3Set(&v[0], b->x * 0.5, b->y * 0.5, b->z * 0.5);
    gjkVec3Set(&v[1], b->x * 0.5, b->y * -0.5, b->z * 0.5);
    gjkVec3Set(&v[2], b->x * 0.5, b->y * 0.5, b->z * -0.5);
    gjkVec3Set(&v[3], b->x * 0.5, b->y * -0.5, b->z * -0.5);
    gjkVec3Set(&v[4], b->x * -0.5, b->y * 0.5, b->z * 0.5);
    gjkVec3Set(&v[5], b->x * -0.5, b->y * -0.5, b->z * 0.5);
    gjkVec3Set(&v[6], b->x * -0.5, b->y * 0.5, b->z * -0.5);
    gjkVec3Set(&v[7], b->x * -0.5, b->y * -0.5, b->z * -0.5);

    for (i = 0; i < 8; i++){
        gjkQuatRotVec(&v[i], &b->quat);
        gjkVec3Add(&v[i], &b->pos);
    }

    fprintf(out, "-----\n");
    if (name)
        fprintf(out, "Name: %s\n", name);
    fprintf(out, "Face color: %s\n", color);
    fprintf(out, "Edge color: %s\n", color);
    fprintf(out, "Point color: %s\n", color);
    fprintf(out, "Points:\n");
    for (i = 0; i < 8; i++){
        fprintf(out, "%lf %lf %lf\n", gjkVec3X(&v[i]), gjkVec3Y(&v[i]), gjkVec3Z(&v[i]));
    }

    fprintf(out, "Edges:\n");
    fprintf(out, "0 1\n 0 2\n2 3\n3 1\n1 2\n6 2\n1 7\n1 5\n");
    fprintf(out, "5 0\n0 4\n4 2\n6 4\n6 5\n5 7\n6 7\n7 2\n7 3\n4 5\n");

    fprintf(out, "Faces:\n");
    fprintf(out, "0 2 1\n1 2 3\n6 2 4\n4 2 0\n4 0 5\n5 0 1\n");
    fprintf(out, "5 1 7\n7 1 3\n6 4 5\n6 5 7\n2 6 7\n2 7 3\n");
    fprintf(out, "-----\n");
}


void svtObj(void *_o, FILE *out, const char *color, const char *name)
{
    gjk_obj_t *o = (gjk_obj_t *)_o;

    if (o->type == GJK_OBJ_CYL){
        svtCyl((gjk_cyl_t *)o, out, color, name);
    }else if (o->type == GJK_OBJ_BOX){
        svtBox((gjk_box_t *)o, out, color, name);
    }
}

void svtObjPen(void *o1, void *o2,
               FILE *out, const char *name,
               double depth, const gjk_vec3_t *dir, const gjk_vec3_t *pos)
{
    gjk_vec3_t sep;
    char oname[500];

    gjkVec3Copy(&sep, dir);
    gjkVec3Scale(&sep, depth);
    gjkVec3Add(&sep, pos);

    fprintf(out, "------\n");
    if (name)
        fprintf(out, "Name: %s\n", name);
    fprintf(out, "Point color: 0.1 0.1 0.9\n");
    fprintf(out, "Points:\n%lf %lf %lf\n", gjkVec3X(pos), gjkVec3Y(pos), gjkVec3Z(pos));
    fprintf(out, "------\n");
    fprintf(out, "Point color: 0.1 0.9 0.9\n");
    fprintf(out, "Edge color: 0.1 0.9 0.9\n");
    fprintf(out, "Points:\n%lf %lf %lf\n", gjkVec3X(pos), gjkVec3Y(pos), gjkVec3Z(pos));
    fprintf(out, "%lf %lf %lf\n", gjkVec3X(&sep), gjkVec3Y(&sep), gjkVec3Z(&sep));
    fprintf(out, "Edges: 0 1\n");

    oname[0] = 0x0;
    if (name)
        sprintf(oname, "%s o1", name);
    svtObj(o1, out, "0.9 0.1 0.1", oname);

    oname[0] = 0x0;
    if (name)
        sprintf(oname, "%s o1", name);
    svtObj(o2, out, "0.1 0.9 0.1", oname);
}
