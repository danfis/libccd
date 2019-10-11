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

#include <stdio.h>
#include <float.h>
#include "polytope.h"
#include "alloc.h"

_ccddbl_inline void _ccddblPtNearestUpdate(ccddbl_pt_t *pt, ccddbl_pt_el_t *el)
{
    if (ccddblEq(pt->nearest_dist, el->dist)){
        if (el->type < pt->nearest_type){
            pt->nearest = el;
            pt->nearest_dist = el->dist;
            pt->nearest_type = el->type;
        }
    }else if (el->dist < pt->nearest_dist){
        pt->nearest = el;
        pt->nearest_dist = el->dist;
        pt->nearest_type = el->type;
    }
}

static void _ccddblPtNearestRenew(ccddbl_pt_t *pt)
{
    ccddbl_pt_vertex_t *v;
    ccddbl_pt_edge_t *e;
    ccddbl_pt_face_t *f;

    pt->nearest_dist = CCDDBL_REAL_MAX;
    pt->nearest_type = 3;
    pt->nearest = NULL;

    ccddblListForEachEntry(&pt->vertices, v, ccddbl_pt_vertex_t, list){
        _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)v);
    }

    ccddblListForEachEntry(&pt->edges, e, ccddbl_pt_edge_t, list){
        _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)e);
    }

    ccddblListForEachEntry(&pt->faces, f, ccddbl_pt_face_t, list){
        _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)f);
    }
}



void ccddblPtInit(ccddbl_pt_t *pt)
{
    ccddblListInit(&pt->vertices);
    ccddblListInit(&pt->edges);
    ccddblListInit(&pt->faces);

    pt->nearest = NULL;
    pt->nearest_dist = CCDDBL_REAL_MAX;
    pt->nearest_type = 3;
}

void ccddblPtDestroy(ccddbl_pt_t *pt)
{
    ccddbl_pt_face_t *f, *f2;
    ccddbl_pt_edge_t *e, *e2;
    ccddbl_pt_vertex_t *v, *v2;

    // first delete all faces
    ccddblListForEachEntrySafe(&pt->faces, f, ccddbl_pt_face_t, f2, ccddbl_pt_face_t, list){
        ccddblPtDelFace(pt, f);
    }

    // delete all edges
    ccddblListForEachEntrySafe(&pt->edges, e, ccddbl_pt_edge_t, e2, ccddbl_pt_edge_t, list){
        ccddblPtDelEdge(pt, e);
    }

    // delete all vertices
    ccddblListForEachEntrySafe(&pt->vertices, v, ccddbl_pt_vertex_t, v2, ccddbl_pt_vertex_t, list){
        ccddblPtDelVertex(pt, v);
    }
}


ccddbl_pt_vertex_t *ccddblPtAddVertex(ccddbl_pt_t *pt, const ccddbl_support_t *v)
{
    ccddbl_pt_vertex_t *vert;

    vert = CCDDBL_ALLOC(ccddbl_pt_vertex_t);
    if (vert == NULL)
        return NULL;

    vert->type = CCDDBL_PT_VERTEX;
    ccddblSupportCopy(&vert->v, v);

    vert->dist = ccddblVec3Len2(&vert->v.v);
    ccddblVec3Copy(&vert->witness, &vert->v.v);

    ccddblListInit(&vert->edges);

    // add vertex to list
    ccddblListAppend(&pt->vertices, &vert->list);

    // update position in .nearest array
    _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)vert);

    return vert;
}

ccddbl_pt_edge_t *ccddblPtAddEdge(ccddbl_pt_t *pt, ccddbl_pt_vertex_t *v1,
                                          ccddbl_pt_vertex_t *v2)
{
    const ccddbl_vec3_t *a, *b;
    ccddbl_pt_edge_t *edge;

    if (v1 == NULL || v2 == NULL)
        return NULL;

    edge = CCDDBL_ALLOC(ccddbl_pt_edge_t);
    if (edge == NULL)
        return NULL;

    edge->type = CCDDBL_PT_EDGE;
    edge->vertex[0] = v1;
    edge->vertex[1] = v2;
    edge->faces[0] = edge->faces[1] = NULL;

    a = &edge->vertex[0]->v.v;
    b = &edge->vertex[1]->v.v;
    edge->dist = ccddblVec3PointSegmentDist2(ccddbl_vec3_origin, a, b, &edge->witness);

    ccddblListAppend(&edge->vertex[0]->edges, &edge->vertex_list[0]);
    ccddblListAppend(&edge->vertex[1]->edges, &edge->vertex_list[1]);

    ccddblListAppend(&pt->edges, &edge->list);

    // update position in .nearest array
    _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)edge);

    return edge;
}

