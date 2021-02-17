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

class log_formatter : formatter_impl<char>
{
public:
    template <typename... Args>
    log_formatter(const char *fmt, const Args&... args) : formatter_impl(fmt)
    {
        args_collector ac(args...);

        size_t idx = 0;
        const auto callback = [&ac, &idx](const std::string &)
        {
            std::string ret;
            if (idx < ac.size())
                ret = ac.at(idx);
            ++idx;
            return ret;
        };
        m_log = formatter_impl::format(callback);
    }

    operator const std::string& () const { return m_log; }
private:
    std::string m_log;
};

template <typename... Args>
void log(const char *fmt, const Args&... args)
{
    log_formatter lf(fmt, args...);
#ifdef _Z_OS_WINDOWS
    std::wstring ws = multi_byte_to_wide_string(lf);
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
