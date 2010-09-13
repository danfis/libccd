#ifndef TEST_BOXCYL_H
#define TEST_BOXCYL_H

#include <cu/cu.h>

TEST(boxcylPenEPA);

TEST_SUITE(TSBoxCyl){
    TEST_ADD(boxcylPenEPA),

    TEST_SUITE_CLOSURE
};

#endif
