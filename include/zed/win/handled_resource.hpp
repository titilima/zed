#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: handled_resource.hpp
//      Author: Ziming Li
//     Created: 2021-02-06
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_WIN_HANDLED_RESOURCE_HPP
#define ZED_WIN_HANDLED_RESOURCE_HPP

#include <Windows.h>
#include "../memory.hpp"

namespace zed {

template <>
struct default_resource_finalizer<HANDLE>
{
    void operator()(HANDLE h) const { ::CloseHandle(h); }
};

struct file_handle_traits
{
    static constexpr HANDLE invalid_value = INVALID_HANDLE_VALUE;
};

} // namespace zed

#endif // ZED_WIN_HANDLED_RESOURCE_HPP
