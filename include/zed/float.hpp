#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: float.hpp
//      Author: Ziming Li
//     Created: 2021-09-05
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_FLOAT_HPP
#define ZED_FLOAT_HPP

#include <cmath>
#include <limits>

namespace zed {

bool almost_equals(float f1, float f2);
bool almost_equals(double d1, double d2);

bool is_almost_zero(float f);
bool is_almost_zero(double d);

namespace detail {

template <typename T>
bool almost_equals(T n1, T n2)
{
    return fabs(n1 - n2) < std::numeric_limits<T>::epsilon();
}

template <typename T>
bool is_almost_zero(T n)
{
    return fabs(n) < std::numeric_limits<T>::epsilon();
}

} // namespace detail

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

inline bool almost_equals(float f1, float f2)
{
    return detail::almost_equals<float>(f1, f2);
}

inline bool almost_equals(double d1, double d2)
{
    return detail::almost_equals<double>(d1, d2);
}

inline bool is_almost_zero(float f)
{
    return detail::is_almost_zero<float>(f);
}

inline bool is_almost_zero(double d)
{
    return detail::is_almost_zero<double>(d);
}

} // namespace zed

#endif // ZED_FLOAT_HPP
