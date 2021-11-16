// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace anari {
namespace rpr {

struct Perspective : public Camera
{
  Perspective(rpr_camera &camera);

  void commit() override;
};

} // namespace reference
} // namespace anari
