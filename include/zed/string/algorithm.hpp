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

#include <string>
#include <string_view>

namespace zed {

/**
 * Trimming Stuff
 */

template <typename CharT>
struct ascii_whitespace
{
	static constexpr CharT chars[] = { ' ', '\t', '\n', '\v', '\f', '\r', '\0' };
};

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim_left(const String &s)
{
	size_t p = s.find_first_not_of(Chars::chars);
	return String::npos != p ? s.substr(p) : s;
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim_left(const CharT *cs)
{
	return trim_left<std::basic_string_view<CharT>, Chars>(std::basic_string_view<CharT>(cs));
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_left(std::basic_string<CharT> *s)
{
	std::basic_string_view<CharT> v(s->data(), s->length());
	v = trim_left<std::basic_string_view<CharT>, Chars>(v);
	if (v.data() != s->data())
	{
		::memmove(const_cast<CharT *>(s->data()), v.data(), v.length() * sizeof(CharT));
		s->resize(v.length());
	}
}

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim_right(const String &s)
{
	size_t p = s.find_last_not_of(Chars::chars);
	return String::npos != p ? s.substr(0, p + 1) : String();
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim_right(const CharT *cs)
{
	return trim_right<std::basic_string_view<CharT>, Chars>(std::basic_string_view<CharT>(cs));
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim_right(std::basic_string<CharT> *s)
{
	std::basic_string_view<CharT> v(s->data(), s->length());
	v = trim_right<std::basic_string_view<CharT>, Chars>(v);
	if (v.length() != s->length())
		s->resize(v.length());
}

template <typename String, typename Chars = ascii_whitespace<typename String::value_type>>
String trim(const String &s)
{
	String r = trim_right<String, Chars>(s);
	if (!r.empty())
		r = trim_left<String, Chars>(r);
	return r;
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
std::basic_string_view<CharT> trim(const CharT *cs)
{
	return trim<std::basic_string_view<CharT>, Chars>(std::basic_string_view<CharT>(cs));
}

template <typename CharT, typename Chars = ascii_whitespace<CharT>>
void trim(std::basic_string<CharT> *s)
{
	trim_right<CharT, Chars>(s);
	if (!s->empty())
		trim_left<CharT, Chars>(s);
}

} // namespace zed

#endif // ZED_STRING_ALGORITHM_HPP
