#include "Cones.h"

namespace anari::rpr{

Cones::Cones(rpr_context context, rpr_material_system materialSystem) : Primitives(context, materialSystem){}

void Cones::commit()
{
  m_vertices = getParamObject<Array1D>("vertex.position");
  m_radius = getParamObject<Array1D>("vertex.radius");
  m_indices = getParamObject<Array1D>("primitive.index");
  m_caps = getParamObject<Array1D>("vertex.cap");

  if (!m_vertices) throw std::runtime_error("missing 'vertex.position' on cone geometry");
  if (!m_radius) throw std::runtime_error("missing 'vertex.radius' on cone geometry");


  clearAttributes();
  resetBounds();
  m_num_primitives = m_indices ? m_indices->size() : m_vertices->size() / 2;

  for (int primitiveNumber = 0; primitiveNumber < m_num_primitives; primitiveNumber++)
  {
    uvec2 index = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber] : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
    vec3 downVertex = m_vertices->dataAs<vec3>()[index.x];
    vec3 upVertex = m_vertices->dataAs<vec3>()[index.y];
    float32 downRadius = m_radius->dataAs<float32>()[index.x];
    float32 upRadius = m_radius->dataAs<float32>()[index.y];
    extendBounds(
        calculateConeBounds(upVertex, downVertex, upRadius, downRadius));
  }
}

rpr_shape Cones::getPrimitive(int primitiveNumber, mat4 externalTransform)
{
  uvec2 index = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber] : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
  vec3 downVertex = m_vertices->dataAs<vec3>()[index.x];
  vec3 upVertex = m_vertices->dataAs<vec3>()[index.y];
  float32 downRadius = m_radius->dataAs<float32>()[index.x];
  float32 upRadius = m_radius->dataAs<float32>()[index.y];
  uint8 downCap = m_caps ? m_caps->dataAs<uint8>()[index.x] : 0;
  uint8 upCap = m_caps ? m_caps->dataAs<uint8>()[index.y] : 0;
  return createConeShape(upVertex, downVertex, upRadius, downRadius, upCap, downCap, externalTransform, 32);
}

}
