// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef _WIN32
#ifdef RPR_DEVICE_STATIC_DEFINE
#define RPR_DEVICE_INTERFACE
#else
#ifdef rpr_device_EXPORTS
#define RPR_DEVICE_INTERFACE __declspec(dllexport)
#else
#define RPR_DEVICE_INTERFACE __declspec(dllimport)
#endif
#endif
#elif defined __GNUC__
#define RPR_DEVICE_INTERFACE __attribute__((__visibility__("default")))
#else
#define RRP_DEVICE_INTERFACE
#endif
