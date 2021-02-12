#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: mutex.hpp
//      Author: Ziming Li
//     Created: 2021-02-12
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_MUTEX_HPP
#define ZED_MUTEX_HPP

#include "./platform_sdk.h"
#ifdef _Z_OS_WINDOWS
#   include "./win/handled_resource.hpp"
#endif

namespace zed {

#ifdef _Z_OS_WINDOWS
class mutex : unique_resource<HANDLE>
{
public:
    mutex(void) : unique_resource(::CreateMutex(nullptr, FALSE, nullptr)) {}

    void lock(void) { ::WaitForSingleObject(get(), INFINITE); }
    void unlock(void) { ::ReleaseMutex(get()); }
};
#endif

} // namespace zed

#endif // ZED_MUTEX_HPP
