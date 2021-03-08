#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: hmodule.hpp
//      Author: Ziming Li
//     Created: 2021-02-10
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_WIN_HMODULE_HPP
#define ZED_WIN_HMODULE_HPP

#include <string>
#include <tuple>
#include "../platform_sdk.h"

namespace zed {

class hmodule
{
public:
    static std::wstring get_file_name(HMODULE h);

    using resource_data = std::tuple<PVOID, DWORD>;
    static bool get_resource_data(resource_data &dst, HMODULE h, PCTSTR type, int id);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline std::wstring hmodule::get_file_name(HMODULE h)
{
    constexpr DWORD BUF_SIZE = MAX_PATH;

    std::wstring ret(BUF_SIZE, L'\0');
    for (;;)
    {
        DWORD dw = GetModuleFileNameW(h, const_cast<PWSTR>(ret.data()), ret.length());
        DWORD err = GetLastError();

        if (NOERROR == err)
        {
            size_t p = ret.rfind(L'\\', dw);
            ret.resize(p + 1);
            break;
        }

        if (ERROR_INSUFFICIENT_BUFFER != err)
        {
            ret.clear();
            break;
        }

        DWORD size = ret.length() * 2;
        ret.resize(size);
    }
    return ret;
}

inline bool hmodule::get_resource_data(resource_data &dst, HMODULE h, PCTSTR type, int id)
{
    HRSRC res = ::FindResource(h, MAKEINTRESOURCE(id), type);
    if (nullptr == res)
        return false;

    HGLOBAL mem = ::LoadResource(h, res);
    if (nullptr == mem)
        return false;

    PVOID data = ::LockResource(mem);
    if (nullptr == data)
        return false;

    dst = std::make_tuple(data, ::SizeofResource(h, res));
    return true;
}

} // namespace zed

#endif // ZED_WIN_HMODULE_HPP
