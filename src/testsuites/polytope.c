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
    gjk_pt_t pt;
    gjk_pt_vertex_t *v[3];
    gjk_pt_edge_t *e[3];
    gjk_pt_face_t *f;
    gjk_vec3_t u;
    int res;
    size_t i;

    DBG2("------");

    gjkPtInit(&pt);
    gjkPtDestroy(&pt);


    gjkPtInit(&pt);

    gjkVec3Set(&u, -1., -1., 0.);
    v[0] = gjkPtAddVertexCoords(&pt, -1., -1., 0.);
    assertTrue(gjkVec3Eq(&u, &v[0]->v.v));

    gjkVec3Set(&u, 1., 0., 0.);
    v[1] = gjkPtAddVertexCoords(&pt, 1., 0., 0.);
    assertTrue(gjkVec3Eq(&u, &v[1]->v.v));

    gjkVec3Set(&u, 0., 0., 1.);
    v[2] = gjkPtAddVertexCoords(&pt, 0., 0., 1.);
    assertTrue(gjkVec3Eq(&u, &v[2]->v.v));

    for (i = 0; i < 3; i++){
        assertTrue(gjkEq(v[i]->dist, gjkVec3Len2(&v[i]->v.v)));
    }


    e[0] = gjkPtAddEdge(&pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(&pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(&pt, v[2], v[0]);
    for (i = 0; i < 3; i++){
        DBG("e[%d]->dist: %lf", i, e[i]->dist);
        DBG_VEC3(&e[i]->witness, "     ->witness: ");
    }

    f = gjkPtAddFace(&pt, e[0], e[1], e[2]);
    DBG("f->dist: %lf", f->dist);
    DBG_VEC3(&f->witness, "     ->witness: ");

    for (i = 0; i < 3; i++){
        res = gjkPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
        res = gjkPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    gjkPtDelFace(&pt, f);
    for (i = 0; i < 3; i++){
        res = gjkPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }
    for (i = 0; i < 3; i++){
        res = gjkPtDelEdge(&pt, e[i]);
        assertTrue(res == 0);
    }
    for (i = 0; i < 3; i++){
        res = gjkPtDelVertex(&pt, v[i]);
        assertTrue(res == 0);
    }

    v[0] = gjkPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = gjkPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = gjkPtAddVertexCoords(&pt, 0., 0., 1.);

    e[0] = gjkPtAddEdge(&pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(&pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(&pt, v[2], v[0]);

    f = gjkPtAddFace(&pt, e[0], e[1], e[2]);

    gjkPtDestroy(&pt);
}

TEST(ptCreate2)
{
    gjk_pt_t pt;
    gjk_pt_vertex_t *v[4];
    gjk_pt_edge_t *e[6];
    gjk_pt_face_t *f[4];
    gjk_vec3_t u;
    int res;
    size_t i;

    DBG2("------");

    gjkPtInit(&pt);

    gjkVec3Set(&u, -1., -1., 0.);
    v[0] = gjkPtAddVertexCoords(&pt, -1., -1., 0.);
    assertTrue(gjkVec3Eq(&u, &v[0]->v.v));

    gjkVec3Set(&u, 1., 0., 0.);
    v[1] = gjkPtAddVertexCoords(&pt, 1., 0., 0.);
    assertTrue(gjkVec3Eq(&u, &v[1]->v.v));

    gjkVec3Set(&u, 0., 0., 1.);
    v[2] = gjkPtAddVertexCoords(&pt, 0., 0., 1.);
    assertTrue(gjkVec3Eq(&u, &v[2]->v.v));

    gjkVec3Set(&u, 0., 1., 0.);
    v[3] = gjkPtAddVertexCoords(&pt, 0., 1., 0.);
    assertTrue(gjkVec3Eq(&u, &v[3]->v.v));

    for (i = 0; i < 4; i++){
        assertTrue(gjkEq(v[i]->dist, gjkVec3Len2(&v[i]->v.v)));
    }
    for (i = 0; i < 4; i++){
        DBG("v[%d]->dist: %lf", i, v[i]->dist);
        DBG_VEC3(&v[i]->witness, "     ->witness: ");
    }

    e[0] = gjkPtAddEdge(&pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(&pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(&pt, v[2], v[0]);
    e[3] = gjkPtAddEdge(&pt, v[3], v[0]);
    e[4] = gjkPtAddEdge(&pt, v[3], v[1]);
    e[5] = gjkPtAddEdge(&pt, v[3], v[2]);
    for (i = 0; i < 6; i++){
        DBG("e[%d]->dist: %lf", i, e[i]->dist);
        DBG_VEC3(&e[i]->witness, "     ->witness: ");
    }

    f[0] = gjkPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = gjkPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = gjkPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = gjkPtAddFace(&pt, e[5], e[3], e[2]);
    for (i = 0; i < 4; i++){
        DBG("f[%d]->dist: %lf", i, f[i]->dist);
        DBG_VEC3(&f[i]->witness, "     ->witness: ");
    }

    for (i = 0; i < 4; i++){
        res = gjkPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }
    for (i = 0; i < 6; i++){
        res = gjkPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    res = gjkPtDelFace(&pt, f[0]);
    for (i = 0; i < 6; i++){
        res = gjkPtDelEdge(&pt, e[i]);
        assertFalse(res == 0);
    }

    res = gjkPtDelFace(&pt, f[1]);
    assertTrue(gjkPtDelEdge(&pt, e[0]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[1]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[2]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[3]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[4]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[5]) == 0);
    for (i = 0; i < 4; i++){
        res = gjkPtDelVertex(&pt, v[i]);
        assertFalse(res == 0);
    }

    res = gjkPtDelFace(&pt, f[2]);
    assertTrue(gjkPtDelEdge(&pt, e[1]) == 0);
    assertTrue(gjkPtDelEdge(&pt, e[4]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[2]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[3]) == 0);
    assertFalse(gjkPtDelEdge(&pt, e[5]) == 0);

    assertTrue(gjkPtDelVertex(&pt, v[1]) == 0);
    assertFalse(gjkPtDelVertex(&pt, v[0]) == 0);
    assertFalse(gjkPtDelVertex(&pt, v[2]) == 0);
    assertFalse(gjkPtDelVertex(&pt, v[3]) == 0);

    res = gjkPtDelFace(&pt, f[3]);
    assertTrue(gjkPtDelEdge(&pt, e[2]) == 0);
    assertTrue(gjkPtDelEdge(&pt, e[3]) == 0);
    assertTrue(gjkPtDelEdge(&pt, e[5]) == 0);

    assertTrue(gjkPtDelVertex(&pt, v[0]) == 0);
    assertTrue(gjkPtDelVertex(&pt, v[2]) == 0);
    assertTrue(gjkPtDelVertex(&pt, v[3]) == 0);


    v[0] = gjkPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = gjkPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = gjkPtAddVertexCoords(&pt, 0., 0., 1.);
    v[3] = gjkPtAddVertexCoords(&pt, 0., 1., 0.);

    e[0] = gjkPtAddEdge(&pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(&pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(&pt, v[2], v[0]);
    e[3] = gjkPtAddEdge(&pt, v[3], v[0]);
    e[4] = gjkPtAddEdge(&pt, v[3], v[1]);
    e[5] = gjkPtAddEdge(&pt, v[3], v[2]);

    f[0] = gjkPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = gjkPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = gjkPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = gjkPtAddFace(&pt, e[5], e[3], e[2]);

    gjkPtDestroy(&pt);
}

TEST(ptNearest)
{
    gjk_pt_t pt;
    gjk_pt_vertex_t *v[4];
    gjk_pt_edge_t *e[6];
    gjk_pt_face_t *f[4];
    gjk_pt_el_t *nearest;

    DBG2("------");

    gjkPtInit(&pt);

    v[0] = gjkPtAddVertexCoords(&pt, -1., -1., 0.);
    v[1] = gjkPtAddVertexCoords(&pt, 1., 0., 0.);
    v[2] = gjkPtAddVertexCoords(&pt, 0., 0., 1.);
    v[3] = gjkPtAddVertexCoords(&pt, 0., 1., 0.);

    e[0] = gjkPtAddEdge(&pt, v[0], v[1]);
    e[1] = gjkPtAddEdge(&pt, v[1], v[2]);
    e[2] = gjkPtAddEdge(&pt, v[2], v[0]);
    e[3] = gjkPtAddEdge(&pt, v[3], v[0]);
    e[4] = gjkPtAddEdge(&pt, v[3], v[1]);
    e[5] = gjkPtAddEdge(&pt, v[3], v[2]);

    f[0] = gjkPtAddFace(&pt, e[0], e[1], e[2]);
    f[1] = gjkPtAddFace(&pt, e[3], e[4], e[0]);
    f[2] = gjkPtAddFace(&pt, e[4], e[5], e[1]);
    f[3] = gjkPtAddFace(&pt, e[5], e[3], e[2]);

    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_FACE);
    assertEquals(nearest, (gjk_pt_el_t *)f[1]);
    assertTrue(gjkPtDelFace(&pt, f[1]) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_FACE);
    assertTrue(nearest == (gjk_pt_el_t *)f[0]
                || nearest == (gjk_pt_el_t *)f[3]);
    assertTrue(gjkPtDelFace(&pt, (gjk_pt_face_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_FACE);
    assertTrue(nearest == (gjk_pt_el_t *)f[0]
                || nearest == (gjk_pt_el_t *)f[3]);
    assertTrue(gjkPtDelFace(&pt, (gjk_pt_face_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[0]
                || nearest == (gjk_pt_el_t *)e[3]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[0]
                || nearest == (gjk_pt_el_t *)e[3]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_FACE);
    assertEquals(nearest, (gjk_pt_el_t *)f[2]);
    assertTrue(gjkPtDelFace(&pt, f[2]) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[1]
                || nearest == (gjk_pt_el_t *)e[4]
                || nearest == (gjk_pt_el_t *)e[5]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[1]
                || nearest == (gjk_pt_el_t *)e[4]
                || nearest == (gjk_pt_el_t *)e[5]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[1]
                || nearest == (gjk_pt_el_t *)e[4]
                || nearest == (gjk_pt_el_t *)e[5]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_EDGE);
    assertTrue(nearest == (gjk_pt_el_t *)e[2]);
    assertTrue(gjkPtDelEdge(&pt, (gjk_pt_edge_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_VERTEX);
    assertTrue(nearest == (gjk_pt_el_t *)v[1]
                || nearest == (gjk_pt_el_t *)v[2]
                || nearest == (gjk_pt_el_t *)v[3]);
    assertTrue(gjkPtDelVertex(&pt, (gjk_pt_vertex_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_VERTEX);
    assertTrue(nearest == (gjk_pt_el_t *)v[1]
                || nearest == (gjk_pt_el_t *)v[2]
                || nearest == (gjk_pt_el_t *)v[3]);
    assertTrue(gjkPtDelVertex(&pt, (gjk_pt_vertex_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_VERTEX);
    assertTrue(nearest == (gjk_pt_el_t *)v[1]
                || nearest == (gjk_pt_el_t *)v[2]
                || nearest == (gjk_pt_el_t *)v[3]);
    assertTrue(gjkPtDelVertex(&pt, (gjk_pt_vertex_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    //DBG("nearest->type: %d", nearest->type);
    //DBG("       ->dist: %lf", nearest->dist);
    //DBG_VEC3(&nearest->witness, "       ->witness: ");
    assertEquals(nearest->type, GJK_PT_VERTEX);
    assertTrue(nearest == (gjk_pt_el_t *)v[0]);
    assertTrue(gjkPtDelVertex(&pt, (gjk_pt_vertex_t *)nearest) == 0);


    nearest = gjkPtNearest(&pt);
    assertTrue(nearest == NULL);

    gjkPtDestroy(&pt);
}
