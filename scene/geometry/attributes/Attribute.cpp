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

}