#ifndef MPR_BOX_BOX
#define MPR_BOX_BOX

#include <cu/cu.h>

TEST(mprBoxboxAlignedX);
TEST(mprBoxboxAlignedY);
TEST(mprBoxboxAlignedZ);

TEST(mprBoxboxRot);

TEST(mprBoxboxSeparate);
TEST(mprBoxboxPenetration);
TEST(mprBoxboxPenetration2);

TEST_SUITE(TSMPRBoxBox) {
    TEST_ADD(mprBoxboxAlignedX),
    TEST_ADD(mprBoxboxAlignedY),
    TEST_ADD(mprBoxboxAlignedZ),
    TEST_ADD(mprBoxboxRot),
    TEST_ADD(mprBoxboxSeparate),
    TEST_ADD(mprBoxboxPenetration),
	TEST_ADD(mprBoxboxPenetration2),
    TEST_SUITE_CLOSURE
};

#endif
