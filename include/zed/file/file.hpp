#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: file.hpp
//      Author: Ziming Li
//     Created: 2021-02-11
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_FILE_FILE_HPP
#define ZED_FILE_FILE_HPP

#include <cstdio>
#include "../build_macros.h"
#ifdef _Z_OS_WINDOWS
#   include "../win/handled_resource.hpp"
#endif

namespace zed {

class file
{
public:
#ifdef _Z_OS_WINDOWS
    using path_t = PCWSTR;
#else
    using path_t = const char *;
#endif

    static bool read(path_t path, std::string &dst);
};

template <>
struct default_resource_finalizer<FILE *>
{
    void operator()(FILE *fp) const { ::fclose(fp); }
};

#ifdef _Z_OS_WINDOWS
using unique_file = unique_resource<HANDLE, default_resource_finalizer<HANDLE>, file_handle_traits>;
#else
using unique_file = unique_resource<FILE *>;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _Z_OS_WINDOWS
inline bool file::read(path_t path, std::string &dst)
{
    unique_file file(::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr));
    if (file)
    {
        dst.clear();
        DWORD size = ::GetFileSize(file.get(), nullptr);
        if (size > 0)
        {
            dst.resize(size);
            return ::ReadFile(file.get(), const_cast<char *>(dst.data()), dst.length(), &size, nullptr);
        }
    }
    return false;
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_FILE_FILE_HPP
