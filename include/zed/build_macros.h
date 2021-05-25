#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: build_macros.h
//      Author: Ziming Li
//     Created: 2021-02-05
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_BUILD_MACROS_H
#define ZED_BUILD_MACROS_H

/**
 * OS
 */

#if defined(_WIN32)
#   define _Z_OS_WINDOWS
#   ifdef __cplusplus
#       define NOMINMAX
#   endif
#elif defined(__APPLE__)
#   define _Z_OS_APPLE
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define _Z_OS_IOS
#   else
#       define _Z_OS_MACOS
#   endif
#   define _Z_OS_POSIX
#elif defined(__linux__)
#   define _Z_OS_LINUX
#   define _Z_OS_POSIX
#else
#   error OS is not detected!
#endif

/**
 * C++ Versions
 */

#define _Z_CPP_DEFAULT  0
#define _Z_CPP_14       1
#define _Z_CPP_17       2
#define _Z_CPP_20       3

#ifdef __cplusplus
#   if defined(_MSC_VER)
#       include <vcruntime.h>
#       if defined(_HAS_CXX20) && (_HAS_CXX20)
#           define _Z_CPP   _Z_CPP_20
#       elif defined(_HAS_CXX17) && (_HAS_CXX17)
#           define _Z_CPP   _Z_CPP_17
#       endif
#   else
#       pragma message("TODO: Adjust _Z_CPP.")
#   endif
#endif
#ifndef _Z_CPP
#   define _Z_CPP   _Z_CPP_DEFAULT
#endif

/*
 * _Z_STRING_VIEW_ENABLED
 */

#ifdef __cplusplus
#   if defined(_MSC_VER)
#       if _MSC_VER >= 1910 && _MSVC_LANG > 201402
#           define _Z_STRING_VIEW_ENABLED
#       endif
#   elif defined(__has_include)
#       if __has_include(<string_view>)
#           define _Z_STRING_VIEW_ENABLED
#       endif
#   endif
#endif

#endif // ZED_BUILD_MACROS_H
