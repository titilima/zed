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
#include "../string.hpp"

namespace zed {

std::string decode_uri_component(const char *psz);
std::string decode_uri_component(const string_piece<char> &s);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

template <class CharIterator>
std::string decode_uri_component(CharIterator &it)
{
    std::string ret;
    while (!it.reach_the_end())
    {
        char ch = *it;
        if ('%' == ch)
        {
            ++it;
            if (it.reach_the_end())
            {
                ret.push_back('?');
                break;
            }

            std::string hex(1, *it);
            ++it;
            if (it.reach_the_end())
            {
                ret.push_back('?');
                break;
            }

            hex.push_back(*it);
            try
            {
                ch = std::stoi(hex, nullptr, 16);
            }
            catch (...)
            {
                ch = '?';
            }
        }
        else if ('+' == ch)
        {
            ch = ' ';
        }

        ret.push_back(ch);
        ++it;
    }
    return ret;
}

} // namespace detail

inline std::string decode_uri_component(const char *psz)
{
    detail::char_iterator it(psz);
    return detail::decode_uri_component(it);
}

inline std::string decode_uri_component(const string_piece<char> &s)
{
    detail::char_iterator it(s);
    return detail::decode_uri_component(it);
}

} // namespace zed

#endif // ZED_NET_HTTP_CODECS_HPP
