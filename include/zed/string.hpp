#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: string.hpp
//      Author: Ziming Li
//     Created: 2021-02-08
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_HPP
#define ZED_STRING_HPP

#include "./build_macros.h"

#include <string>
#include "./assert.h"
#include "./ctype.hpp"
#include "./type_traits.hpp"
#ifdef _Z_STRING_VIEW_ENABLED
#   include <string_view>
#endif

namespace zed {
#ifdef _Z_STRING_VIEW_ENABLED
template <typename CharT>
using string_piece = std::basic_string_view<CharT>;
#else
template <typename CharT>
class string_piece
{
public:
    explicit string_piece(void) = default;
    explicit string_piece(const CharT *ps, size_t length) : m_ps(ps), m_length(length) {}
    explicit string_piece(const std::basic_string<CharT> &s) : m_ps(s.data()), m_length(s.length()) {}

    constexpr static auto npos = std::basic_string<CharT>::npos;

    const CharT* data(void) const { return m_ps; }
    size_t length(void) const { return m_length; }
    bool empty(void) const { return 0 == m_length; }

    size_t find(CharT ch, size_type pos = 0) const;
    size_t rfind(CharT ch, size_type pos = npos) const;
    string_piece<CharT> substr(size_type pos, size_type count = npos) const;
private:
    const CharT *m_ps = nullptr;
    size_t m_length = 0;
};
#endif // _Z_STRING_VIEW_ENABLED
} // namespace zed

namespace std {
template <typename CharT>
[[nodiscard]] basic_string<CharT> to_string(const zed::string_piece<CharT> &s)
{
    return s.empty() ? basic_string<CharT>() : basic_string<CharT>(s.data(), s.length());
}
} // namespace std

