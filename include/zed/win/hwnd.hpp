#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: hwnd.hpp
//      Author: Ziming Li
//     Created: 2022-02-20
// -------------------------------------------------
// Copyright (C) 2022 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_WIN_HWND_HPP
#define ZED_WIN_HWND_HPP

#include "../platform_sdk.h"

namespace zed {

class hwnd
{
public:
    static bool get_relative_rect(HWND r, HWND h, RECT &dst);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline bool hwnd::get_relative_rect(HWND r, HWND h, RECT &dst)
{
    POINT lt = { 0 };
    if (!::ClientToScreen(r, &lt))
        return false;
    if (!::GetWindowRect(h, &dst))
        return false;
    return ::OffsetRect(&dst, -lt.x, -lt.y);
}

} // namespace zed

#endif // ZED_WIN_HWND_HPP
