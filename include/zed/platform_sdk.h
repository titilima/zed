#pragma once
// -------------------------------------------------
// ZED Kit
// -------------------------------------------------
//   File Name: platform_sdk.h
//      Author: Ziming Li
//     Created: 2021-02-11
// -------------------------------------------------
// Copyright (C) 2021 MingYang Software Technology.
// -------------------------------------------------

#ifndef ZED_PLATFORM_SDK_H
#define ZED_PLATFORM_SDK_H

#include "./build_macros.h"

#ifdef _Z_OS_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#endif

#endif // ZED_PLATFORM_SDK_H
