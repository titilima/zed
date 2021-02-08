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

namespace zed {

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

} // namespace zed

#endif // ZED_STRING_HPP
