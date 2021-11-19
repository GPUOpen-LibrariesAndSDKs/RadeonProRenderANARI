// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
// bvh

namespace anari::rpr {

struct Mesh : public Geometry
{
  explicit Mesh(rpr_context & context);

  void commit() override;

  ~Mesh() override;

};

} // namespace anari
