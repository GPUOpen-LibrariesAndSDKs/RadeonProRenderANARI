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
};

}


