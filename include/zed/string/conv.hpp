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
std::wstring multi_byte_to_wide_string(const string_piece<char> &s, UINT cp = CP_UTF8);
std::wstring multi_byte_to_wide_string(PCSTR psz, UINT cp = CP_UTF8);
inline std::wstring multi_byte_to_wide_string(const std::string &s, UINT cp = CP_UTF8) { return multi_byte_to_wide_string(string_piece<char>(s), cp); }

std::string wide_string_to_multi_byte(const string_piece<wchar_t> &s, UINT cp = CP_UTF8);
std::string wide_string_to_multi_byte(PCWSTR psz, UINT cp = CP_UTF8);
inline std::string wide_string_to_multi_byte(const std::wstring &s, UINT cp = CP_UTF8) { return wide_string_to_multi_byte(string_piece<wchar_t>(s), cp); }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

namespace detail {
#ifdef _Z_OS_WINDOWS
inline std::wstring multi_byte_to_wide_string(PCSTR ps, int length, UINT cp)
{
    std::wstring ret;
    int r = ::MultiByteToWideChar(cp, 0, ps, length, nullptr, 0);
    if (r > 0)
    {
        ret.resize(r);
        ::MultiByteToWideChar(cp, 0, ps, length, const_cast<PWSTR>(ret.c_str()), r + 1);
    }
    return ret;
}

std::string wide_string_to_multi_byte(PCWSTR ps, int length, UINT cp)
{
    std::string ret;
    int r = ::WideCharToMultiByte(cp, 0, ps, length, nullptr, 0, nullptr, nullptr);
    if (r > 0)
    {
        ret.resize(r);
        ::WideCharToMultiByte(cp, 0, ps, length, const_cast<PSTR>(ret.c_str()), r + 1, nullptr, nullptr);
    }
    return ret;
}
#endif // _Z_OS_WINDOWS
} // namespace detail

#ifdef _Z_OS_WINDOWS
inline std::wstring multi_byte_to_wide_string(const string_piece<char> &s, UINT cp)
{
    return detail::multi_byte_to_wide_string(s.data(), s.length(), cp);
}

inline std::wstring multi_byte_to_wide_string(PCSTR psz, UINT cp)
{
    return detail::multi_byte_to_wide_string(psz, -1, cp);
}

inline std::string wide_string_to_multi_byte(const string_piece<wchar_t> &s, UINT cp)
{
    return detail::wide_string_to_multi_byte(s.data(), s.length(), cp);
}

inline std::string wide_string_to_multi_byte(PCWSTR psz, UINT cp)
{
    return detail::wide_string_to_multi_byte(psz, -1, cp);
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_STRING_CONV_HPP
