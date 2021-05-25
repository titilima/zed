#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: container_utilites.hpp
//      Author: Ziming Li
//     Created: 2021-02-14
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_CONTAINER_UTILITES_HPP
#define ZED_CONTAINER_UTILITES_HPP

#include "./build_macros.h"
#include <stack>
#include <queue>
#include <unordered_map>
#include <utility>

namespace zed {

/**
 * Map Helpers
 */

template <typename MapType>
const typename MapType::mapped_type* find_value(const MapType &src_map, const typename MapType::key_type &key);

template <typename MapType>
typename MapType::mapped_type* find_value(MapType &src_map, const typename MapType::key_type &key);

template <typename MapType>
bool key_exists(const MapType &src_map, const typename MapType::key_type &key);

template <typename MapType>
typename MapType::mapped_type query_value(
    const MapType &src_map,
    const typename MapType::key_type &key,
    const typename MapType::mapped_type &default_value = typename MapType::mapped_type()
);

/**
 * Pop Stuff
 */

template <class C>
struct default_container_pop_policy
{
    static typename C::reference value_to_pop(C &c);
};

template <class C, typename P = default_container_pop_policy<C>>
typename C::value_type pop(C &c);

template <class C>
typename C::value_type pop_front(C &c);

template <class C>
typename C::value_type pop_back(C &c);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename MapType>
const typename MapType::mapped_type* find_value(const MapType &src_map, const typename MapType::key_type &key)
{
    auto it = src_map.find(key);
    return src_map.end() != it ? &(it->second) : nullptr;
}

template <typename MapType>
typename MapType::mapped_type* find_value(MapType &src_map, const typename MapType::key_type &key)
{
    auto it = src_map.find(key);
    return src_map.end() != it ? &(it->second) : nullptr;
}

template <typename MapType>
bool key_exists(const MapType &src_map, const typename MapType::key_type &key)
{
#if (_Z_CPP < _Z_CPP_20)
    return src_map.find(key) != src_map.end();
#else
    return src_map.contains(key);
#endif
}

template <typename MapType>
typename MapType::mapped_type query_value(
    const MapType &src_map,
    const typename MapType::key_type &key,
    const typename MapType::mapped_type &default_value)
{
    auto it = src_map.find(key);
    return src_map.end() != it ? it->second : default_value;
}

template <typename T>
struct default_container_pop_policy<std::queue<T>>
{
    static T& value_to_pop(std::queue<T> &q) { return q.front(); }
};

template <typename T>
struct default_container_pop_policy<std::stack<T>>
{
    static T& value_to_pop(std::stack<T> &s) { return s.top(); }
};

template <class C, typename P>
[[nodiscard]] typename C::value_type pop(C &c)
{
    typename C::value_type ret = C::value_type();
    std::swap(ret, P::value_to_pop(c));
    c.pop();
    return ret;
}

template <class C>
[[nodiscard]] typename C::value_type pop_front(C &c)
{
    typename C::value_type ret = C::value_type();
    std::swap(ret, c.front());
    c.pop_front();
    return ret;
}

template <class C>
[[nodiscard]] typename C::value_type pop_back(C &c)
{
    typename C::value_type ret = C::value_type();
    std::swap(ret, c.back());
    c.pop_back();
    return ret;
}

} // namespace zed

#endif
