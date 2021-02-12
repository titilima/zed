// -------------------------------------------------
// ZED Kit - Unit tests
// -------------------------------------------------
//   File Name: main.hpp
//      Author: Ziming Li
//     Created: 2021-02-13
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#include <gtest/gtest.h>
#include "zed/string/algorithm.hpp"

TEST(StringComparisons, ComparesCorrectly)
{
    ASSERT_TRUE(zed::strequ("FOO", "FOO"));
    ASSERT_FALSE(zed::strequ("FOO", "foo"));
    ASSERT_FALSE(zed::strequ("Accept", "Accept-Language"));
    ASSERT_TRUE(zed::striequ("FOO", "foo"));
    ASSERT_NE(zed::stricmp("_", "A"), zed::stricmp("_", "a"));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
