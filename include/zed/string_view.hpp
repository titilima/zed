#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: string_view.hpp
//      Author: Ziming Li
//     Created: 2021-02-13
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_STRING_VIEW_HPP
#define ZED_STRING_VIEW_HPP

#if defined(_MSC_VER)
#   if _MSC_VER >= 1910 && _MSVC_LANG > 201402
#       define _Z_STRING_VIEW_ENABLED
#   endif
#elif defined(__has_include)
#   if __has_include(<string_view>)
#       define _Z_STRING_VIEW_ENABLED
#   endif
#endif

#ifdef _Z_STRING_VIEW_ENABLED
#   include <string_view>
#endif

#endif // ZED_STRING_VIEW_HPP
