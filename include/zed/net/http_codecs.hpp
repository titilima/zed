#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: http_codecs.hpp
//      Author: Ziming Li
//     Created: 2021-02-13
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_NET_HTTP_CODECS_HPP
#define ZED_NET_HTTP_CODECS_HPP

#include <string>
#include "../string_view.hpp"

namespace zed {

std::string decode_uri_component(const std::string_view &s);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline std::string decode_uri_component(const std::string_view &s)
{
    std::string ret;

    const char *p = s.data();
    size_t left = s.length();
    while (left > 0)
    {
        char ch = *p;
        if ('%' == ch)
        {
            if (left < 3)
            {
                ret.push_back('?');
                break;
            }

            try
            {
                std::string hex(p + 1, 2);
                ch = std::stoi(hex, nullptr, 16);
            }
            catch (...)
            {
                ch = '?';
            }
            p += 2; left -= 2;
        }
        else if ('+' == ch)
        {
            ch = ' ';
        }

        ret.push_back(ch);
        ++p; --left;
    }
    return ret;
}

} // namespace zed

#endif // ZED_NET_HTTP_CODECS_HPP
