#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: ctype.hpp
//      Author: Ziming Li
//     Created: 2021-02-09
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_CTYPE_HPP
#define ZED_CTYPE_HPP

#include <cstring>

namespace zed {

template <typename CharT>
struct ascii_whitespace
{
    static constexpr CharT chars[] = { ' ', '\t', '\n', '\v', '\f', '\r', '\0' };
};

inline bool isspace(int ch) { return nullptr != ::strchr(ascii_whitespace<char>::chars, ch); }

/**
 * NOTE:
 *
 *   The "is-" implementations from Microsoft contain assertions,
 *   and the assert-message-boxes will pop up if `ch` is not an ASCII character.
 *
 *   To avoid these unnecessary troubles, use the functions below.
 */

inline bool islower(int ch) { return 'a' <= ch && ch <= 'z'; }
inline bool isupper(int ch) { return 'A' <= ch && ch <= 'Z'; }
inline bool isalpha(int ch) { return islower(ch) || isupper(ch); }
inline bool isdigit(int ch) { return '0' <= ch && ch <= '9'; }

} // namespace zed

#endif // ZED_CTYPE_HPP
