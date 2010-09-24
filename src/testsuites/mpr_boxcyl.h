#ifndef MPR_TEST_BOXCYL_H
#define MPR_TEST_BOXCYL_H

#include <cu/cu.h>

TEST(mprBoxcylIntersect);
TEST(mprBoxcylPenEPA);

TEST_SUITE(TSMPRBoxCyl){
    TEST_ADD(mprBoxcylIntersect),
    //TEST_ADD(mprBoxcylPenEPA),

    TEST_SUITE_CLOSURE
};

#endif
