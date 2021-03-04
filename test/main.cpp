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
#include "zed/parsers/ini.hpp"
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

TEST(StringTrimming, TrimsCorrectly)
{
    const char s[] = " \t Hello!\t \t";
    ASSERT_TRUE(zed::strequ(zed::trim_left(s), "Hello!\t \t"));
    ASSERT_TRUE(zed::strequ(zed::trim_right(s), " \t Hello!"));
    ASSERT_TRUE(zed::strequ(zed::trim(s), "Hello!"));
}

TEST(INIParsing, ParsesCorrectly)
{
    const char data[] = 
        "; last modified 1 April 2001 by John Doe\n"
        "[owner]\n"
        "name=John Doe\n"
        "organization=Acme Products\n"
        "\n"
        "[database]\n"
        "server=192.0.2.42 ; use IP address in case network name resolution is not working\n"
        "port=143\n"
        "file='acme payroll.dat'"
        ;

    zed::ini_data ini = zed::ini_data::parse_cstr(data);
    ASSERT_EQ(ini.get_string("owner", "name"), "John Doe");
    ASSERT_EQ(ini.get_string("database", "server"), "192.0.2.42");
    ASSERT_EQ(ini.get_int("database", "port", 0), 143);
    ASSERT_EQ(ini.get_string("database", "file"), "acme payroll.dat");
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
