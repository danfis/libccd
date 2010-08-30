#ifndef __GJK_POLYTOPE_H__
#define __GJK_POLYTOPE_H__

#include <stdlib.h>
#include "gjk/vec3.h"

struct _gjk_polytope_tri_t {
    gjk_vec3_t v[3];
    double dist;
    gjk_vec3_t witness;

    struct _gjk_polytope_tri_t *next, *prev;
};
typedef struct _gjk_polytope_tri_t gjk_polytope_tri_t;


struct _gjk_polytope_t {
    gjk_polytope_tri_t *head;
};
typedef struct _gjk_polytope_t gjk_polytope_t;


_gjk_inline void gjkPolytopeInit(gjk_polytope_t *pt);
_gjk_inline void gjkPolytopeDestroy(gjk_polytope_t *pt);


_gjk_inline gjk_polytope_tri_t *gjkPolytopeAddTri(gjk_polytope_t *pt,
                                    const gjk_vec3_t *a, const gjk_vec3_t *b,
                                    const gjk_vec3_t *c,
                                    double dist, const gjk_vec3_t *witness);

_gjk_inline void gjkPolytopeDelTri(gjk_polytope_t *pt, gjk_polytope_tri_t *tri);


/**
 * Get nearest triangle from polytope.
 */
_gjk_inline gjk_polytope_tri_t *gjkPolytopeNearest(gjk_polytope_t *pt);


/**** INLINES ****/
_gjk_inline void gjkPolytopeInit(gjk_polytope_t *pt)
{
    pt->head = NULL;
}

_gjk_inline void gjkPolytopeDestroy(gjk_polytope_t *pt)
{
    gjk_polytope_tri_t *cur, *next;

    cur = pt->head;
    while (cur){
        next = cur->next;
        free(cur);
        cur = next;
    }
    pt->head = NULL;
}

_gjk_inline gjk_polytope_tri_t *gjkPolytopeAddTri(gjk_polytope_t *pt,
                                    const gjk_vec3_t *a, const gjk_vec3_t *b,
                                    const gjk_vec3_t *c,
                                    double dist, const gjk_vec3_t *witness)
{
    gjk_polytope_tri_t *tri;

    tri = (gjk_polytope_tri_t *)malloc(sizeof(gjk_polytope_tri_t));

    // copy vertices
    gjkVec3Copy(&tri->v[0], a);
    gjkVec3Copy(&tri->v[1], b);
    gjkVec3Copy(&tri->v[2], c);

    // set distance and witness point
    tri->dist = dist;
    gjkVec3Copy(&tri->witness, witness);

    // connect it to list
    tri->next = pt->head;
    if (pt->head)
        pt->head->prev = tri;
    tri->prev = NULL;
    pt->head = tri;

    return tri;
}

_gjk_inline void gjkPolytopeDelTri(gjk_polytope_t *pt, gjk_polytope_tri_t *tri)
{
    if (tri == pt->head){
        pt->head = tri->next;
        if (pt->head)
            pt->head->prev = NULL;
        free(tri);
    }else{
        tri->prev->next = tri->next;
        if (tri->next)
            tri->next->prev = tri->prev;
        free(tri);
    }
}

_gjk_inline gjk_polytope_tri_t *gjkPolytopeNearest(gjk_polytope_t *pt)
{
    gjk_polytope_tri_t *cur, *best;

    best = cur = pt->head;

    if (cur)
        cur = cur->next;
    while (cur){
        if (cur->dist < best->dist)
            best = cur;
        cur = cur->next;
    }

    return best;
}

#endif /* __GJK_POLYTOPE_H__ */
