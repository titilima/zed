#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: url.hpp
//      Author: Ziming Li
//     Created: 2021-03-22
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_NET_URL_PARSER_HPP
#define ZED_NET_URL_PARSER_HPP

#include <memory>
#include "../file/path.hpp"

namespace zed {

struct url_parts
{
    struct part {
        unsigned start = 0, length = 0;

        part(void) = default;
        part(unsigned s, unsigned l) : start(s), length(l) {}
    };

    part scheme;
    part username, password, host, port;
    part path, query, ref;
    bool is_ipv6 = false;

    void reset(void);
};

bool parse_url(const char *psz, url_parts &dst);

template <class String>
bool parse_url(const String &s, url_parts &dst);

class url
{
public:
    url(void) = default;
    url(const std::string &s);

    bool operator==(const url &o) const { return m_string == o.m_string; }
    bool operator!=(const url &o) const { return !(*this == o); }

    bool is_valid(void) const { return m_valid; }
    const std::string& spec(void) const { return m_string; }

    std::string get_scheme(void) const { return get_part(m_parts.scheme); }
    bool scheme_is(const char *scheme) const { return m_valid ? strequ(scheme_piece(), scheme) : false; }
    bool scheme_is_in_http_family(void) const { return scheme_is("http") || scheme_is("https"); }
    bool scheme_is_file(void) const { return scheme_is("file"); }

    std::string get_username(void) const { return get_part(m_parts.username); }
    std::string get_password(void) const { return get_part(m_parts.password); }
    std::string get_host(void) const { return get_part(m_parts.host); }
    std::string get_path(void) const { return get_part(m_parts.path); }

    url combine(const std::string &relative) const;
protected:
    const url_parts& raw_parts(void) const { return m_parts; }
private:
    std::string get_part(const url_parts::part &part) const;

    string_piece<char> piece_of(const url_parts::part &part) const;
    string_piece<char> scheme_piece(void) const { return piece_of(m_parts.scheme); }
    string_piece<char> path_piece(void) const { return piece_of(m_parts.path); }

    bool m_valid = false;
    std::string m_string;
    url_parts m_parts;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

using parse_url_iterator = char_iterator<string_piece<char>>;

class url_part_wrapper final : public string_piece<char>
{
public:
    url_part_wrapper(const char *psz) : string_piece<char>(psz, strlen(psz)) {}
    url_part_wrapper(const char *ps, size_t len) : string_piece<char>(ps, len) {}

    url_parts::part wrap(const string_piece<char> &piece) const
    {
        ZASSERT(is_sub_piece(piece));
        return url_parts::part(piece.data() - data(), piece.length());
    }
    url_parts::part wrap(const char *ps, size_t len) const
    {
        return wrap(string_piece<char>(ps, len));
    }
    string_piece<char> wrap(const url_parts::part &part) const
    {
        return string_piece<char>(data() + part.start, part.length);
    }
private:
#ifndef NDEBUG
    bool is_sub_piece(const string_piece<char> &piece) const
    {
        if (piece.data() < data())
            return false;
        if ((piece.data() + piece.length()) > (data() + length()))
            return false;
        return true;
    }
#endif
};

inline bool extract_scheme(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts::part &dst)
{
    dst = url_parts::part();

    if (it.reach_the_end())
        return false;

    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if (':' == ch)
        {
            dst = wrapper.wrap(start, len);
            return true;
        }

        ++len;
    }

    return false;
}

inline bool parse_user_info(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts::part &username, url_parts::part &password)
{
    auto start = it.data();
    size_t len = 0;
    while (!it.reach_the_end())
    {
        char ch = *it;
        ++it;

        if (':' == ch)
        {
            username = wrapper.wrap(start, len);
            password = wrapper.wrap(it.take_left_piece());
            return password.length > 0;
        }

        ++len;
    }

    username = wrapper.wrap(start, len);
    password = url_parts::part();
    return true;
}

inline bool parse_host(const url_part_wrapper &wrapper, const string_piece<char> &s, url_parts::part &dst, bool &is_ipv6)
{
    ZASSERT(!s.empty());
    if ('[' == *s.data())
    {
        if (']' != *(s.data() + s.length() - 1))
            return false;

        dst = wrapper.wrap(s.data() + 1, s.length() - 2);
        is_ipv6 = true;
    }
    else
    {
        dst = wrapper.wrap(s);
        is_ipv6 = false;
    }
    return true;
}

inline bool parse_server_info(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts::part &host, bool &is_ipv6, url_parts::part &port)
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
            if (!parse_host(wrapper, string_piece<char>(start, len), host, is_ipv6))
                return false;
            port = wrapper.wrap(it.take_left_piece());
            return true;
        }

        ++len;
    }

    return parse_host(wrapper, string_piece<char>(start, len), host, is_ipv6);
}

