// Copyright 2020 The Khronos GeometricModel
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../material/Material.h"
#include "../../rpr_common.h"
#include "../Instance.h"

namespace anari {
namespace rpr {

struct GeometricModel : public Object
{
  friend struct Instance;

  GeometricModel(rpr_context &context) : m_context(context) {};

  void commit() override;

  inline box3 bounds() const override{
    return box3(m_lower_bound, m_upper_bound);
  }

  ~GeometricModel();

 private:
  rpr_context m_context;
  std::vector<rpr_shape> m_shapes;
  vec3 m_upper_bound;
  vec3 m_lower_bound;
  rpr_material_node m_material = nullptr;
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
  //instance color
  rpr_buffer m_color_buffer = nullptr;
  rpr_material_node m_id_lookup = nullptr;
  rpr_material_node m_color_sampler = nullptr;

  void clearMaterialNodesVertex();
  void clearMaterialNodeInstance();
};


} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::GeometricModel *, ANARI_GEOMETRIC_MODEL);

} // namespace anari