namespace zed {

/**
 * Comparisons
 */

template <typename S1, typename S2, typename = std::enable_if<chartypes_same<S1, S2>::value>>
int strcmp(const S1 &s1, const S2 &s2);

template <typename S1, typename S2, typename = std::enable_if<chartypes_same<S1, S2>::value>>
int stricmp(const S1 &s1, const S2 &s2);

template <typename S1, typename S2, typename = std::enable_if<chartypes_same<S1, S2>::value>>
bool strequ(const S1 &s1, const S2 &s2) { return 0 == zed::strcmp(s1, s2); }

template <typename S1, typename S2, typename = std::enable_if<chartypes_same<S1, S2>::value>>
bool striequ(const S1 &s1, const S2 &s2) { return 0 == zed::stricmp(s1, s2); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {

template <typename T>
class string_adaptor
{
public:
    using char_type = typename T::value_type;
    constexpr static auto npos = T::npos;

    explicit string_adaptor(const T &s) : m_s(s) {}

    size_t find(const char_type *psz, size_t pos = 0) const { return m_s.find(psz, pos); }
    size_t find_first_not_of(const char_type *psz) const { return m_s.find_first_not_of(psz); }
    size_t find_last_not_of(const char_type *psz) const { return m_s.find_last_not_of(psz); }

    string_piece<char_type> sub_piece(size_t pos, size_t count = npos) const
    {
        count = std::min(m_s.length() - pos, count);
        return count > 0 ? string_piece<char_type>(m_s.data() + pos, count) : string_piece<char_type>();
    }
private:
    const T &m_s;
};

template <typename CharT>
class string_adaptor<const CharT *>
{
public:
    using char_type = CharT;
    constexpr static auto npos = std::basic_string<CharT>::npos;

    explicit string_adaptor(const CharT *psz) : m_psz(psz) {}

    size_t find_first_not_of(const char_type *psz) const
    {
        for (const CharT *p = m_psz; '\0' != *p; ++p)
        {
            if (!in_chars(psz, *p))
                return p - m_psz;
        }
        return npos;
    }
    size_t find_last_not_of(const char_type *psz) const
    {
        size_t i = 0, ret = 0;
        for (const CharT *p = m_psz; '\0' != *p; ++p)
        {
            if (!in_chars(psz, *p))
                ret = i;
            ++i;
        }
        return ret;
    }

    string_piece<char_type> sub_piece(size_t pos, size_t count = npos) const
    {
        if (npos == count)
        {
            count = 0;
            for (const CharT *p = m_psz + pos; '\0' != *p; ++p)
                ++count;
        }
        return string_piece<char_type>(m_psz + pos, count);
    }
private:
    static bool in_chars(const CharT *chars, int ch)
    {
        while ('\0' != *chars)
        {
            if (*chars == ch)
                return true;
            ++chars;
        }
        return false;
    }

    const CharT *m_psz;
};

template <class String>
class char_iterator
{
public:
    using char_type = typename String::value_type;

    explicit char_iterator(const String &s) : m_ps(s.data()), m_left(s.length()) {}

    typename char_type operator*() const { return *m_ps; }
    char_iterator& operator++()
    {
        ZASSERT(!reach_the_end());
        ++m_ps; --m_left;
        return *this;
    }

    bool reach_the_end(void) const { return 0 == m_left; }

    const char_type* data(void) const { ZASSERT(!reach_the_end()); return m_ps; }

    string_piece<char_type> take_left_piece(void)
    {
        if (m_left > 0)
        {
            auto start = m_ps;
            size_t len = m_left;

            m_ps  += m_left;
            m_left = 0;
            return string_piece<char_type>(start, len);
        }
        else
        {
            return string_piece<char_type>();
        }
    }
private:
    const char_type *m_ps;
    size_t m_left;
};

template <typename CharT>
class char_iterator<const CharT *>
{
public:
    using char_type = CharT;

    explicit char_iterator(const CharT *psz) : m_psz(psz) {}

    char_type operator*() const { return *m_psz; }
    char_iterator& operator++()
    {
        ZASSERT(!reach_the_end());
        ++m_psz;
        return *this;
    }

    bool reach_the_end(void) const { return *m_psz == '\0'; }

    const char_type* data(void) const { return m_psz; }

    string_piece<char_type> take_left_piece(void)
    {
        auto start = m_psz;
        size_t len = 0;
        while ('\0' != *m_psz)
        {
            ++m_psz;
            ++len;
        }
        return len > 0 ? string_piece<char_type>(start, len) : string_piece<char_type>();
    }
private:
    const char_type *m_psz;
};

template <typename CharT, size_t N>
class char_iterator<CharT[N]>
{
public:
    using char_type = CharT;

    explicit char_iterator(const CharT psz[]) : m_psz(psz) {}

    char_type operator*() const { return *m_psz; }
    char_iterator& operator++()
    {
        ZASSERT(!reach_the_end());
        ++m_psz;
        return *this;
    }

    bool reach_the_end(void) const { return *m_psz == '\0'; }

    const char_type* data(void) const { return m_psz; }

    string_piece<char_type> take_left_piece(void)
    {
        auto start = m_psz;
        size_t len = 0;
        while ('\0' != *m_psz)
        {
            ++m_psz;
            ++len;
        }
        return len > 0 ? string_piece<char_type>(start, len) : string_piece<char_type>();
    }
private:
    const char_type *m_psz;
};

} // namespace detail

#ifndef _Z_STRING_VIEW_ENABLED
template <typename CharT>
size_t string_piece<CharT>::find(CharT ch, size_type pos) const
{
    while (pos < m_length)
    {
        if (m_ps[pos] == ch)
            return pos;
        ++pos;
    }
    return npos;
}

template <typename CharT>
size_t string_piece<CharT>::rfind(CharT ch, size_type pos) const
{
    if (0 == m_length)
        return npos;

    if (pos >= m_length)
        pos = m_length - 1;
    while (pos > 0)
    {
        if (m_ps[pos] == ch)
            return pos;
        --pos;
    }
    return m_ps[0] == ch ? 0 : npos;
}

template <typename CharT>
string_piece<CharT> string_piece<CharT>::substr(size_type pos, size_type count) const
{
    if (npos == count)
        count = m_length;
    return string_piece<CharT>(m_ps + pos, count - pos);
}
#endif // _Z_STRING_VIEW_ENABLED

template <typename S1, typename S2, typename>
int strcmp(const S1 &s1, const S2 &s2)
{
    detail::char_iterator it1(s1);
    detail::char_iterator it2(s2);
    while (!it1.reach_the_end() && !it2.reach_the_end())
    {
        auto c1 = *it1;
        auto c2 = *it2;
        if (c1 != c2)
        {
            if (c1 > c2)
                return 1;
            if (c1 < c2)
                return -1;
        }
        ++it1; ++it2;
    }

    if (!it1.reach_the_end())
        return 1;
    if (!it2.reach_the_end())
        return -1;
    return 0;
}

template <typename S1, typename S2, typename>
int stricmp(const S1 &s1, const S2 &s2)
{
    detail::char_iterator it1(s1);
    detail::char_iterator it2(s2);
    while (!it1.reach_the_end() && !it2.reach_the_end())
    {
        auto c1 = *it1;
        auto c2 = *it2;
        if (c1 != c2)
        {
            if (zed::isalpha(c1) && zed::isalpha(c2))
            {
                if (isupper(c1))
                    c1 = std::tolower(c1);
                if (isupper(c2))
                    c2 = std::tolower(c2);
            }

            if (c1 > c2)
                return 1;
            if (c1 < c2)
                return -1;
        }
        ++it1; ++it2;
    }

    if (!it1.reach_the_end())
        return 1;
    if (!it2.reach_the_end())
        return -1;
    return 0;
}

} // namespace zed

#endif // ZED_STRING_HPP
