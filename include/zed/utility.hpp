#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: utility.hpp
//      Author: Ziming Li
//     Created: 2021-05-21
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_UTILITY_HPP
#define ZED_UTILITY_HPP

#include "./string.hpp"

namespace zed {

template <typename K, typename V>
struct key_pair {
    static key_pair make(const K &k, const V &v);

    K key;
    V value;

    key_pair(void) = default;
    key_pair(const K &k, const V &v) : key(k), value(v) {}
};

template <typename T>
class scoped_swap
{
public:
    scoped_swap(T &dst, const T &new_val);
    ~scoped_swap(void);
private:
    T &m_dst;
    const T m_old_val;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename K, typename V>
key_pair<K, V> key_pair<K, V>::make(const K &k, const V &v)
{
    return key_pair<K, V>(k, v);
}

template <typename T>
scoped_swap<T>::scoped_swap(T &dst, const T &new_val) : m_dst(dst), m_old_val(dst)
{
    m_dst = new_val;
}

template <typename T>
scoped_swap<T>::~scoped_swap(void)
{
    m_dst = m_old_val;
}

} // namespace zed

#endif // ZED_UTILITY_HPP
