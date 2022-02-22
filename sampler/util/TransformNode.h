#pragma once
#include "../../rpr_common.h"
#include "../../glm_extras.h"

namespace anari::rpr{

struct TransformNode
{

public:
  TransformNode(rpr_material_system matsys, mat4 transform_matrix, rpr_material_node input);
  ~TransformNode();

  rpr_material_node getMaterial();


private:
  rpr_material_node m_dot0{};
  rpr_material_node m_dot1{};
  rpr_material_node m_dot2{};
  rpr_material_node m_dot3{};
  rpr_material_node m_transform{};

  // TODO remove it after combine operation fix
  rpr_material_node m_r{};
  rpr_material_node m_g{};
  rpr_material_node m_b{};
  rpr_material_node m_a{};
  rpr_material_node m_rg{};
  rpr_material_node m_rgb{};
};

}


