#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: algorithm.hpp
//      Author: Ziming Li
//     Created: 2021-02-08
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_ALGORITHM_HPP
#define ZED_STRING_ALGORITHM_HPP

#include "../string.hpp"
#ifndef _Z_STRING_VIEW_ENABLED
#   error TODO: Implementation for string_view?
#endif

namespace zed {

/**
 * Replace
 */

template <typename CharT>
std::basic_string<CharT> replace(const std::basic_string_view<CharT> &src, const std::basic_string_view<CharT> &new_sub, const std::basic_string_view<CharT> &old_sub);

template <typename S1, typename S2, typename S3, typename = std::enable_if<chartypes_same<S1, S2, S2>::value>>
std::basic_string<typename chartype_trait<S1>::char_type> replace(const S1 &src, const S2 &new_sub, const S3 &old_sub);

/**
 * Trimming Stuff
 */

template <typename CharT>
struct ascii_whitespace
{
    static constexpr CharT chars[] = { ' ', '\t', '\n', '\v', '\f', '\r', '\0' };
};

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim_left(const String &s);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim_left(const CharT *cs) { return trim_left<std::basic_string_view<CharT>, Chars>(std::basic_string_view<CharT>(cs)); }

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_left(std::basic_string<CharT> *s);

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim_right(const String &s);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim_right(const CharT *cs) { return trim_right<std::basic_string_view<CharT>, Chars>(std::basic_string_view<CharT>(cs)); }

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_right(std::basic_string<CharT> *s);

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim(const String &s);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim(const CharT *cs);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim(std::basic_string<CharT> *s);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename CharT>
std::basic_string<CharT> replace(const std::basic_string_view<CharT> &src, const std::basic_string_view<CharT> &new_sub, const std::basic_string_view<CharT> &old_sub)
{
    std::basic_string<CharT> ret;

    size_t b = 0, e, l = old_sub.length();
    while ((e = src.find(old_sub, b)) != std::basic_string_view<CharT>::npos)
    {
        ret.append(src, b, e - b).append(new_sub);
        b = e + l;
    }

    l = src.length();
    if (b < l)
        ret.append(src, b, l - b);
    return ret;
}

template <typename S1, typename S2, typename S3, typename>
std::basic_string<typename chartype_trait<S1>::char_type> replace(const S1 &src, const S2 &new_sub, const S3 &old_sub)
{
    using char_type = typename chartype_trait<S1>::char_type;
    using string_view_type = std::basic_string_view<char_type>;
    return replace<char_type>(string_view_type(src), string_view_type(new_sub), string_view_type(old_sub));
}

template <typename String, typename Chars>
String trim_left(const String &s)
{
    size_t p = s.find_first_not_of(Chars::chars);
    return String::npos != p ? s.substr(p) : s;
}

template <typename CharT, typename Chars>
void trim_left(std::basic_string<CharT> *s)
{
    std::basic_string_view<CharT> v = stov(*s);
    v = trim_left<std::basic_string_view<CharT>, Chars>(v);
    if (v.data() != s->data())
    {
        ::memmove(const_cast<CharT *>(s->data()), v.data(), v.length() * sizeof(CharT));
        s->resize(v.length());
    }
}

template <typename String, typename Chars>
String trim_right(const String &s)
{
    size_t p = s.find_last_not_of(Chars::chars);
    return String::npos != p ? s.substr(0, p + 1) : String();
}

template <typename CharT, typename Chars>
void trim_right(std::basic_string<CharT> *s)
{
    std::basic_string_view<CharT> v = stov(*s);
    v = trim_right<std::basic_string_view<CharT>, Chars>(v);
    if (v.length() != s->length())
        s->resize(v.length());
}

template <typename String, typename Chars>
String trim(const String &s)
{
    String r = trim_right<String, Chars>(s);
    if (!r.empty())
        r = trim_left<String, Chars>(r);
    return r;
}

template <typename CharT, typename Chars>
void trim(std::basic_string<CharT> *s)
{
    trim_right<CharT, Chars>(s);
    if (!s->empty())
        trim_left<CharT, Chars>(s);
}

} // namespace zed

#endif // ZED_STRING_ALGORITHM_HPP
