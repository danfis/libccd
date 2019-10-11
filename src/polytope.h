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

#ifndef __CCDDBL_POLYTOPE_H__
#define __CCDDBL_POLYTOPE_H__

#include <stdlib.h>
#include <stdio.h>
#include <ccddbl/support.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CCDDBL_PT_VERTEX 1
#define CCDDBL_PT_EDGE   2
#define CCDDBL_PT_FACE   3


#define __CCDDBL_PT_EL \
    int type;           /*! type of element */ \
    ccddbl_real_t dist;        /*! distance from origin */ \
    ccddbl_vec3_t witness; /*! witness point of projection of origin */ \
    ccddbl_list_t list;    /*! list of elements of same type */

/**
 * General polytope element.
 * Could be vertex, edge or triangle.
 */
struct _ccddbl_pt_el_t {
    __CCDDBL_PT_EL
};
typedef struct _ccddbl_pt_el_t ccddbl_pt_el_t;

struct _ccddbl_pt_edge_t;
struct _ccddbl_pt_face_t;

/**
 * Polytope's vertex.
 */
struct _ccddbl_pt_vertex_t {
    __CCDDBL_PT_EL

    int id;
    ccddbl_support_t v;
    ccddbl_list_t edges; //!< List of edges
};
typedef struct _ccddbl_pt_vertex_t ccddbl_pt_vertex_t;

/**
 * Polytope's edge.
 */
struct _ccddbl_pt_edge_t {
    __CCDDBL_PT_EL

    ccddbl_pt_vertex_t *vertex[2]; //!< Reference to vertices
    struct _ccddbl_pt_face_t *faces[2]; //!< Reference to faces

    ccddbl_list_t vertex_list[2]; //!< List items in vertices' lists
};
typedef struct _ccddbl_pt_edge_t ccddbl_pt_edge_t;

/**
 * Polytope's triangle faces.
 */
struct _ccddbl_pt_face_t {
    __CCDDBL_PT_EL

    ccddbl_pt_edge_t *edge[3]; //!< Reference to surrounding edges
};
typedef struct _ccddbl_pt_face_t ccddbl_pt_face_t;


/**
 * Struct containing polytope.
 */
struct _ccddbl_pt_t {
    ccddbl_list_t vertices; //!< List of vertices
    ccddbl_list_t edges; //!< List of edges
    ccddbl_list_t faces; //!< List of faces

    ccddbl_pt_el_t *nearest;
    ccddbl_real_t nearest_dist;
    int nearest_type;
};
typedef struct _ccddbl_pt_t ccddbl_pt_t;


CCDDBL_EXPORT void ccddblPtInit(ccddbl_pt_t *pt);
CCDDBL_EXPORT void ccddblPtDestroy(ccddbl_pt_t *pt);

/**
 * Returns vertices surrounding given triangle face.
 */
_ccddbl_inline void ccddblPtFaceVec3(const ccddbl_pt_face_t *face,
                               ccddbl_vec3_t **a,
                               ccddbl_vec3_t **b,
                               ccddbl_vec3_t **c);
_ccddbl_inline void ccddblPtFaceVertices(const ccddbl_pt_face_t *face,
                                   ccddbl_pt_vertex_t **a,
                                   ccddbl_pt_vertex_t **b,
                                   ccddbl_pt_vertex_t **c);
_ccddbl_inline void ccddblPtFaceEdges(const ccddbl_pt_face_t *f,
                                ccddbl_pt_edge_t **a,
                                ccddbl_pt_edge_t **b,
                                ccddbl_pt_edge_t **c);

_ccddbl_inline void ccddblPtEdgeVec3(const ccddbl_pt_edge_t *e,
                               ccddbl_vec3_t **a,
                               ccddbl_vec3_t **b);
_ccddbl_inline void ccddblPtEdgeVertices(const ccddbl_pt_edge_t *e,
                                   ccddbl_pt_vertex_t **a,
                                   ccddbl_pt_vertex_t **b);
_ccddbl_inline void ccddblPtEdgeFaces(const ccddbl_pt_edge_t *e,
                                ccddbl_pt_face_t **f1,
                                ccddbl_pt_face_t **f2);


/**
 * Adds vertex to polytope and returns pointer to newly created vertex.
 */
CCDDBL_EXPORT ccddbl_pt_vertex_t *ccddblPtAddVertex(ccddbl_pt_t *pt, const ccddbl_support_t *v);
_ccddbl_inline ccddbl_pt_vertex_t *ccddblPtAddVertexCoords(ccddbl_pt_t *pt,
                                                  ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z);

/**
 * Adds edge to polytope.
 */
CCDDBL_EXPORT ccddbl_pt_edge_t *ccddblPtAddEdge(ccddbl_pt_t *pt, ccddbl_pt_vertex_t *v1,
                                          ccddbl_pt_vertex_t *v2);

/**
 * Adds face to polytope.
 */
CCDDBL_EXPORT ccddbl_pt_face_t *ccddblPtAddFace(ccddbl_pt_t *pt, ccddbl_pt_edge_t *e1,
                                          ccddbl_pt_edge_t *e2,
                                          ccddbl_pt_edge_t *e3);

/**
 * Deletes vertex from polytope.
 * Returns 0 on success, -1 otherwise.
 */
