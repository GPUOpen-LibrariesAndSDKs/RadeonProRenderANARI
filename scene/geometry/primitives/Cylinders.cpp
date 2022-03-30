#include "Cylinders.h"

namespace anari::rpr{

Cylinders::Cylinders(rpr_context context, rpr_material_system materialSystem) : Cones(context, materialSystem){}

void Cylinders::commit()
{
  m_vertices = getParamObject<Array1D>("vertex.position");
  m_radius = getParamObject<Array1D>("primitive.radius");
  m_indices = getParamObject<Array1D>("primitive.index");
  m_caps = getParamObject<Array1D>("vertex.cap");
  m_globalRadius = getParam<float32>("radius", 1.f);
  m_globalCaps = processConeCaps(getParam<std::string>("caps", "none"));

  if (!m_vertices) throw std::runtime_error("missing 'vertex.position' on cylinder geometry");

  clearAttributes();
  resetBounds();
  m_num_primitives = m_indices ? m_indices->size() : m_vertices->size() / 2;

  for (int primitiveNumber = 0; primitiveNumber < m_num_primitives; primitiveNumber++)
  {
    uvec2 index = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber] : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
    vec3 firstVertex = m_vertices->dataAs<vec3>()[index.x];
    vec3 secondVertex = m_vertices->dataAs<vec3>()[index.y];
    float32 radius = m_radius ? m_radius->dataAs<float32>()[primitiveNumber] : m_globalRadius;
    extendBounds(calculateConeBounds(firstVertex, secondVertex, radius, radius));
  }
  processAttributeParameters(nullptr);
}

rpr_shape Cylinders::getPrimitive(int primitiveNumber, mat4 externalTransform)
{
    uvec2 index = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber] : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
    vec3 firstVertex = m_vertices->dataAs<vec3>()[index.x];
    vec3 secondVertex = m_vertices->dataAs<vec3>()[index.y];
    float32 radius = m_radius ? m_radius->dataAs<float32>()[primitiveNumber] : m_globalRadius;
    uint8 firstCap = m_caps ? m_caps->dataAs<uint8>()[index.x] : m_globalCaps.x;
    uint8 secondCap = m_caps ? m_caps->dataAs<uint8>()[index.y] : m_globalCaps.y;
    rpr_shape cylinder = createConeShape(firstVertex, secondVertex, radius, radius, firstCap, secondCap, externalTransform, m_numSegments);
    processConeAttributes(cylinder, index);
    return cylinder;
}

}