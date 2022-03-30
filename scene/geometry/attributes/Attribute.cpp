#include "Attribute.h"

namespace anari::rpr{

Attribute::Attribute(rpr_material_node materialNode) : m_material(materialNode){}

rpr_material_node Attribute::getMaterial()
{
  return m_material;
}

Attribute::~Attribute()
{
  if(m_material)
  {
    CHECK(rprObjectDelete(m_material))
  }
}

Attribute *Attribute::fromType(rpr_material_system matsys, rpr_material_node_lookup_value type)
{
  rpr_material_node materialNode;
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &materialNode))
  CHECK(rprMaterialNodeSetInputUByKey(materialNode, RPR_MATERIAL_INPUT_VALUE, type))
  return new Attribute(materialNode);
}

vec4 Attribute::processColor(Array1D *array, size_t index)
{
  switch (array->elementType()){
  case ANARI_FLOAT32: return {array->dataAs<float32>()[index], 0, 0, 1};
  case ANARI_FLOAT32_VEC2: return {array->dataAs<vec2>()[index], 0, 1};
  case ANARI_FLOAT32_VEC3: return {array->dataAs<vec3>()[index], 1};
  case ANARI_FLOAT32_VEC4: return array->dataAs<vec4>()[index];
  default: throw std::runtime_error("Unsupported data type");
  }
}

}
