// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"

namespace anari {
namespace rpr {

  struct Future : public Object
  {
    Future() = default;
    float m_duration{1.f};
  };

} // namespace reference
} // namespace anari