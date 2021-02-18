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
#include "./cctype.hpp"
#include "./string_view.hpp"
#include "./type_traits.hpp"

namespace zed {

#ifdef _Z_STRING_VIEW_ENABLED
/**
 * string <-> string_view
 */

template <typename CharT>
std::basic_string_view<CharT> stov(const std::basic_string<CharT> &s) { return std::basic_string_view<CharT>(s); }

template <typename CharT>
[[nodiscard]] std::basic_string<CharT> vtos(const std::basic_string_view<CharT> &s) { return std::basic_string<CharT>(s); }
#endif

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