ccddbl_pt_face_t *ccddblPtAddFace(ccddbl_pt_t *pt, ccddbl_pt_edge_t *e1,
                                          ccddbl_pt_edge_t *e2,
                                          ccddbl_pt_edge_t *e3)
{
    const ccddbl_vec3_t *a, *b, *c;
    ccddbl_pt_face_t *face;
    ccddbl_pt_edge_t *e;
    size_t i;

    if (e1 == NULL || e2 == NULL || e3 == NULL)
        return NULL;

    face = CCDDBL_ALLOC(ccddbl_pt_face_t);
    if (face == NULL)
        return NULL;

    face->type = CCDDBL_PT_FACE;
    face->edge[0] = e1;
    face->edge[1] = e2;
    face->edge[2] = e3;

    // obtain triplet of vertices
    a = &face->edge[0]->vertex[0]->v.v;
    b = &face->edge[0]->vertex[1]->v.v;
    e = face->edge[1];
    if (e->vertex[0] != face->edge[0]->vertex[0]
            && e->vertex[0] != face->edge[0]->vertex[1]){
        c = &e->vertex[0]->v.v;
    }else{
        c = &e->vertex[1]->v.v;
    }
    face->dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, a, b, c, &face->witness);


    for (i = 0; i < 3; i++){
        if (face->edge[i]->faces[0] == NULL){
            face->edge[i]->faces[0] = face;
        }else{
            face->edge[i]->faces[1] = face;
        }
    }

    ccddblListAppend(&pt->faces, &face->list);

    // update position in .nearest array
    _ccddblPtNearestUpdate(pt, (ccddbl_pt_el_t *)face);

    return face;
}


void ccddblPtRecomputeDistances(ccddbl_pt_t *pt)
{
    ccddbl_pt_vertex_t *v;
    ccddbl_pt_edge_t *e;
    ccddbl_pt_face_t *f;
    const ccddbl_vec3_t *a, *b, *c;
    ccddbl_real_t dist;

    ccddblListForEachEntry(&pt->vertices, v, ccddbl_pt_vertex_t, list){
        dist = ccddblVec3Len2(&v->v.v);
        v->dist = dist;
        ccddblVec3Copy(&v->witness, &v->v.v);
    }

    ccddblListForEachEntry(&pt->edges, e, ccddbl_pt_edge_t, list){
        a = &e->vertex[0]->v.v;
        b = &e->vertex[1]->v.v;
        dist = ccddblVec3PointSegmentDist2(ccddbl_vec3_origin, a, b, &e->witness);
        e->dist = dist;
    }

    ccddblListForEachEntry(&pt->faces, f, ccddbl_pt_face_t, list){
        // obtain triplet of vertices
        a = &f->edge[0]->vertex[0]->v.v;
        b = &f->edge[0]->vertex[1]->v.v;
        e = f->edge[1];
        if (e->vertex[0] != f->edge[0]->vertex[0]
                && e->vertex[0] != f->edge[0]->vertex[1]){
            c = &e->vertex[0]->v.v;
        }else{
            c = &e->vertex[1]->v.v;
        }

        dist = ccddblVec3PointTriDist2(ccddbl_vec3_origin, a, b, c, &f->witness);
        f->dist = dist;
    }
}

ccddbl_pt_el_t *ccddblPtNearest(ccddbl_pt_t *pt)
{
    if (!pt->nearest){
        _ccddblPtNearestRenew(pt);
    }
    return pt->nearest;
}


void ccddblPtDumpSVT(ccddbl_pt_t *pt, const char *fn)
{
    FILE *fout;

    fout = fopen(fn, "a");
    if (fout == NULL)
        return;

    ccddblPtDumpSVT2(pt, fout);

    fclose(fout);
}

void ccddblPtDumpSVT2(ccddbl_pt_t *pt, FILE *fout)
{
    ccddbl_pt_vertex_t *v, *a, *b, *c;
    ccddbl_pt_edge_t *e;
    ccddbl_pt_face_t *f;
    size_t i;

    fprintf(fout, "-----\n");

    fprintf(fout, "Points:\n");
    i = 0;
    ccddblListForEachEntry(&pt->vertices, v, ccddbl_pt_vertex_t, list){
        v->id = i++;
        fprintf(fout, "%lf %lf %lf\n",
                ccddblVec3X(&v->v.v), ccddblVec3Y(&v->v.v), ccddblVec3Z(&v->v.v));
    }

    fprintf(fout, "Edges:\n");
    ccddblListForEachEntry(&pt->edges, e, ccddbl_pt_edge_t, list){
        fprintf(fout, "%d %d\n", e->vertex[0]->id, e->vertex[1]->id);
    }

    fprintf(fout, "Faces:\n");
    ccddblListForEachEntry(&pt->faces, f, ccddbl_pt_face_t, list){
        a = f->edge[0]->vertex[0];
        b = f->edge[0]->vertex[1];
        c = f->edge[1]->vertex[0];
        if (c == a || c == b){
            c = f->edge[1]->vertex[1];
        }
        fprintf(fout, "%d %d %d\n", a->id, b->id, c->id);
    }
}
