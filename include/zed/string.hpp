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
#include <string_view>
#include "./cctype.hpp"

namespace zed {

/**
 * string <-> string_view
 */

template <typename CharT>
std::basic_string_view<CharT> stov(const std::basic_string<CharT> &s)
{
    return std::basic_string_view<CharT>(s.data(), s.length());
}

template <typename CharT>
std::basic_string<CharT> vtos(const std::basic_string_view<CharT> &s)
{
    return std::basic_string<CharT>(s.data(), s.length());
}

/**
 * Iterator Stuff
 */

template <typename CharT>
class psz_iterator
{
public:
	explicit psz_iterator(void) = default;
	explicit psz_iterator(const CharT *psz) : m_p(psz) {}

	CharT operator*() const { return *m_p; }
	psz_iterator& operator++()
	{
		++m_p;
		if ('\0' == *m_p)
			m_p = nullptr;
		return *this;
	}
	bool operator!=(const psz_iterator &r) const
	{
		return m_p != r.m_p;
	}
private:
	const CharT *m_p = nullptr;
};

template <typename CharT>
psz_iterator<CharT> begin(const CharT *psz)
{
	return psz_iterator<CharT>(psz);
}

template <typename CharT>
psz_iterator<CharT> end(const CharT *)
{
	return psz_iterator<CharT>();
}

template <typename String>
typename String::const_iterator begin(const String &s)
{
	return s.begin();
}

template <typename String>
typename String::const_iterator end(const String &s)
{
	return s.end();
}

/**
 * Comparisons
 */

template <typename S1, typename S2>
int strcmp(const S1 &s1, const S2 &s2)
{
	auto b1 = zed::begin(s1);
	const auto e1 = zed::end(s1);
	auto b2 = zed::begin(s2);
	const auto e2 = zed::end(s2);
	while (b1 != e1 && b2 != e2)
	{
		auto c1 = *b1;
		auto c2 = *b2;
		if (c1 != c2)
		{
			if (c1 > c2)
				return 1;
			if (c1 < c2)
				return -1;
		}
		++b1; ++b2;
	}

	if (b1 != e1)
		return 1;
	if (b2 != e2)
		return -1;
	return 0;
}

template <typename S1, typename S2>
int stricmp(const S1 &s1, const S2 &s2)
{
	auto b1 = zed::begin(s1);
	const auto e1 = zed::end(s1);
	auto b2 = zed::begin(s2);
	const auto e2 = zed::end(s2);
	while (b1 != e1 && b2 != e2)
	{
		auto c1 = *b1;
		auto c2 = *b2;
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
		++b1; ++b2;
	}

	if (b1 != e1)
		return 1;
	if (b2 != e2)
		return -1;
	return 0;
}

template <typename S1, typename S2>
bool strequ(const S1 &s1, const S2 &s2)
{
	return 0 == zed::strcmp(s1, s2);
}

template <typename S1, typename S2>
bool striequ(const S1 &s1, const S2 &s2)
{
	return 0 == zed::stricmp(s1, s2);
}

} // namespace zed

#endif // ZED_STRING_HPP
