#ifndef SAMPLE_TEST_HPP
#define SAMPLE_TEST_HPP

#include "gtest/gtest.h"

TEST(SampleSuit, SampleTest_good)
{
    ASSERT_EQ(true, true);
}

// TEST(SampleSuit, SampleTest_bad)
// {
//     ASSERT_EQ(true, false);
// }
#endif  // SAMPLE_TEST_HPP