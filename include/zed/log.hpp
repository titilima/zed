#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: log.hpp
//      Author: Ziming Li
//     Created: 2021-02-17
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_LOG_HPP
#define ZED_LOG_HPP

#include "./platform_sdk.h"
#include "./string/format.hpp"
#ifdef _Z_OS_WINDOWS
#   include "./string/conv.hpp"
#endif

namespace zed {

template <typename... Args>
void log(const char *fmt, const Args&... args);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename... Args>
void log(const char *fmt, const Args&... args)
{
    std::string s = sequence_format(fmt, args...);
#ifdef _Z_OS_WINDOWS
    std::wstring ws = multi_byte_to_wide_string(s);
    ws.append(L"\r\n");
    ::OutputDebugStringW(ws.c_str());
#endif
}

inline void log(const char *s)
{
#ifdef _Z_OS_WINDOWS
    std::wstring ws = multi_byte_to_wide_string(s);
    ws.append(L"\r\n");
    ::OutputDebugStringW(ws.c_str());
#endif
}

} // namespace zed

#ifdef NDEBUG
#   define ZLOG(...)    ((void)0)
#else
#   define ZLOG         ::zed::log
#endif

#endif // ZED_LOG_HPP