inline bool parse_authority(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts &dst)
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
            if (!parse_user_info(wrapper, user_info, dst.username, dst.password))
                return false;
            return parse_server_info(wrapper, it, dst.host, dst.is_ipv6, dst.port);
        }

        ++len;
    }

    parse_url_iterator server_info(string_piece<char>(start, len));
    return parse_server_info(wrapper, server_info, dst.host, dst.is_ipv6, dst.port);
}

inline void parse_query_and_ref(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts::part &query, url_parts::part &ref)
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
                query = wrapper.wrap(start, len);
            else
                query = url_parts::part();
            ref = wrapper.wrap(it.take_left_piece());
            return;
        }

        ++len;
    }

    query = wrapper.wrap(start, len);
}

inline void parse_path(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts &dst)
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
                dst.path = wrapper.wrap(start, len);
            else
                dst.path = url_parts::part();
            if ('?' == ch)
            {
                parse_query_and_ref(wrapper, it, dst.query, dst.ref);
                return;
            }

            dst.query = url_parts::part();
            dst.ref   = wrapper.wrap(it.take_left_piece());
            return;
        }

        ++len;
    }

    dst.path = wrapper.wrap(start, len);
}

inline bool parse_after_scheme(const url_part_wrapper &wrapper, parse_url_iterator &it, url_parts &dst)
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

    string_piece<char> scheme = wrapper.wrap(dst.scheme);
    if (strequ(scheme, "http") || strequ(scheme, "https"))
    {
        parse_url_iterator authority(string_piece<char>(start, len));
        if (!parse_authority(wrapper, authority, dst))
            return false;
    }
    else if (strequ(scheme, "file"))
    {
        if (len > 0)
            dst.host = wrapper.wrap(start, len);
        else
            dst.host = url_parts::part();
    }
    else
    {
        ZASSERT(false); // Not reached!
        return false;
    }

    parse_path(wrapper, it, dst);
    return true;
}

inline bool parse_url(const url_part_wrapper &wrapper, url_parts &dst)
{
    parse_url_iterator it(wrapper);

    dst.reset();

    if (!extract_scheme(wrapper, it, dst.scheme))
        return false;

    string_piece<char> scheme = wrapper.wrap(dst.scheme);
    if (strequ(scheme, "http") || strequ(scheme, "https") || strequ(scheme, "file"))
        return parse_after_scheme(wrapper, it, dst);

    parse_path(wrapper, it, dst);
    return true;
}

class url_path_combiner final : public path_combiner<char>
{
    using base_class = path_combiner<char>;
public:
    url_path_combiner(const string_piece<char> &base) : base_class('/')
    {
        size_t p = base.rfind('/');
        if (string_piece<char>::npos != p)
            initialize(base.substr(0, p + 1));
        else
            set_as_root();
    }
};

} // namespace detail

inline void url_parts::reset(void)
{
    scheme   = part();
    username = part();
    password = part();
    host     = part();
    port     = part();
    path     = part();
    query    = part();
    ref      = part();
    is_ipv6 = false;
}

inline bool parse_url(const char *psz, url_parts &dst)
{
    detail::url_part_wrapper wrapper(psz);
    return detail::parse_url(wrapper, dst);
}

template <class String>
bool parse_url(const String &s, url_parts &dst)
{
    detail::url_part_wrapper wrapper(s.data(), s.length());
    return detail::parse_url(wrapper, dst);
}

inline url::url(const std::string &s) : m_string(s)
{
    if (parse_url(m_string, m_parts))
        m_valid = true;
    else
        m_string.clear();
}

inline url url::combine(const std::string &relative) const
{
    if (!m_valid)
        return url();

    url may_be_absolute(relative);
    if (may_be_absolute.is_valid())
        return may_be_absolute;

    url ret;
    ret.m_valid = true;
    ret.m_parts = m_parts;

    detail::url_path_combiner combiner(path_piece());
    combiner.set_prefix(m_string.data(), m_parts.path.start);
    ret.m_string = combiner.combine(relative);

    detail::url_part_wrapper wrapper(ret.m_string.data(), ret.m_string.length());
    detail::parse_url_iterator it(wrapper.substr(m_parts.path.start));
    ret.m_parts.path  = url_parts::part();
    ret.m_parts.query = url_parts::part();
    ret.m_parts.ref   = url_parts::part();
    parse_path(wrapper, it, ret.m_parts);
    return ret;
}

inline std::string url::get_part(const url_parts::part &part) const
{
    return m_valid ? m_string.substr(part.start, part.length) : std::string();
}

inline string_piece<char> url::piece_of(const url_parts::part &part) const
{
    return m_valid ? string_piece<char>(m_string.data() + part.start, part.length) : string_piece<char>();
}

} // namespace zed

#endif // ZED_NET_URL_PARSER_HPP
