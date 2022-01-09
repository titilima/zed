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

struct log_serializer {
    template <typename T>
    static void push(std::vector<std::string> &dst, const T &arg) {
        static_assert(false, "Not implemented!");
    }

    static void push(std::vector<std::string> &dst, bool b) {
        push(dst, b ? "true" : "false");
    }
    static void push(std::vector<std::string> &dst, int n) {
        push(dst, std::to_string(n));
    }
    static void push(std::vector<std::string> &dst, const std::string &s) {
        dst.emplace_back(s);
    }
    static void push(std::vector<std::string> &dst, const char *psz) {
        dst.emplace_back(psz);
    }
};

template <typename... Args>
void log(const char *fmt, const Args&... args)
{
    args_collector<log_serializer> ac;
    std::string s = detail::sequence_format(ac, fmt, args...);
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
