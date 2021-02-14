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

#include <stack>
#include <queue>
#include <utility>

namespace zed {

template <class C>
struct default_container_pop_policy
{
    typename C::reference value_to_pop(C &c);
};

template <class C, typename P = default_container_pop_policy<C>>
typename C::value_type pop(C &c);

template <class C>
typename C::value_type pop_front(C &c);

template <class C>
typename C::value_type pop_back(C &c);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename T>
struct default_container_pop_policy<std::queue<T>>
{
    T& value_to_pop(std::queue<T> &q) { return q.front(); }
};

template <typename T>
struct default_container_pop_policy<std::stack<T>>
{
    T& value_to_pop(std::stack<T> &s) { return s.top(); }
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
