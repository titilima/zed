#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: thread.hpp
//      Author: Ziming Li
//     Created: 2021-12-10
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_THREAD_HPP
#define ZED_THREAD_HPP

#include "./platform_sdk.h"

namespace zed {

template <class T>
class thread
{
public:
    using worker = void(T::*)(void);
    thread(T *p, worker pfn);
    ~thread(void);

    void join(void);
private:
    void work(void) { (m_p->*m_worker)(); }

    T *m_p;
    worker m_worker;

#if defined(_Z_OS_WINDOWS)
    static DWORD WINAPI callback(PVOID arg);
    HANDLE m_handle;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <class T>
thread<T>::thread(T *p, worker pfn)
    : m_p(p), m_worker(pfn)
#if defined(_Z_OS_WINDOWS)
    , m_handle(::CreateThread(nullptr, 0, callback, this, 0, nullptr))
#endif
{
}

#ifdef _Z_OS_WINDOWS
template <class T>
thread<T>::~thread(void)
{
    ::CloseHandle(m_handle);
}

template <class T>
DWORD WINAPI thread<T>::callback(PVOID arg)
{
    reinterpret_cast<thread<T> *>(arg)->work();
    return EXIT_SUCCESS;
}

template <class T>
void thread<T>::join(void)
{
    ::WaitForSingleObject(m_handle, INFINITE);
}
#endif

} // namespace zed

#endif // ZED_THREAD_HPP
