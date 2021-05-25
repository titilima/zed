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

#include <unordered_map>
#include "../string/algorithm.hpp"

namespace zed {

std::string decode_uri_component(const char *psz);
std::string decode_uri_component(const string_piece<char> &s);

std::string encode_uri_component(const char *psz);
std::string encode_uri_component(const string_piece<char> &s);

std::string url_encode(const std::unordered_map<std::string, std::string> &form_data);

std::unordered_map<std::string, std::string> url_decode(const string_piece<char> &s);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

inline void append_encoded(std::string &s, char ch)
{
    constexpr char special_chars[] = "-_.!~*'()";
    constexpr char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    if (zed::isdigit(ch) || zed::isalpha(ch) || nullptr != strchr(special_chars, ch))
    {
        s.push_back(ch);
    }
    else if (' ' == ch)
    {
        s.push_back('+');
    }
    else
    {
        unsigned char b = static_cast<unsigned char>(ch);
        s.push_back('%');
        s.push_back(hex_digits[(b >> 4)]);
        s.push_back(hex_digits[(b & 0xf)]);
    }
}

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

template <class CharIterator>
std::string encode_uri_component(CharIterator &it)
{
    std::string ret;
    while (!it.reach_the_end())
    {
        append_encoded(ret, *it);
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

inline std::string encode_uri_component(const char *psz)
{
    detail::char_iterator it(psz);
    return detail::encode_uri_component(it);
}

inline std::string encode_uri_component(const string_piece<char> &s)
{
    detail::char_iterator it(s);
    return detail::encode_uri_component(it);
}

inline std::unordered_map<std::string, std::string> url_decode(const string_piece<char> &s)
{
    std::unordered_map<std::string, std::string> ret;
    auto pairs = split(s, "&");
    for (const auto &pair : pairs)
    {
        std::string k, v;

        size_t p = pair.find('=');
        if (string_piece<char>::npos != p)
        {
            k = decode_uri_component(pair.substr(0, p));
            v = decode_uri_component(pair.substr(p + 1));
        }
        else
        {
            ret.emplace(decode_uri_component(pair), std::string());
        }

        ret.emplace(k, v);
    }
    return ret;
}

inline std::string url_encode(const std::unordered_map<std::string, std::string> &form_data)
{
    std::string ret;

    for (const auto &it : form_data)
    {
        ret.append(encode_uri_component(it.first)).push_back('=');
        ret.append(encode_uri_component(it.second)).push_back('&');
    }

    if (!ret.empty())
        ret.pop_back();
    return ret;
}

} // namespace zed

#endif // ZED_NET_HTTP_CODECS_HPP
