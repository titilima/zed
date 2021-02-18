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
#include "zed/net/http_codecs.hpp"
#include "zed/string/algorithm.hpp"
#include "zed/log.hpp"

TEST(HTTPDecoders, DecodesCorrectly)
{
    ASSERT_EQ(zed::decode_uri_component("https%3A%2F%2Fexample.org%2F"), "https://example.org/");
}

TEST(StringComparisons, ComparesCorrectly)
{
    ASSERT_TRUE(zed::strequ("FOO", "FOO"));
    ASSERT_FALSE(zed::strequ("FOO", "foo"));
    ASSERT_FALSE(zed::strequ("Accept", "Accept-Language"));
    ASSERT_TRUE(zed::striequ("FOO", "foo"));
    ASSERT_NE(zed::stricmp("_", "A"), zed::stricmp("_", "a"));
}

TEST(Formatters, FormatsCorrectly)
{
    ASSERT_EQ(std::string("Hello, 123!").compare(zed::log_formatter("{}, {}!", "Hello", 123)), 0);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
