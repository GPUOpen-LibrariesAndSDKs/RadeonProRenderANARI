// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Light.h"

namespace anari::rpr {

struct QuadLight : public Light
{
  QuadLight(rpr_context & context, rpr_material_system & matsys);
  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform) override;
  ~QuadLight() override;

 private:
  rpr_material_node m_material{};
  rpr_material_system m_matsys;

  std::vector<vec3> m_vertex;
  std::vector<vec3> m_normal;
  rpr_int m_faces[1] = {4};
  rpr_int m_index[4] = {0, 1, 3, 2};

  void clear();
};

} // namespace anari
