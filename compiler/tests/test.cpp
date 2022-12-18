#include "gtest/gtest.h"

class FooTest : public ::testing::Test
{
};

TEST_F(FooTest, MethodBarDoesAbc) { EXPECT_EQ(1, 1); }
