#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: url_parser.hpp
//      Author: Ziming Li
//     Created: 2021-03-22
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_NET_URL_PARSER_HPP
#define ZED_NET_URL_PARSER_HPP

#include "../string.hpp"

namespace zed {

struct url_parts
{
    using part = string_piece<char>;
    part scheme;
    part username, password, host, port;
    part path, query, ref;
    bool is_ipv6 = false;
};

bool parse_url(const char *psz, url_parts &dst);

template <class String>
bool parse_url(const String &s, url_parts &dst);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

using parse_url_iterator = char_iterator<string_piece<char>>;

inline bool extract_scheme(parse_url_iterator &it, url_parts::part &dst)
{
    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if (':' == ch)
        {
            dst = string_piece<char>(start, len);
            return true;
        }

        ++len;
    }

    dst = url_parts::part();
    return false;
}


inline bool parse_user_info(parse_url_iterator &it, url_parts::part &username, url_parts::part &password)
{
    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if (':' == ch)
        {
            username = string_piece<char>(start, len);
            password = it.take_left_piece();
            return !password.empty();
        }

        ++len;
    }

    username = string_piece<char>(start, len);
    password = url_parts::part();
    return true;
}

inline bool parse_host(const string_piece<char> &s, url_parts::part &dst, bool &is_ipv6)
{
    ZASSERT(!s.empty());
    if ('[' == *s.data())
    {
        if (']' != *(s.data() + s.length() - 1))
            return false;

        dst = string_piece<char>(s.data() + 1, s.length() - 2);
        is_ipv6 = true;
    }
    else
    {
        dst = s;
        is_ipv6 = false;
    }
    return true;
}

inline bool parse_server_info(parse_url_iterator &it, url_parts::part &host, bool &is_ipv6, url_parts::part &port)
{
    if (it.reach_the_end())
    {
        host = url_parts::part();
        port = url_parts::part();
        return true;
    }

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if (':' == ch)
        {
            if (!parse_host(string_piece<char>(start, len), host, is_ipv6))
                return false;
            port = it.take_left_piece();
            return true;
        }

        ++len;
    }

    return parse_host(string_piece<char>(start, len), host, is_ipv6);
}

inline bool parse_authority(parse_url_iterator &it, url_parts &dst)
{
    if (it.reach_the_end())
    {
        dst.username = url_parts::part();
        dst.password = url_parts::part();
        dst.host     = url_parts::part();
        dst.port     = url_parts::part();
        return true;
    }

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if ('@' == ch)
        {
            parse_url_iterator user_info(string_piece<char>(start, len));
            if (!parse_user_info(user_info, dst.username, dst.password))
                return false;
            return parse_server_info(it, dst.host, dst.is_ipv6, dst.port);
        }

        ++len;
    }

    parse_url_iterator server_info(string_piece<char>(start, len));
    return parse_server_info(server_info, dst.host, dst.is_ipv6, dst.port);
}

inline void parse_query_and_ref(parse_url_iterator &it, url_parts::part &query, url_parts::part &ref)
{
    if (it.reach_the_end())
    {
        query = url_parts::part();
        ref   = url_parts::part();
        return;
    }

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if ('#' == ch)
        {
            if (len > 0)
                query = string_piece<char>(start, len);
            else
                query = url_parts::part();
            ref = it.take_left_piece();
            return;
        }

        ++len;
    }

    query = string_piece<char>(start, len);
}

inline void parse_path(parse_url_iterator &it, url_parts &dst)
{
    if (it.reach_the_end())
    {
        dst.path  = url_parts::part();
        dst.query = url_parts::part();
        dst.ref   = url_parts::part();
        return;
    }

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if ('?' == ch || '#' == ch)
        {
            if (len > 0)
                dst.path = string_piece<char>(start, len);
            else
                dst.path = url_parts::part();
            if ('?' == ch)
            {
                parse_query_and_ref(it, dst.query, dst.ref);
                return;
            }

            dst.query = url_parts::part();
            dst.ref   = it.take_left_piece();
            return;
        }

        ++len;
    }

    dst.path = string_piece<char>(start, len);
}

inline bool parse_after_scheme(parse_url_iterator &it, url_parts &dst)
{
    if ('/' != *it)
        return false;
    ++it;
    if ('/' != *it)
        return false;
    ++it;

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        if ('/' == ch || '?' == ch || '#' == ch)
            break;

        ++it; ++len;
    }

    parse_url_iterator authority(string_piece<char>(start, len));
    if (!parse_authority(authority, dst))
        return false;
    parse_path(it, dst);
    return true;
}

inline bool parse_url(parse_url_iterator &it, url_parts &dst)
{
    if (!extract_scheme(it, dst.scheme))
        return false;
    return parse_after_scheme(it, dst);
}

} // namespace detail

inline bool parse_url(const char *psz, url_parts &dst)
{
    string_piece<char> s(psz, strlen(psz));
    detail::char_iterator it(s);
    return detail::parse_url(it, dst);
}

template <class String>
bool parse_url(const String &s, url_parts &dst)
{
    detail::char_iterator<string_piece<char>> it(s);
    return detail::parse_url(it, dst);
}

} // namespace zed

#endif // ZED_NET_URL_PARSER_HPP
