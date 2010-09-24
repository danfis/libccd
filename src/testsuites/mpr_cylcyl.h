#ifndef MPR_CYL_CYL
#define MPR_CYL_CYL

#include <cu/cu.h>

TEST(mprCylcylAlignedX);
TEST(mprCylcylAlignedY);
TEST(mprCylcylAlignedZ);

TEST(mprCylcylPenetrationEPA);

TEST_SUITE(TSMPRCylCyl) {
    TEST_ADD(mprCylcylAlignedX),
    TEST_ADD(mprCylcylAlignedY),
    TEST_ADD(mprCylcylAlignedZ),

    //TEST_ADD(mprCylcylPenetrationEPA),

    TEST_SUITE_CLOSURE
};

#endif

