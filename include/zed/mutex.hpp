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
#ifdef _Z_OS_POSIX
#   include <pthread.h>
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
#endif // _Z_OS_WINDOWS

#ifdef _Z_OS_POSIX
class mutex
{
public:
    mutex(void) { ::pthread_mutex_init(&m_mutex, nullptr); }
    ~mutex(void) { ::pthread_mutex_destroy(&m_mutex); }

    void lock(void) { ::pthread_mutex_lock(&m_mutex); }
    void unlock(void) { ::pthread_mutex_unlock(&m_mutex); }
private:
    pthread_mutex_t m_mutex;
};
#endif // _Z_OS_POSIX

} // namespace zed

#endif // ZED_MUTEX_HPP
