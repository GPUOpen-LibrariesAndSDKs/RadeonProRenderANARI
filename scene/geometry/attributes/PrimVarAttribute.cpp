#include "PrimVarAttribute.h"

namespace anari::rpr{

PrimVarAttribute::PrimVarAttribute(rpr_material_system matsys, int key)
{
  CHECK(rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_PRIMVAR_LOOKUP, &m_material))
  CHECK(rprMaterialNodeSetInputUByKey(m_material, RPR_MATERIAL_INPUT_VALUE, key))
}

}
