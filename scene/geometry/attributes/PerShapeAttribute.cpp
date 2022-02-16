#include "PerShapeAttribute.h"

namespace anari::rpr{

PerShapeAttribute::PerShapeAttribute(rpr_context context, rpr_material_system matsys, rpr_uint num_primitives, const float *data)
{
  rpr_buffer_desc bufferDesc;
  bufferDesc.element_channel_size = 4;
  bufferDesc.element_type = RPR_BUFFER_ELEMENT_TYPE_FLOAT32;
  bufferDesc.nb_element = num_primitives;

  CHECK(rprContextCreateBuffer(context, &bufferDesc, data, &m_buffer))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_lookupNode))
  CHECK(rprMaterialNodeSetInputUByKey(m_lookupNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID))
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_BUFFER_SAMPLER, &m_material))
  CHECK(rprMaterialNodeSetInputNByKey(m_material, RPR_MATERIAL_INPUT_UV, m_lookupNode))
  CHECK(rprMaterialNodeSetInputBufferDataByKey(m_material, RPR_MATERIAL_INPUT_DATA, m_buffer))
}

PerShapeAttribute::~PerShapeAttribute()
{
  if(m_lookupNode){
    CHECK(rprObjectDelete(m_lookupNode))
  }
  if(m_buffer){
    CHECK(rprObjectDelete(m_buffer))
  }
}

}
