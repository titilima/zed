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
#include <Windows.h>

namespace zed {

class hmodule
{
public:
    static std::wstring get_file_name(HMODULE h);
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

} // namespace zed

#endif // ZED_WIN_HMODULE_HPP
