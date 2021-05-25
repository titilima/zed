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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename K, typename V>
key_pair<K, V> key_pair<K, V>::make(const K &k, const V &v)
{
    return key_pair<K, V>(k, v);
}

} // namespace zed

#endif // ZED_UTILITY_HPP
