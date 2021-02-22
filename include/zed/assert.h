#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: assert.h
//      Author: Ziming Li
//     Created: 2021-02-22
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_ASSERT_H
#define ZED_ASSERT_H

#include <cassert>
#include "./build_macros.h"

#ifdef _Z_OS_WINDOWS
#   ifndef NDEBUG
#       define ZASSERT(e)   (!(e) ? __debugbreak() : (void)0)
#   endif
#endif

#ifndef ZASSERT
#   define ZASSERT  assert
#endif

#endif // ZED_ASSERT_H
