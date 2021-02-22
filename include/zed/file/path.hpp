#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: path.hpp
//      Author: Ziming Li
//     Created: 2021-02-10
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_FILE_PATH_HPP
#define ZED_FILE_PATH_HPP

#include <string>
#include "../build_macros.h"
#ifdef _Z_OS_WINDOWS
#   include "../win/hmodule.hpp"
#endif

namespace zed {

class path
{
public:
#ifdef _Z_OS_WINDOWS
    using string_t = std::wstring;
    static constexpr WCHAR splitter = L'\\';
#else
    using string_t = std::string;
    static constexpr char splitter = '/';
#endif

    static string_t path_of_file(const string_t &file_name);
};

class sys_path
{
public:
    static path::string_t get_app_path(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline path::string_t path::path_of_file(const string_t &file_name)
{
    size_t p = file_name.rfind(splitter);
    return string_t::npos != p ? file_name.substr(0, p + 1) : string_t();
}

#ifdef _Z_OS_WINDOWS
inline path::string_t sys_path::get_app_path(void)
{
    std::wstring app_file = hmodule::get_file_name(nullptr);
    return path::path_of_file(app_file);
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_FILE_PATH_HPP
