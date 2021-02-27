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

#include <vector>
#include "../string.hpp"

namespace zed {

/**
 * Replace
 */

template <typename CharT>
std::basic_string<CharT> replace(const std::basic_string_view<CharT> &src, const std::basic_string_view<CharT> &new_sub, const std::basic_string_view<CharT> &old_sub);

template <typename S1, typename S2, typename S3, typename = std::enable_if<chartypes_same<S1, S2, S2>::value>>
std::basic_string<typename chartype_trait<S1>::char_type> replace(const S1 &src, const S2 &new_sub, const S3 &old_sub);

/**
 * Split
 */

template <typename String>
std::vector<string_piece<typename String::value_type>> split(const String &src, const typename String::value_type *separator);

template <typename CharT>
std::vector<string_piece<CharT>> split(const CharT *src, const CharT *separator);

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
string_piece<CharT> trim_left(const CharT *psz);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_left(std::basic_string<CharT> *s);

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim_right(const String &s);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
string_piece<CharT> trim_right(const CharT *psz);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_right(std::basic_string<CharT> *s);

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim(const String &s);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
string_piece<CharT> trim(const CharT *psz);

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim(std::basic_string<CharT> *s);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

template <typename CharT>
size_t pszlen(const CharT *psz)
{
    size_t l = 0;
    while ('\0' != *psz)
    {
        ++psz; ++l;
    }
    return l;
}

template <class Adaptor>
std::vector<string_piece<typename Adaptor::char_type>> split(const Adaptor &s, const typename Adaptor::char_type *separator)
{
    using piece_type = string_piece<typename Adaptor::char_type>;

    std::vector<piece_type> ret;

    size_t b = 0;
    size_t e = s.find(separator, b);
    size_t l = pszlen(separator);
    while (Adaptor::npos != e)
    {
        piece_type tmp = s.sub_piece(b, e - b);
        if (!tmp.empty())
        {
            tmp = trim<string_adaptor<piece_type>>(string_adaptor<piece_type>(tmp));
            if (!tmp.empty())
                ret.push_back(tmp);
        }

        b = e + l;
        e = s.find(separator, b);
    }

    piece_type left = s.sub_piece(b);
    if (!left.empty())
    {
        left = trim<string_adaptor<piece_type>>(string_adaptor<piece_type>(left));
        if (!left.empty())
            ret.push_back(left);
    }
    return ret;
}

template <typename Adaptor, typename Chars>
string_piece<typename Adaptor::char_type> trim_left(const Adaptor &s)
{
    size_t p = s.find_first_not_of(Chars::chars);
    if (Adaptor::npos == p)
        p = 0;
    return s.sub_piece(p);
}

template <typename Adaptor, typename Chars>
string_piece<typename Adaptor::char_type> trim_right(const Adaptor &s)
{
    size_t p = s.find_last_not_of(Chars::chars);
    return Adaptor::npos != p ? s.sub_piece(0, p + 1) : string_piece<typename Adaptor::char_type>();
}

template <typename Adaptor, typename Chars = ascii_whitespace<typename Adaptor::char_type>>
string_piece<typename Adaptor::char_type> trim(const Adaptor &s)
{
    string_piece<typename Adaptor::char_type> ret = trim_right<Adaptor, Chars>(s);
    if (!ret.empty())
        ret = trim_left<string_adaptor<string_piece<typename Adaptor::char_type>>, Chars>(string_adaptor(ret));
    return ret;
}

} // namespace detail

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

template <typename String>
std::vector<string_piece<typename String::value_type>> split(const String &s, const typename String::value_type *separator)
{
    detail::string_adaptor<String> adaptor(s);
    return detail::split(adaptor, separator);
}

template <typename CharT>
std::vector<string_piece<CharT>> split(const CharT *ps, const CharT *separator)
{
    detail::string_adaptor<const CharT *> adaptor(ps);
    return detail::split(adaptor, separator);
}

template <typename String, typename Chars>
String trim_left(const String &s)
{
    string_piece ret = detail::trim_left<detail::string_adaptor<String>, Chars>(detail::string_adaptor(s));
    return String(ret.data(), ret.length());
}

template <typename CharT, typename Chars>
string_piece<CharT> trim_left(const CharT *psz)
{
    return detail::trim_left<detail::string_adaptor<const CharT *>, Chars>(detail::string_adaptor(psz));
}

template <typename CharT, typename Chars>
void trim_left(std::basic_string<CharT> *s)
{
    string_piece ret = detail::trim_left<detail::string_adaptor<std::basic_string<CharT>>, Chars>(detail::string_adaptor(*s));
    if (ret.data() != s->data())
    {
        ::memmove(const_cast<CharT *>(s->data()), ret.data(), ret.length() * sizeof(CharT));
        s->resize(ret.length());
    }
}

template <typename String, typename Chars>
String trim_right(const String &s)
{
    string_piece ret = detail::trim_right<detail::string_adaptor<String>, Chars>(detail::string_adaptor(s));
    return String(ret.data(), ret.length());
}

template <typename CharT, typename Chars>
string_piece<CharT> trim_right(const CharT *psz)
{
    return detail::trim_right<detail::string_adaptor<const CharT *>, Chars>(detail::string_adaptor(psz));
}

template <typename CharT, typename Chars>
void trim_right(std::basic_string<CharT> *s)
{
    string_piece<CharT> ret = detail::trim_right<detail::string_adaptor<std::basic_string<CharT>>, Chars>(detail::string_adaptor(*s));
    if (ret.length() != s->length())
        s->resize(ret.length());
}

template <typename String, typename Chars>
String trim(const String &s)
{
    string_piece<typename String::value_type> ret = detail::trim<detail::string_adaptor<String>, Chars>(detail::string_adaptor(s));
    return String(ret.data(), ret.length());
}

template <typename CharT, typename Chars>
string_piece<CharT> trim(const CharT *psz)
{
    return detail::trim<detail::string_adaptor<const CharT *>, Chars>(detail::string_adaptor(psz));
}

template <typename CharT, typename Chars>
void trim(std::basic_string<CharT> *s)
{
    trim_right<detail::string_adaptor<std::basic_string<CharT>>, Chars>(s);
    if (!s->empty())
        trim_left<detail::string_adaptor<std::basic_string<CharT>>, Chars>(s);
}

} // namespace zed

#endif // ZED_STRING_ALGORITHM_HPP
