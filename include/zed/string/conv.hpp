#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: conv.hpp
//      Author: Ziming Li
//     Created: 2021-02-11
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_CONV_HPP
#define ZED_STRING_CONV_HPP

#include "../platform_sdk.h"
#include "../string.hpp"

namespace zed {

#ifdef _Z_OS_WINDOWS
std::wstring multi_byte_to_wide_string(const std::string_view &s, UINT cp = CP_UTF8);
inline std::wstring multi_byte_to_wide_string(PCSTR psz, UINT cp = CP_UTF8) { return multi_byte_to_wide_string(std::string_view(psz), cp); }
inline std::wstring multi_byte_to_wide_string(const std::string &s, UINT cp = CP_UTF8) { return multi_byte_to_wide_string(stov(s), cp); }

std::string wide_string_to_multi_byte(const std::wstring_view &s, UINT cp = CP_UTF8);
inline std::string wide_string_to_multi_byte(PCWSTR psz, UINT cp = CP_UTF8) { return wide_string_to_multi_byte(std::wstring_view(psz), cp); }
inline std::string wide_string_to_multi_byte(const std::wstring &s, UINT cp = CP_UTF8) { return wide_string_to_multi_byte(stov(s), cp); }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

#ifdef _Z_OS_WINDOWS
inline std::wstring multi_byte_to_wide_string(const std::string_view &s, UINT cp)
{
    std::wstring ret;
    int r = ::MultiByteToWideChar(cp, 0, s.data(), s.length(), nullptr, 0);
    if (r > 0)
    {
        ret.resize(r);
        ::MultiByteToWideChar(cp, 0, s.data(), s.length(), const_cast<PWSTR>(ret.c_str()), r + 1);
    }
    return ret;
}

std::string wide_string_to_multi_byte(const std::wstring_view &s, UINT cp)
{
    std::string ret;
    int r = ::WideCharToMultiByte(cp, 0, s.data(), s.length(), nullptr, 0, nullptr, nullptr);
    if (r > 0)
    {
        ret.resize(r);
        ::WideCharToMultiByte(cp, 0, s.data(), s.length(), const_cast<PSTR>(ret.c_str()), r + 1, nullptr, nullptr);
    }
    return ret;
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_STRING_CONV_HPP
