/**********************************************************************
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

#include "TransformNode.h"

namespace anari::rpr {

TransformNode::TransformNode(rpr_material_system matsys, mat4 transform_matrix, rpr_material_node input)
{
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot0))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot1))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot2))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot3))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_transform))

  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_r))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_g))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_b))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_a))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_rg))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_rgb))

  CHECK(rprMaterialNodeSetInputNByKey(m_dot0, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot1, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot2, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot3, RPR_MATERIAL_INPUT_COLOR0, input))

  CHECK(rprMaterialNodeSetInputFByKey(m_dot0, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][0],
      transform_matrix[1][0], transform_matrix[2][0], transform_matrix[3][0]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot1, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][1],
      transform_matrix[1][1], transform_matrix[2][1], transform_matrix[3][1]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot2, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][2],
      transform_matrix[1][2], transform_matrix[2][2], transform_matrix[3][2]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot3, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][3],
      transform_matrix[1][3], transform_matrix[2][3], transform_matrix[3][3]))

  CHECK(rprMaterialNodeSetInputUByKey(m_dot0, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot1, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot2, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot3, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))

  CHECK(rprMaterialNodeSetInputNByKey(m_r, RPR_MATERIAL_INPUT_COLOR0, m_dot0))
  CHECK(rprMaterialNodeSetInputNByKey(m_g, RPR_MATERIAL_INPUT_COLOR0, m_dot1))
  CHECK(rprMaterialNodeSetInputNByKey(m_b, RPR_MATERIAL_INPUT_COLOR0, m_dot2))
  CHECK(rprMaterialNodeSetInputNByKey(m_a, RPR_MATERIAL_INPUT_COLOR0, m_dot3))

  CHECK(rprMaterialNodeSetInputFByKey(m_r, RPR_MATERIAL_INPUT_COLOR1, 1, 0, 0, 0))
  CHECK(rprMaterialNodeSetInputFByKey(m_g, RPR_MATERIAL_INPUT_COLOR1, 0, 1, 0, 0))
  CHECK(rprMaterialNodeSetInputFByKey(m_b, RPR_MATERIAL_INPUT_COLOR1, 0, 0, 1, 0))
  CHECK(rprMaterialNodeSetInputFByKey(m_a, RPR_MATERIAL_INPUT_COLOR1, 0, 0, 0, 1))

  CHECK(rprMaterialNodeSetInputUByKey(m_r, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))
  CHECK(rprMaterialNodeSetInputUByKey(m_g, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))
  CHECK(rprMaterialNodeSetInputUByKey(m_b, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))
  CHECK(rprMaterialNodeSetInputUByKey(m_a, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))

  CHECK(rprMaterialNodeSetInputNByKey(m_rg, RPR_MATERIAL_INPUT_COLOR0, m_r))
  CHECK(rprMaterialNodeSetInputNByKey(m_rg, RPR_MATERIAL_INPUT_COLOR1, m_g))
  CHECK(rprMaterialNodeSetInputUByKey(m_rg, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))

  CHECK(rprMaterialNodeSetInputNByKey(m_rgb, RPR_MATERIAL_INPUT_COLOR0, m_rg))
  CHECK(rprMaterialNodeSetInputNByKey(m_rgb, RPR_MATERIAL_INPUT_COLOR1, m_b))
  CHECK(rprMaterialNodeSetInputUByKey(m_rgb, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))

  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR0, m_rgb))
  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR1, m_a))
  CHECK(rprMaterialNodeSetInputUByKey(m_transform, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))
}

rpr_material_node TransformNode::getMaterial()
{
  return m_transform;
}

TransformNode::~TransformNode()
{
  CHECK(rprObjectDelete(m_dot0))
  CHECK(rprObjectDelete(m_dot1))
  CHECK(rprObjectDelete(m_dot2))
  CHECK(rprObjectDelete(m_dot3))
  CHECK(rprObjectDelete(m_transform))
  CHECK(rprObjectDelete(m_r))
  CHECK(rprObjectDelete(m_g))
  CHECK(rprObjectDelete(m_b))
  CHECK(rprObjectDelete(m_a))
  CHECK(rprObjectDelete(m_rg))
  CHECK(rprObjectDelete(m_rgb))
}

} // namespace anari::rpr