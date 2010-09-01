#ifndef __GJK_POLYTOPE_H__
#define __GJK_POLYTOPE_H__

#include <stdlib.h>
#include "gjk/vec3.h"
#include "gjk/list.h"
#include "gjk/alloc.h"

#define GJK_PT_VERTEX 1
#define GJK_PT_EDGE   2
#define GJK_PT_FACE   3


#define __GJK_PT_EL \
    int type;           /*! type of element */ \
    double dist;        /*! distance from origin */ \
    gjk_vec3_t witness; /*! witness point of projection */ \
    gjk_list_t list;    /*! list of elements of same type */

/**
 * General polytope element.
 * Could be vertex, edge or triangle.
 */
struct _gjk_pt_el_t {
    __GJK_PT_EL
};
typedef struct _gjk_pt_el_t gjk_pt_el_t;

struct _gjk_pt_edge_t;
struct _gjk_pt_face_t;

/**
 * Polytope's vertex.
 */
struct _gjk_pt_vertex_t {
    __GJK_PT_EL

    gjk_vec3_t v;
    gjk_list_t edges; //!< List of edges
};
typedef struct _gjk_pt_vertex_t gjk_pt_vertex_t;

/**
 * Polytope's edge.
 */
struct _gjk_pt_edge_t {
    __GJK_PT_EL

    gjk_pt_vertex_t *vertex[2]; //!< Reference to vertices
    struct _gjk_pt_face_t *faces[2]; //!< Reference to faces

    gjk_list_t vertex_list[2]; //!< List items in vertices' lists
};
typedef struct _gjk_pt_edge_t gjk_pt_edge_t;

/**
 * Polytope's triangle faces.
 */
struct _gjk_pt_face_t {
    __GJK_PT_EL

    gjk_pt_edge_t *edge[3]; //!< Reference to surrounding edges
};
typedef struct _gjk_pt_face_t gjk_pt_face_t;


/**
 * Struct containing polytope.
 */
struct _gjk_pt_t {
    gjk_list_t vertices; //!< List of vertices
    gjk_list_t edges; //!< List of edges
    gjk_list_t faces; //!< List of faces
};
typedef struct _gjk_pt_t gjk_pt_t;


void gjkPtInit(gjk_pt_t *pt);
void gjkPtDestroy(gjk_pt_t *pt);

/**
 * Adds vertex to polytope and returns pointer to newly created vertex.
 */
gjk_pt_vertex_t *gjkPtAddVertex(gjk_pt_t *pt, const gjk_vec3_t *v);
_gjk_inline gjk_pt_vertex_t *gjkPtAddVertexCoords(gjk_pt_t *pt,
                                                  double x, double y, double z);

/**
 * Adds edge to polytope.
 */
gjk_pt_edge_t *gjkPtAddEdge(gjk_pt_t *pt, gjk_pt_vertex_t *v1,
                                          gjk_pt_vertex_t *v2);

/**
 * Adds face to polytope.
 */
gjk_pt_face_t *gjkPtAddFace(gjk_pt_t *pt, gjk_pt_edge_t *e1,
                                          gjk_pt_edge_t *e2,
                                          gjk_pt_edge_t *e3);

/**
 * Deletes vertex from polytope.
 * Returns 0 on success, -1 otherwise.
 */
_gjk_inline int gjkPtDelVertex(gjk_pt_t *pt, gjk_pt_vertex_t *);
_gjk_inline int gjkPtDelEdge(gjk_pt_t *pt, gjk_pt_edge_t *);
_gjk_inline int gjkPtDelFace(gjk_pt_t *pt, gjk_pt_face_t *);


/**
 * Recompute distances from origin for all elements in pt.
 */
void gjkPtRecomputeDistances(gjk_pt_t *pt);

/**
 * Returns nearest element to origin.
 */
gjk_pt_el_t *gjkPtNearest(gjk_pt_t *pt);



/**** INLINES ****/
_gjk_inline gjk_pt_vertex_t *gjkPtAddVertexCoords(gjk_pt_t *pt,
                                                  double x, double y, double z)
{
    GJK_VEC3(v, x, y, z);
    return gjkPtAddVertex(pt, &v);
}

_gjk_inline int gjkPtDelVertex(gjk_pt_t *pt, gjk_pt_vertex_t *v)
{
    // test if any edge is connected to this vertex
    if (!gjkListEmpty(&v->edges))
        return -1;

    // delete vertex from main list
    gjkListDel(&v->list);

    free(v);
    return 0;
}

_gjk_inline int gjkPtDelEdge(gjk_pt_t *pt, gjk_pt_edge_t *e)
{
    // text if any face is connected to this edge (faces[] is always
    // aligned to lower indices)
    if (e->faces[0] != NULL)
        return -1;

    // disconnect edge from lists of edges in vertex struct
    gjkListDel(&e->vertex_list[0]);
    gjkListDel(&e->vertex_list[1]);

    // disconnect edge from main list
    gjkListDel(&e->list);

    free(e);
    return 0;
}

_gjk_inline int gjkPtDelFace(gjk_pt_t *pt, gjk_pt_face_t *f)
{
    gjk_pt_edge_t *e;
    size_t i;

    // remove face from edges' recerence lists
    for (i = 0; i < 3; i++){
        e = f->edge[i];
        if (e->faces[0] == f){
            e->faces[0] = e->faces[1];
        }
        e->faces[1] = NULL;
    }

    // remove face from list of all faces
    gjkListDel(&f->list);

    free(f);
    return 0;
}












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
