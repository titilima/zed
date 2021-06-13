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

#include <mutex> // for std::unique_lock
#if defined(_Z_OS_WINDOWS)
#   include "./win/handled_resource.hpp"
#elif defined(_Z_OS_POSIX)
#   include <pthread.h>
#endif

namespace zed {

namespace detail {

template <class T>
class mutex_base
{
public:
    std::unique_lock<T> guard(void);
protected:
    mutex_base(void) = default;
};

} // namespace detail

#if defined(_Z_OS_WINDOWS)
class mutex : public detail::mutex_base<mutex>, unique_resource<HANDLE>
{
public:
    mutex(void) : unique_resource(::CreateMutex(nullptr, FALSE, nullptr)) {}

    void lock(void) { ::WaitForSingleObject(get(), INFINITE); }
    void unlock(void) { ::ReleaseMutex(get()); }
};

class recursive_mutex : public detail::mutex_base<recursive_mutex>
{
public:
    recursive_mutex(void) { ::InitializeCriticalSection(&m_cs); }
    ~recursive_mutex(void) { ::DeleteCriticalSection(&m_cs); }

    void lock(void) { ::EnterCriticalSection(&m_cs); }
    void unlock(void) { ::LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs;
};
#elif defined(_Z_OS_POSIX)
class mutex : public detail::mutex_base<mutex>
{
public:
    mutex(void) { ::pthread_mutex_init(&m_mutex, nullptr); }
    ~mutex(void) { ::pthread_mutex_destroy(&m_mutex); }

    void lock(void) { ::pthread_mutex_lock(&m_mutex); }
    void unlock(void) { ::pthread_mutex_unlock(&m_mutex); }
private:
    pthread_mutex_t m_mutex;
};
#endif // defined(_Z_OS_WINDOWS)

namespace detail {

template <class T>
std::unique_lock<T> mutex_base<T>::guard(void)
{
    return std::unique_lock<T>(static_cast<T &>(*this));
}

} // namespace detail

} // namespace zed

#endif // ZED_MUTEX_HPP
