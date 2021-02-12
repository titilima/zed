#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: type_traits.hpp
//      Author: Ziming Li
//     Created: 2021-02-12
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_TYPE_TRAITS_HPP
#define ZED_TYPE_TRAITS_HPP

#include <type_traits>

namespace zed {

template <typename T>
struct chartype_trait { using char_type = void; };

template <typename T1, typename T2, typename... Args>
struct chartypes_same { static constexpr bool value = chartypes_same<T1, Args...>::value && chartypes_same<T2, Args...>::value; };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations

template <typename CharT>
struct chartype_trait<CharT *> { using char_type = typename CharT; };

template <typename CharT>
struct chartype_trait<const CharT *> { using char_type = typename CharT; };

template <typename CharT, std::size_t N>
struct chartype_trait<CharT[N]> { using char_type = typename CharT; };

template <typename CharT, std::size_t N>
struct chartype_trait<const CharT[N]> { using char_type = typename CharT; };

template <typename CharT>
struct chartype_trait<std::basic_string<CharT>> { using char_type = typename CharT; };

template <typename CharT>
struct chartype_trait<std::basic_string_view<CharT>> { using char_type = typename CharT; };

template <typename T1, typename T2>
struct chartypes_same<T1, T2> { static constexpr bool value = std::is_same<chartype_trait<T1>::char_type, chartype_trait<T2>::char_type>::value; };

} // namespace zed

#endif // ZED_TYPE_TRAITS_HPP
