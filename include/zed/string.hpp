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

#include <string>
#include "./build_macros.h"
#include "./cctype.hpp"
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

    const CharT* data(void) const { return m_ps; }
    size_t length(void) const { return m_length; }
    bool empty(void) const { return 0 == m_length; }
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
 * Iterator Stuff
 */

template <class String>
class char_iterator
{
public:
    explicit char_iterator(const String &s) : m_it(s.begin()), m_end(s.end()) {}

    typename String::value_type operator*() const { return *m_it; }
    char_iterator& operator++() { ++m_it; return *this; }

    bool reach_the_end(void) const { return m_it == m_end; }
private:
    typename String::const_iterator m_it;
    const typename String::const_iterator m_end;
};

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
    size_t find_first_not_of(const char_type *psz, size_t pos = 0) const { return m_s.find_first_not_of(psz, pos); }
    size_t find_last_not_of(const char_type *psz, size_t pos = npos) const { return m_s.find_last_not_of(psz, pos); }

    string_piece<char_type> sub_piece(size_t pos, size_t count = npos) const
    {
        count = std::min(m_s.length() - pos, count);
        return count > 0 ? string_piece<char_type>(m_s.data() + pos, count) : string_piece<char_type>();
    }
private:
    const T &m_s;
};

} // namespace detail

template <typename CharT>
class char_iterator<const CharT *>
{
public:
    explicit char_iterator(const CharT *psz) : m_p(psz) {}

    CharT operator*() const { return *m_p; }
    char_iterator& operator++() { ++m_p; return *this; }

    bool reach_the_end(void) const { return *m_p == '\0'; }
private:
    const CharT *m_p;
};

template <typename CharT, size_t N>
class char_iterator<CharT[N]>
{
public:
    explicit char_iterator(const CharT psz[]) : m_p(psz) {}

    CharT operator*() const { return *m_p; }
    char_iterator& operator++() { ++m_p; return *this; }

    bool reach_the_end(void) const { return *m_p == '\0'; }
private:
    const CharT *m_p;
};

template <typename S1, typename S2, typename>
int strcmp(const S1 &s1, const S2 &s2)
{
    char_iterator it1(s1);
    char_iterator it2(s2);
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
    char_iterator it1(s1);
    char_iterator it2(s2);
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
