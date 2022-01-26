// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Mesh.h"

namespace anari::rpr {

struct Triangles : public Mesh
{
  explicit Triangles(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
};

} // namespace anari
