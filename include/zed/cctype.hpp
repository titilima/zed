#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: cctype.hpp
//      Author: Ziming Li
//     Created: 2021-02-09
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_CCTYPE_HPP
#define ZED_CCTYPE_HPP

namespace zed {

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

} // namespace zed

#endif // ZED_CCTYPE_HPP
