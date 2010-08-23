#ifndef BOX_BOX
#define BOX_BOX

#include <cu/cu.h>

TEST(boxboxSetUp);
TEST(boxboxTearDown);

TEST(boxboxAlignedX);

TEST_SUITE(TSBoxBox) {
    TEST_ADD(boxboxSetUp),

    TEST_ADD(boxboxAlignedX),

    TEST_ADD(boxboxTearDown),
    TEST_SUITE_CLOSURE
};

#endif
