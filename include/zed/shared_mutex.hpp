#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: shared_mutex.hpp
//      Author: Ziming Li
//     Created: 2021-06-12
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_SHARED_MUTEX_HPP
#define ZED_SHARED_MUTEX_HPP

#include <shared_mutex> // for std::shared_lock
#include "./mutex.hpp"

namespace zed {

namespace detail {

template <class T>
class shared_mutex_base : public mutex_base<T>
{
public:
    std::shared_lock<T> guard_shared(void);
protected:
    shared_mutex_base(void) = default;
};

} // namespace detail

#if defined(_Z_OS_WINDOWS)
class shared_mutex : public detail::shared_mutex_base<shared_mutex>
{
public:
    shared_mutex(void) { ::InitializeSRWLock(&m_lock); }

    void lock_shared(void) { ::AcquireSRWLockShared(&m_lock); }
    void unlock_shared(void) { ::ReleaseSRWLockShared(&m_lock); }
    void lock(void) { ::AcquireSRWLockExclusive(&m_lock); }
    void unlock(void) { ::ReleaseSRWLockExclusive(&m_lock); }
private:
    SRWLOCK m_lock;
};
#elif defined(_Z_OS_POSIX)
class shared_mutex : public detail::shared_mutex_base<shared_mutex>
{
public:
    shared_mutex(void) { ::pthread_rwlock_init(&m_lock, nullptr); }
    ~shared_mutex(void) { ::pthread_rwlock_destroy(&m_lock); }

    void lock_shared(void) { ::pthread_rwlock_rdlock(&m_lock); }
    void unlock_shared(void) { ::pthread_rwlock_unlock(&m_lock); }
    void lock(void) { ::pthread_rwlock_wrlock(&m_lock); }
    void unlock(void) { ::pthread_rwlock_unlock(&m_lock); }
private:
    pthread_rwlock_t m_lock;
};
#endif // defined(_Z_OS_WINDOWS)

namespace detail {

template <class T>
std::shared_lock<T> shared_mutex_base<T>::guard_shared(void)
{
    return std::shared_lock<T>(static_cast<T &>(*this));
}

} // namespace detail

} // namespace zed

#endif // ZED_SHARED_MUTEX_HPP
