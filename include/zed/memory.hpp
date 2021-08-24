#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: memory.hpp
//      Author: Ziming Li
//     Created: 2021-02-05
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_MEMORY_HPP
#define ZED_MEMORY_HPP

#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace zed {

/**
 * unique_resource
 */

template <typename T>
struct is_resource
{
    static constexpr bool value = std::is_integral<T>::value || std::is_pointer<T>::value;
};

template <typename T>
struct default_resource_finalizer
{
    void operator()(T resource) const;
};

template <typename T>
struct default_resource_traits
{
    static constexpr T invalid_value = 0;
};

template <typename T, typename Finalizer = default_resource_finalizer<T>, typename Traits = default_resource_traits<T>, typename = std::enable_if<is_resource<T>::value>>
class unique_resource
{
public:
    unique_resource(void) : m_resource(std::make_pair(Traits::invalid_value, Finalizer())) {}
    explicit unique_resource(T resource) : m_resource(std::make_pair(resource, Finalizer())) {}
    unique_resource(unique_resource &&r) : m_resource(std::make_pair(r.release(), Finalizer())) {}

    ~unique_resource(void)
    {
        if (is_valid(m_resource.first))
            finalize(m_resource.first);
    }

    explicit operator bool() const { return is_valid(m_resource.first); }

    T get(void) const { return m_resource.first; }
    T release(void) { return std::exchange(m_resource.first, Traits::invalid_value); }
    void swap(unique_resource &r) { std::swap(m_resource.first, r.m_resource.first); }

    void reset(T resource = Traits::invalid_value)
    {
        T old_resource = std::exchange(m_resource.first, resource);
        if (is_valid(old_resource))
            finalize(old_resource);
    }

    unique_resource(const unique_resource &) = delete;
    unique_resource& operator=(const unique_resource &) = delete;
protected:
    static bool is_valid(const T resource) { return Traits::invalid_value != resource; }
private:
    void finalize(T resource) { m_resource.second(resource); }

    std::pair<T, Finalizer> m_resource;
};

/**
 * Pointer Wrappers
 */

template <typename T>
class ptr_base
{
public:
    T* get(void) const { return m_ptr; }

    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }
    operator bool() const { return nullptr != m_ptr; }
    bool operator!() const { return nullptr == m_ptr; }

    template <typename U>
    bool operator==(U *ptr) const { return m_ptr == ptr; }
    template <typename U>
    bool operator!=(U *ptr) const { return m_ptr != ptr; }
protected:
    ptr_base(T *ptr) : m_ptr(ptr) {}

    T *m_ptr;
};

template <typename T>
std::unique_ptr<T> wrap_unique(T *p) { return std::unique_ptr<T>(p); }

template <typename T>
std::shared_ptr<T> wrap_shared(T *p) { return std::shared_ptr<T>(p); }

/**
 * Type Casting
 */

template <typename DstType, typename SrcType>
inline DstType bit_cast(const SrcType &src)
{
    static_assert(sizeof(DstType) == sizeof(SrcType));

    DstType dst;
    memcpy(&dst, &src, sizeof(DstType));
    return dst;
}

} // namespace zed

#endif // ZED_MEMORY_HPP
