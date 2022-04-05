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

#include "Attribute.h"

namespace anari::rpr {

Attribute::Attribute(rpr_material_node materialNode) : m_material(materialNode) {}

rpr_material_node Attribute::getMaterial()
{
  return m_material;
}

Attribute::~Attribute()
{
  if (m_material)
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
  switch (array->elementType())
  {
  case ANARI_FLOAT32:
    return {array->dataAs<float32>()[index], 0, 0, 1};
  case ANARI_FLOAT32_VEC2:
    return {array->dataAs<vec2>()[index], 0, 1};
  case ANARI_FLOAT32_VEC3:
    return {array->dataAs<vec3>()[index], 1};
  case ANARI_FLOAT32_VEC4:
    return array->dataAs<vec4>()[index];
  default:
    throw std::runtime_error("Unsupported data type");
  }
}

} // namespace anari::rpr
