// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
// bvh

namespace anari {
namespace rpr {

struct Mesh : public Geometry
{
  Mesh(rpr_context & context);

  void commit() override;

  ~Mesh();

};

} // namespace rpr
} // namespace anari
