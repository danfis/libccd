//#undef NDEBUG
#include <cu/cu.h>
#include "../polytope.h"
#include "../dbg.h"

TEST(ptSetUp)
{
}

TEST(ptTearDown)
{
}


TEST(ptCreate1)
{
    ccddbl_pt_t pt;
    ccddbl_pt_vertex_t *v[3];
    ccddbl_pt_edge_t *e[3];
    ccddbl_pt_face_t *f;
    ccddbl_vec3_t u;
    int res, i;

    DBG2("------");

    ccddblPtInit(&pt);
    ccddblPtDestroy(&pt);


    ccddblPtInit(&pt);

    ccddblVec3Set(&u, -1., -1., 0.);
    v[0] = ccddblPtAddVertexCoords(&pt, -1., -1., 0.);
    assertTrue(ccddblVec3Eq(&u, &v[0]->v.v));

    ccddblVec3Set(&u, 1., 0., 0.);
    v[1] = ccddblPtAddVertexCoords(&pt, 1., 0., 0.);
    assertTrue(ccddblVec3Eq(&u, &v[1]->v.v));

    ccddblVec3Set(&u, 0., 0., 1.);
    v[2] = ccddblPtAddVertexCoords(&pt, 0., 0., 1.);
    assertTrue(ccddblVec3Eq(&u, &v[2]->v.v));

    for (i = 0; i < 3; i++){
        assertTrue(ccddblEq(v[i]->dist, ccddblVec3Len2(&v[i]->v.v)));
    }


    e[0] = ccddblPtAddEdge(&pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(&pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(&pt, v[2], v[0]);
    for (i = 0; i < 3; i++){
        DBG("e[%d]->dist: %lf", i, e[i]->dist);
        DBG_VEC3(&e[i]->witness, "     ->witness: ");
    }

    f = ccddblPtAddFace(&pt, e[0], e[1], e[2]);
    DBG("f->dist: %lf", f->dist);
    DBG_VEC3(&f->witness, "     ->witness: ");

    for (i = 0; i < 3; i++){
        res = ccddblPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
        res = ccddblPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    ccddblPtDelFace(&pt, f);
    for (i = 0; i < 3; i++){
        res = ccddblPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }
    for (i = 0; i < 3; i++){
        res = ccddblPtDelEdge(&pt, e[i]);
        assertTrue(res == 0);
    }
    for (i = 0; i < 3; i++){
        res = ccddblPtDelVertex(&pt, v[i]);
        assertTrue(res == 0);
    }

    v[0] = ccddblPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = ccddblPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = ccddblPtAddVertexCoords(&pt, 0., 0., 1.);

    e[0] = ccddblPtAddEdge(&pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(&pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(&pt, v[2], v[0]);

    f = ccddblPtAddFace(&pt, e[0], e[1], e[2]);

    ccddblPtDestroy(&pt);
}

TEST(ptCreate2)
{
    ccddbl_pt_t pt;
    ccddbl_pt_vertex_t *v[4];
    ccddbl_pt_edge_t *e[6];
    ccddbl_pt_face_t *f[4];
    ccddbl_vec3_t u;
    int res, i;

    DBG2("------");

    ccddblPtInit(&pt);

    ccddblVec3Set(&u, -1., -1., 0.);
    v[0] = ccddblPtAddVertexCoords(&pt, -1., -1., 0.);
    assertTrue(ccddblVec3Eq(&u, &v[0]->v.v));

    ccddblVec3Set(&u, 1., 0., 0.);
    v[1] = ccddblPtAddVertexCoords(&pt, 1., 0., 0.);
    assertTrue(ccddblVec3Eq(&u, &v[1]->v.v));

    ccddblVec3Set(&u, 0., 0., 1.);
    v[2] = ccddblPtAddVertexCoords(&pt, 0., 0., 1.);
    assertTrue(ccddblVec3Eq(&u, &v[2]->v.v));

    ccddblVec3Set(&u, 0., 1., 0.);
    v[3] = ccddblPtAddVertexCoords(&pt, 0., 1., 0.);
    assertTrue(ccddblVec3Eq(&u, &v[3]->v.v));

    for (i = 0; i < 4; i++){
        assertTrue(ccddblEq(v[i]->dist, ccddblVec3Len2(&v[i]->v.v)));
    }
    for (i = 0; i < 4; i++){
        DBG("v[%d]->dist: %lf", i, v[i]->dist);
        DBG_VEC3(&v[i]->witness, "     ->witness: ");
    }

    e[0] = ccddblPtAddEdge(&pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(&pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(&pt, v[2], v[0]);
    e[3] = ccddblPtAddEdge(&pt, v[3], v[0]);
    e[4] = ccddblPtAddEdge(&pt, v[3], v[1]);
    e[5] = ccddblPtAddEdge(&pt, v[3], v[2]);
    for (i = 0; i < 6; i++){
        DBG("e[%d]->dist: %lf", i, e[i]->dist);
        DBG_VEC3(&e[i]->witness, "     ->witness: ");
    }

    f[0] = ccddblPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = ccddblPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = ccddblPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = ccddblPtAddFace(&pt, e[5], e[3], e[2]);
    for (i = 0; i < 4; i++){
        DBG("f[%d]->dist: %lf", i, f[i]->dist);
        DBG_VEC3(&f[i]->witness, "     ->witness: ");
    }

    for (i = 0; i < 4; i++){
        res = ccddblPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }
    for (i = 0; i < 6; i++){
        res = ccddblPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    res = ccddblPtDelFace(&pt, f[0]);
    for (i = 0; i < 6; i++){
        res = ccddblPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    res = ccddblPtDelFace(&pt, f[1]);
    assertTrue(ccddblPtDelEdge(&pt, e[0]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[1]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[2]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[3]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[4]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[5]) == 0);
    for (i = 0; i < 4; i++){
        res = ccddblPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }

    res = ccddblPtDelFace(&pt, f[2]);
    assertTrue(ccddblPtDelEdge(&pt, e[1]) == 0);
    assertTrue(ccddblPtDelEdge(&pt, e[4]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[2]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[3]) == 0);
    assertFalse(ccddblPtDelEdge(&pt, e[5]) == 0);

    assertTrue(ccddblPtDelVertex(&pt, v[1]) == 0);
    assertFalse(ccddblPtDelVertex(&pt, v[0]) == 0);
    assertFalse(ccddblPtDelVertex(&pt, v[2]) == 0);
    assertFalse(ccddblPtDelVertex(&pt, v[3]) == 0);

    res = ccddblPtDelFace(&pt, f[3]);
    assertTrue(ccddblPtDelEdge(&pt, e[2]) == 0);
    assertTrue(ccddblPtDelEdge(&pt, e[3]) == 0);
    assertTrue(ccddblPtDelEdge(&pt, e[5]) == 0);

    assertTrue(ccddblPtDelVertex(&pt, v[0]) == 0);
    assertTrue(ccddblPtDelVertex(&pt, v[2]) == 0);
    assertTrue(ccddblPtDelVertex(&pt, v[3]) == 0);


    v[0] = ccddblPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = ccddblPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = ccddblPtAddVertexCoords(&pt, 0., 0., 1.);
    v[3] = ccddblPtAddVertexCoords(&pt, 0., 1., 0.);

    e[0] = ccddblPtAddEdge(&pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(&pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(&pt, v[2], v[0]);
    e[3] = ccddblPtAddEdge(&pt, v[3], v[0]);
    e[4] = ccddblPtAddEdge(&pt, v[3], v[1]);
    e[5] = ccddblPtAddEdge(&pt, v[3], v[2]);

    f[0] = ccddblPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = ccddblPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = ccddblPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = ccddblPtAddFace(&pt, e[5], e[3], e[2]);

    ccddblPtDestroy(&pt);
}

TEST(ptNearest)
{
    ccddbl_pt_t pt;
    ccddbl_pt_vertex_t *v[4];
    ccddbl_pt_edge_t *e[6];
    ccddbl_pt_face_t *f[4];
    ccddbl_pt_el_t *nearest;

    DBG2("------");

    ccddblPtInit(&pt);

    v[0] = ccddblPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = ccddblPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = ccddblPtAddVertexCoords(&pt, 0., 0., 1.);
    v[3] = ccddblPtAddVertexCoords(&pt, 0., 1., 0.);

    e[0] = ccddblPtAddEdge(&pt, v[0], v[1]);
    e[1] = ccddblPtAddEdge(&pt, v[1], v[2]);
    e[2] = ccddblPtAddEdge(&pt, v[2], v[0]);
    e[3] = ccddblPtAddEdge(&pt, v[3], v[0]);
    e[4] = ccddblPtAddEdge(&pt, v[3], v[1]);
    e[5] = ccddblPtAddEdge(&pt, v[3], v[2]);

    f[0] = ccddblPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = ccddblPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = ccddblPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = ccddblPtAddFace(&pt, e[5], e[3], e[2]);

    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_FACE);
    assertEquals(nearest, (ccddbl_pt_el_t *)f[1]);
    assertTrue(ccddblPtDelFace(&pt, f[1]) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_FACE);
    assertTrue(nearest == (ccddbl_pt_el_t *)f[0]
                || nearest == (ccddbl_pt_el_t *)f[3]);
    assertTrue(ccddblPtDelFace(&pt, (ccddbl_pt_face_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_FACE);
    assertTrue(nearest == (ccddbl_pt_el_t *)f[0]
                || nearest == (ccddbl_pt_el_t *)f[3]);
    assertTrue(ccddblPtDelFace(&pt, (ccddbl_pt_face_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[0]
                || nearest == (ccddbl_pt_el_t *)e[3]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[0]
                || nearest == (ccddbl_pt_el_t *)e[3]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_FACE);
    assertEquals(nearest, (ccddbl_pt_el_t *)f[2]);
    assertTrue(ccddblPtDelFace(&pt, f[2]) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[1]
                || nearest == (ccddbl_pt_el_t *)e[4]
                || nearest == (ccddbl_pt_el_t *)e[5]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[1]
                || nearest == (ccddbl_pt_el_t *)e[4]
                || nearest == (ccddbl_pt_el_t *)e[5]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[1]
                || nearest == (ccddbl_pt_el_t *)e[4]
                || nearest == (ccddbl_pt_el_t *)e[5]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_EDGE);
    assertTrue(nearest == (ccddbl_pt_el_t *)e[2]);
    assertTrue(ccddblPtDelEdge(&pt, (ccddbl_pt_edge_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_VERTEX);
    assertTrue(nearest == (ccddbl_pt_el_t *)v[1]
                || nearest == (ccddbl_pt_el_t *)v[2]
                || nearest == (ccddbl_pt_el_t *)v[3]);
    assertTrue(ccddblPtDelVertex(&pt, (ccddbl_pt_vertex_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_VERTEX);
    assertTrue(nearest == (ccddbl_pt_el_t *)v[1]
                || nearest == (ccddbl_pt_el_t *)v[2]
                || nearest == (ccddbl_pt_el_t *)v[3]);
    assertTrue(ccddblPtDelVertex(&pt, (ccddbl_pt_vertex_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_VERTEX);
    assertTrue(nearest == (ccddbl_pt_el_t *)v[1]
                || nearest == (ccddbl_pt_el_t *)v[2]
                || nearest == (ccddbl_pt_el_t *)v[3]);
    assertTrue(ccddblPtDelVertex(&pt, (ccddbl_pt_vertex_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, CCDDBL_PT_VERTEX);
    assertTrue(nearest == (ccddbl_pt_el_t *)v[0]);
    assertTrue(ccddblPtDelVertex(&pt, (ccddbl_pt_vertex_t *)nearest) == 0);


    nearest = ccddblPtNearest(&pt);
    assertTrue(nearest == NULL);

    ccddblPtDestroy(&pt);
}
