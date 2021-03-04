#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: signal.hpp
//      Author: Ziming Li
//     Created: 2021-03-01
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_THREADING_SIGNAL_HPP
#define ZED_THREADING_SIGNAL_HPP

#include "../platform_sdk.h"
#if defined(_Z_OS_WINDOWS)
#   include "../win/handled_resource.hpp"
#elif defined(_Z_OS_POSIX)
#   include <pthread.h>
#endif

namespace zed {

#ifdef _Z_OS_WINDOWS
class signal : unique_resource<HANDLE>
{
public:
    signal(void) : unique_resource(::CreateEvent(nullptr, FALSE, FALSE, nullptr)) {}

    void notify(void) { ::SetEvent(get()); }
    void wait(void) { ::WaitForSingleObject(get(), INFINITE); }
};
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_THREADING_SIGNAL_HPP