_ccddbl_inline int ccddblPtDelVertex(ccddbl_pt_t *pt, ccddbl_pt_vertex_t *);
_ccddbl_inline int ccddblPtDelEdge(ccddbl_pt_t *pt, ccddbl_pt_edge_t *);
_ccddbl_inline int ccddblPtDelFace(ccddbl_pt_t *pt, ccddbl_pt_face_t *);


/**
 * Recompute distances from origin for all elements in pt.
 */
CCDDBL_EXPORT void ccddblPtRecomputeDistances(ccddbl_pt_t *pt);

/**
 * Returns nearest element to origin.
 */
CCDDBL_EXPORT ccddbl_pt_el_t *ccddblPtNearest(ccddbl_pt_t *pt);


CCDDBL_EXPORT void ccddblPtDumpSVT(ccddbl_pt_t *pt, const char *fn);
CCDDBL_EXPORT void ccddblPtDumpSVT2(ccddbl_pt_t *pt, FILE *);


/**** INLINES ****/
_ccddbl_inline ccddbl_pt_vertex_t *ccddblPtAddVertexCoords(ccddbl_pt_t *pt,
                                                  ccddbl_real_t x, ccddbl_real_t y, ccddbl_real_t z)
{
    ccddbl_support_t s;
    ccddblVec3Set(&s.v, x, y, z);
    return ccddblPtAddVertex(pt, &s);
}

_ccddbl_inline int ccddblPtDelVertex(ccddbl_pt_t *pt, ccddbl_pt_vertex_t *v)
{
    // test if any edge is connected to this vertex
    if (!ccddblListEmpty(&v->edges))
        return -1;

    // delete vertex from main list
    ccddblListDel(&v->list);

    if ((void *)pt->nearest == (void *)v){
        pt->nearest = NULL;
    }

    free(v);
    return 0;
}

_ccddbl_inline int ccddblPtDelEdge(ccddbl_pt_t *pt, ccddbl_pt_edge_t *e)
{
    // text if any face is connected to this edge (faces[] is always
    // aligned to lower indices)
    if (e->faces[0] != NULL)
        return -1;

    // disconnect edge from lists of edges in vertex struct
    ccddblListDel(&e->vertex_list[0]);
    ccddblListDel(&e->vertex_list[1]);

    // disconnect edge from main list
    ccddblListDel(&e->list);

    if ((void *)pt->nearest == (void *)e){
        pt->nearest = NULL;
    }

    free(e);
    return 0;
}

_ccddbl_inline int ccddblPtDelFace(ccddbl_pt_t *pt, ccddbl_pt_face_t *f)
{
    ccddbl_pt_edge_t *e;
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
    ccddblListDel(&f->list);

    if ((void *)pt->nearest == (void *)f){
        pt->nearest = NULL;
    }

    free(f);
    return 0;
}

_ccddbl_inline void ccddblPtFaceVec3(const ccddbl_pt_face_t *face,
                               ccddbl_vec3_t **a,
                               ccddbl_vec3_t **b,
                               ccddbl_vec3_t **c)
{
    *a = &face->edge[0]->vertex[0]->v.v;
    *b = &face->edge[0]->vertex[1]->v.v;

    if (face->edge[1]->vertex[0] != face->edge[0]->vertex[0]
            && face->edge[1]->vertex[0] != face->edge[0]->vertex[1]){
        *c = &face->edge[1]->vertex[0]->v.v;
    }else{
        *c = &face->edge[1]->vertex[1]->v.v;
    }
}

_ccddbl_inline void ccddblPtFaceVertices(const ccddbl_pt_face_t *face,
                                   ccddbl_pt_vertex_t **a,
                                   ccddbl_pt_vertex_t **b,
                                   ccddbl_pt_vertex_t **c)
{
    *a = face->edge[0]->vertex[0];
    *b = face->edge[0]->vertex[1];

    if (face->edge[1]->vertex[0] != face->edge[0]->vertex[0]
            && face->edge[1]->vertex[0] != face->edge[0]->vertex[1]){
        *c = face->edge[1]->vertex[0];
    }else{
        *c = face->edge[1]->vertex[1];
    }
}

_ccddbl_inline void ccddblPtFaceEdges(const ccddbl_pt_face_t *f,
                                ccddbl_pt_edge_t **a,
                                ccddbl_pt_edge_t **b,
                                ccddbl_pt_edge_t **c)
{
    *a = f->edge[0];
    *b = f->edge[1];
    *c = f->edge[2];
}

_ccddbl_inline void ccddblPtEdgeVec3(const ccddbl_pt_edge_t *e,
                               ccddbl_vec3_t **a,
                               ccddbl_vec3_t **b)
{
    *a = &e->vertex[0]->v.v;
    *b = &e->vertex[1]->v.v;
}

_ccddbl_inline void ccddblPtEdgeVertices(const ccddbl_pt_edge_t *e,
                                   ccddbl_pt_vertex_t **a,
                                   ccddbl_pt_vertex_t **b)
{
    *a = e->vertex[0];
    *b = e->vertex[1];
}

_ccddbl_inline void ccddblPtEdgeFaces(const ccddbl_pt_edge_t *e,
                                ccddbl_pt_face_t **f1,
                                ccddbl_pt_face_t **f2)
{
    *f1 = e->faces[0];
    *f2 = e->faces[1];
}


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CCDDBL_POLYTOPE_H__ */
