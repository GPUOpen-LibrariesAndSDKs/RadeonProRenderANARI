// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "Material.h"

namespace anari::rpr {

struct OBJMaterial : public Material{
  explicit OBJMaterial(rpr_material_system);

  void commit() override;

 private:
  rpr_material_node generateMaterial(rpr_material_node vertex_color) override;

  vec3 m_diffuse_color{};
  vec3 m_specular_color{};
  vec3 m_transparency_color{};
  float m_roughness{};
  float m_transparency{};

};

}
