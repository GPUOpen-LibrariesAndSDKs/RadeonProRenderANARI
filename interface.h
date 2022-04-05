/**********************************************************************
Copyright 2021 The Khronos Group
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

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
