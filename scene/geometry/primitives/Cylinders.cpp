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

#include "Cylinders.h"

namespace anari::rpr {

Cylinders::Cylinders(rpr_context context, rpr_material_system materialSystem) : Cones(context, materialSystem)
{}

void Cylinders::commit()
{
  m_vertices     = getParamObject<Array1D>("vertex.position");
  m_radius       = getParamObject<Array1D>("primitive.radius");
  m_indices      = getParamObject<Array1D>("primitive.index");
  m_caps         = getParamObject<Array1D>("vertex.cap");
  m_globalRadius = getParam<float32>("radius", 1.f);
  m_globalCaps   = processConeCaps(getParam<std::string>("caps", "none"));

  if (!m_vertices)
    throw std::runtime_error("missing 'vertex.position' on cylinder geometry");

  clearAttributes();
  resetBounds();
  m_num_primitives = m_indices ? m_indices->size() : m_vertices->size() / 2;

  for (int primitiveNumber = 0; primitiveNumber < m_num_primitives; primitiveNumber++)
  {
    uvec2   index        = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber]
                                     : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
    vec3    firstVertex  = m_vertices->dataAs<vec3>()[index.x];
    vec3    secondVertex = m_vertices->dataAs<vec3>()[index.y];
    float32 radius       = m_radius ? m_radius->dataAs<float32>()[primitiveNumber] : m_globalRadius;
    extendBounds(calculateConeBounds(firstVertex, secondVertex, radius, radius));
  }
  processAttributeParameters(nullptr);
}

rpr_shape Cylinders::getPrimitive(int primitiveNumber, mat4 externalTransform)
{
  uvec2     index        = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber]
                                     : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
  vec3      firstVertex  = m_vertices->dataAs<vec3>()[index.x];
  vec3      secondVertex = m_vertices->dataAs<vec3>()[index.y];
  float32   radius       = m_radius ? m_radius->dataAs<float32>()[primitiveNumber] : m_globalRadius;
  uint8     firstCap     = m_caps ? m_caps->dataAs<uint8>()[index.x] : m_globalCaps.x;
  uint8     secondCap    = m_caps ? m_caps->dataAs<uint8>()[index.y] : m_globalCaps.y;
  rpr_shape cylinder     = createConeShape(
          firstVertex, secondVertex, radius, radius, firstCap, secondCap, externalTransform, m_numSegments);
  processConeAttributes(cylinder, index);
  return cylinder;
}

} // namespace anari::rpr