#include "TransformNode.h"

namespace anari::rpr{

TransformNode::TransformNode(rpr_material_system matsys, mat4 transform_matrix, rpr_material_node input)
{
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot0))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot1))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot2))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_dot3))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_transform))

  CHECK(rprMaterialNodeSetInputNByKey(m_dot0, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot1, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot2, RPR_MATERIAL_INPUT_COLOR0, input))
  CHECK(rprMaterialNodeSetInputNByKey(m_dot3, RPR_MATERIAL_INPUT_COLOR0, input))

  CHECK(rprMaterialNodeSetInputFByKey(m_dot0, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][0], transform_matrix[1][0], transform_matrix[2][0], transform_matrix[3][0]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot1, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][1], transform_matrix[1][1], transform_matrix[2][1], transform_matrix[3][1]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot2, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][2], transform_matrix[1][2], transform_matrix[2][2], transform_matrix[3][2]))
  CHECK(rprMaterialNodeSetInputFByKey(m_dot3, RPR_MATERIAL_INPUT_COLOR1, transform_matrix[0][3], transform_matrix[1][3], transform_matrix[2][3], transform_matrix[3][3]))

  CHECK(rprMaterialNodeSetInputUByKey(m_dot0, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot1, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot2, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))
  CHECK(rprMaterialNodeSetInputUByKey(m_dot3, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT4))

  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR0,m_dot0))
  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR1,m_dot1))
  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR2,m_dot2))
  CHECK(rprMaterialNodeSetInputNByKey(m_transform, RPR_MATERIAL_INPUT_COLOR3,m_dot3))
  CHECK(rprMaterialNodeSetInputUByKey(m_transform, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_COMBINE))
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
}

}