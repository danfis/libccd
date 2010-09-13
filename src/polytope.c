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

#include <stdio.h>
#include <float.h>
#include "polytope.h"


void gjkPtInit(gjk_pt_t *pt)
{
    gjkListInit(&pt->vertices);
    gjkListInit(&pt->edges);
    gjkListInit(&pt->faces);
}

void gjkPtDestroy(gjk_pt_t *pt)
{
    gjk_pt_face_t *f, *f2;
    gjk_pt_edge_t *e, *e2;
    gjk_pt_vertex_t *v, *v2;

    // first delete all faces
    gjkListForEachEntrySafe(&pt->faces, f, f2, list){
        gjkPtDelFace(pt, f);
    }

    // delete all edges
    gjkListForEachEntrySafe(&pt->edges, e, e2, list){
        gjkPtDelEdge(pt, e);
    }

    // delete all vertices
    gjkListForEachEntrySafe(&pt->vertices, v, v2, list){
        gjkPtDelVertex(pt, v);
    }
}


gjk_pt_vertex_t *gjkPtAddVertex(gjk_pt_t *pt, const gjk_support_t *v)
{
    gjk_pt_vertex_t *vert;

    vert = GJK_ALLOC(gjk_pt_vertex_t);
    vert->type = GJK_PT_VERTEX;
    gjkSupportCopy(&vert->v, v);

    vert->dist = gjkVec3Len2(&vert->v.v);
    gjkVec3Copy(&vert->witness, &vert->v.v);

    gjkListInit(&vert->edges);

    // add vertex to list
    gjkListAppend(&pt->vertices, &vert->list);

    return vert;
}

gjk_pt_edge_t *gjkPtAddEdge(gjk_pt_t *pt, gjk_pt_vertex_t *v1,
                                          gjk_pt_vertex_t *v2)
{
    const gjk_vec3_t *a, *b;
    gjk_pt_edge_t *edge;

    edge = GJK_ALLOC(gjk_pt_edge_t);
    edge->type = GJK_PT_EDGE;
    edge->vertex[0] = v1;
    edge->vertex[1] = v2;
    edge->faces[0] = edge->faces[1] = NULL;

    a = &edge->vertex[0]->v.v;
    b = &edge->vertex[1]->v.v;
    edge->dist = gjkVec3PointSegmentDist2(gjk_vec3_origin, a, b, &edge->witness);

    gjkListAppend(&edge->vertex[0]->edges, &edge->vertex_list[0]);
    gjkListAppend(&edge->vertex[1]->edges, &edge->vertex_list[1]);

    gjkListAppend(&pt->edges, &edge->list);

    return edge;
}

gjk_pt_face_t *gjkPtAddFace(gjk_pt_t *pt, gjk_pt_edge_t *e1,
                                          gjk_pt_edge_t *e2,
                                          gjk_pt_edge_t *e3)
{
    const gjk_vec3_t *a, *b, *c;
    gjk_pt_face_t *face;
    gjk_pt_edge_t *e;
    size_t i;

    face = GJK_ALLOC(gjk_pt_face_t);
    face->type = GJK_PT_FACE;
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
    face->dist = gjkVec3PointTriDist2(gjk_vec3_origin, a, b, c, &face->witness);


    for (i = 0; i < 3; i++){
        if (face->edge[i]->faces[0] == NULL){
            face->edge[i]->faces[0] = face;
        }else{
            face->edge[i]->faces[1] = face;
        }
    }

    gjkListAppend(&pt->faces, &face->list);

    return face;
}


void gjkPtRecomputeDistances(gjk_pt_t *pt)
{
    gjk_pt_vertex_t *v;
    gjk_pt_edge_t *e;
    gjk_pt_face_t *f;
    const gjk_vec3_t *a, *b, *c;
    double dist;

    gjkListForEachEntry(&pt->vertices, v, list){
        dist = gjkVec3Len2(&v->v.v);
        v->dist = dist;
        gjkVec3Copy(&v->witness, &v->v.v);
    }

    gjkListForEachEntry(&pt->edges, e, list){
        a = &e->vertex[0]->v.v;
        b = &e->vertex[1]->v.v;
        dist = gjkVec3PointSegmentDist2(gjk_vec3_origin, a, b, &e->witness);
        e->dist = dist;
    }

    gjkListForEachEntry(&pt->faces, f, list){
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

        dist = gjkVec3PointTriDist2(gjk_vec3_origin, a, b, c, &f->witness);
        f->dist = dist;
    }
}

gjk_pt_el_t *gjkPtNearest(gjk_pt_t *pt)
{
    double nearest_dist = DBL_MAX;
    gjk_pt_el_t *nearest = NULL;
    gjk_pt_vertex_t *v;
    gjk_pt_edge_t *e;
    gjk_pt_face_t *f;

    // Order of element search (face -> edge -> vertex) is important
    // because if nearest face has nearest point on its edge it is
    // necessary to return edge instead of face. If nearest element is
    // vertex it must be returned this vertex (instead of face or edge)
    // because it will terminate algorithm.

    gjkListForEachEntry(&pt->faces, f, list){
        if (f->dist < nearest_dist){
            nearest_dist = f->dist;
            nearest = (gjk_pt_el_t *)f;
        }
    }

    gjkListForEachEntry(&pt->edges, e, list){
        if (e->dist < nearest_dist || gjkEq(e->dist, nearest_dist)){
            nearest_dist = e->dist;
            nearest = (gjk_pt_el_t *)e;
        }
    }

    gjkListForEachEntry(&pt->vertices, v, list){
        if (v->dist < nearest_dist || gjkEq(v->dist, nearest_dist)){
            nearest_dist = v->dist;
            nearest = (gjk_pt_el_t *)v;
        }
    }

    return nearest;
}


void gjkPtDumpSVT(gjk_pt_t *pt, const char *fn)
{
    FILE *fout;

    fout = fopen(fn, "a");
    if (fout == NULL)
        return;

    gjkPtDumpSVT2(pt, fout);

    fclose(fout);
}

void gjkPtDumpSVT2(gjk_pt_t *pt, FILE *fout)
{
    gjk_pt_vertex_t *v, *a, *b, *c;
    gjk_pt_edge_t *e;
    gjk_pt_face_t *f;
    size_t i;

    fprintf(fout, "-----\n");

    fprintf(fout, "Points:\n");
    i = 0;
    gjkListForEachEntry(&pt->vertices, v, list){
        v->id = i++;
        fprintf(fout, "%lf %lf %lf\n",
                gjkVec3X(&v->v.v), gjkVec3Y(&v->v.v), gjkVec3Z(&v->v.v));
    }

    fprintf(fout, "Edges:\n");
    gjkListForEachEntry(&pt->edges, e, list){
        fprintf(fout, "%d %d\n", e->vertex[0]->id, e->vertex[1]->id);
    }

    fprintf(fout, "Faces:\n");
    gjkListForEachEntry(&pt->faces, f, list){
        a = f->edge[0]->vertex[0];
        b = f->edge[0]->vertex[1];
        c = f->edge[1]->vertex[0];
        if (c == a || c == b){
            c = f->edge[1]->vertex[1];
        }
        fprintf(fout, "%d %d %d\n", a->id, b->id, c->id);
    }
}
