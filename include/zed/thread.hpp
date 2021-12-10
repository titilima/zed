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
#ifdef _Z_OS_WINDOWS
#   include "./string/conv.hpp"
#   include "./win/hmodule.hpp"
#endif

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

#if defined(_Z_OS_WINDOWS)
using thread_id_t = DWORD;
#endif

class current_thread
{
public:
    static thread_id_t id(void);
    static void set_name(const char *name);
private:
#ifdef _Z_OS_WINDOWS
    static bool set_description(const char *description);
    static void set_name_for_debugger(const char *name);
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

inline thread_id_t current_thread::id(void)
{
    return ::GetCurrentThreadId();
}

inline void current_thread::set_name(const char *name)
{
#ifdef NDEBUG
    if (!set_description(name))
        set_name_for_debugger(name);
#else
    set_name_for_debugger(name); // Enough for debugging.
#endif
}

inline bool current_thread::set_description(const char *description)
{
    if (HMODULE kernel32 = ::GetModuleHandleA("Kernel32.dll"))
    {
        HRESULT (WINAPI * pfn)(HANDLE, PCWSTR);
        if (hmodule::get_proc_address(kernel32, "SetThreadDescription", pfn))
        {
            std::wstring ws = multi_byte_to_wide_string(description);
            HRESULT hr = pfn(::GetCurrentThread(), ws.c_str());
            if (SUCCEEDED(hr))
                return true;
        }
    }
    return false;
}

inline void current_thread::set_name_for_debugger(const char *name)
{
    constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
    struct THREADNAME_INFO {
        THREADNAME_INFO(PCSTR name) : szName(name) {}
        DWORD dwType = 0x1000;
        PCSTR szName;
        DWORD dwThreadID = -1;
        DWORD dwFlags = 0;
    };
#pragma pack(pop)
    THREADNAME_INFO info(name);
    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR *>(&info));
    }
    __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
}
#endif // _Z_OS_WINDOWS

} // namespace zed

#endif // ZED_THREAD_HPP
