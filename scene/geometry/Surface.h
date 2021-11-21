// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../rpr_common.h"
#include "../../Object.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

struct Surface : public Object
{
  friend struct Instance;

  explicit Surface(rpr_material_system materialSystem) : m_matsys(materialSystem){};

  void commit() override;

  void addToScene(rpr_scene scene) override;

  ~Surface() override;

 private:
  IntrusivePtr<Geometry> m_geometry;
  IntrusivePtr<Material> m_material;
  std::vector<rpr_shape> m_shapes;
  vec3 m_upper_bound{};
  vec3 m_lower_bound{};

  rpr_material_system m_matsys;
  rpr_material_node m_material_instance;
  //vertex color
  rpr_material_node m_vertex_color_lookup_r = nullptr;
  rpr_material_node m_vertex_color_lookup_g = nullptr;
  rpr_material_node m_vertex_color_lookup_b = nullptr;
  rpr_material_node m_vertex_color_lookup_a = nullptr;
  rpr_material_node m_vertex_color_r = nullptr;
  rpr_material_node m_vertex_color_g = nullptr;
  rpr_material_node m_vertex_color_b = nullptr;
  rpr_material_node m_vertex_color_a = nullptr;
  rpr_material_node m_vertex_color_rg = nullptr;
  rpr_material_node m_vertex_color_rgb = nullptr;
  rpr_material_node m_vertex_color = nullptr;

  void clearMaterialNodesVertex();
  void generateVertexColorNode();
};


} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Surface *, ANARI_SURFACE);

} // namespace anari
