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

#include "Spheres.h"
#include "Math/mathutils.h"

namespace anari::rpr {

Spheres::Spheres(rpr_context context, rpr_material_system materialSystem)
    : Primitives(context, materialSystem)
{
  createBaseShape();
}

void Spheres::commit()
{
  m_vertices     = getParamObject<Array1D>("vertex.position");
  m_radius       = getParamObject<Array1D>("vertex.radius");
  m_indices      = getParamObject<Array1D>("primitive.index");
  m_globalRadius = getParam<float>("radius", 1.f);

  if (!m_vertices)
    throw std::runtime_error("missing 'vertex.position' on sphere geometry");
  clearAttributes();

  resetBounds();

  m_num_primitives = m_indices ? m_indices->size() : m_vertices->size();

  for (int primitiveNumber = 0; primitiveNumber < m_num_primitives; primitiveNumber++)
  {
    uint32 index  = m_indices ? m_indices->dataAs<uint32>()[primitiveNumber] : primitiveNumber;
    vec3   vertex = m_vertices->dataAs<vec3>()[index];
    float  radius = m_radius ? m_radius->dataAs<float32>()[index] : m_globalRadius;

    // bounds
    m_bounds.upper.x = max(m_bounds.upper.x, vertex.x + radius);
    m_bounds.upper.y = max(m_bounds.upper.y, vertex.y + radius);
    m_bounds.upper.z = max(m_bounds.upper.z, vertex.z + radius);
    m_bounds.lower.x = min(m_bounds.lower.x, vertex.x - radius);
    m_bounds.lower.y = min(m_bounds.lower.y, vertex.y - radius);
    m_bounds.lower.z = min(m_bounds.lower.z, vertex.z - radius);
  }

  processAttributeParameters(m_indices);
}

rpr_shape Spheres::getPrimitive(int primitiveNumber, mat4 externalTransform)
{
  rpr_shape primitive;
  uint32    index  = m_indices ? m_indices->dataAs<uint32>()[primitiveNumber] : primitiveNumber;
  vec3      vertex = m_vertices->dataAs<vec3>()[index];
  float     radius = m_radius ? m_radius->dataAs<float32>()[index] : m_globalRadius;

  mat4 transform =
      transpose(mat4(radius, 0, 0, vertex.x, 0, radius, 0, vertex.y, 0, 0, radius, vertex.z, 0, 0, 0, 1));
  transform *= externalTransform;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &primitive))
  CHECK(rprShapeSetTransform(primitive, false, value_ptr(transform)))
  return primitive;
}

void Spheres::createBaseShape()
{ // creates base sphere with center 0,0,0 and radius 1
  float const R    = 1.f / (float)(m_rings - 1);
  float const S    = 1.f / (float)(m_sectors - 1);
  auto const  m_pi = pi<float>();

  std::vector<rpr_float> vertices;
  std::vector<rpr_int>   indices;

  int num_faces = (m_rings - 1) * (m_sectors - 1);

  vertices.resize(m_rings * m_sectors * 3);
  indices.resize(num_faces * 4);

  auto v = vertices.begin();
  auto i = indices.begin();

  for (int r = 0; r < m_rings; r++)
    for (int s = 0; s < m_sectors; s++)
    {
      float const y = sin((m_pi / 2.f) + m_pi * R * r);
      float const x = cos(2 * m_pi * s * S) * sin(m_pi * r * R);
      float const z = sin(2 * m_pi * s * S) * sin(m_pi * r * R);

      *v++ = x;
      *v++ = y;
      *v++ = z;
    }

  for (int r = 0; r < m_rings - 1; r++)
    for (int s = 0; s < m_sectors - 1; s++)
    {
      *i++ = r * m_sectors + s;
      *i++ = r * m_sectors + (s + 1);
      *i++ = (r + 1) * m_sectors + (s + 1);
      *i++ = (r + 1) * m_sectors + s;
    }

  std::vector<rpr_int> faces(num_faces, 4);

  // normal coordinates are same with vertex coordinates because center of sphere is (0,0,0)
  CHECK(rprContextCreateMesh(m_context, (rpr_float *)vertices.data(), vertices.size() / 3,
      sizeof(rpr_float) * 3, (rpr_float *)vertices.data(), vertices.size() / 3, sizeof(rpr_float) * 3,
      nullptr, 0, 0, (rpr_int *)indices.data(), sizeof(rpr_int), (rpr_int *)indices.data(), sizeof(rpr_int),
      nullptr, 0, faces.data(), num_faces, &m_base_shape))
  CHECK(rprShapeSetVisibility(
      m_base_shape, false)) // this is invisible 'original' sphere. It's instances will be visible
}

Attribute *Spheres::getAttribute(const char *name)
{
  Attribute *geometryAttribute = Geometry::getAttribute(name);
  if (geometryAttribute)
    return geometryAttribute;

  if (std::strcmp(name, "color") == 0)
    return createPerShapeAttribute(m_colors, name);
  if (std::strcmp(name, "attribute0") == 0)
    return createPerShapeAttribute(m_attribute0, name);
  if (std::strcmp(name, "attribute1") == 0)
    return createPerShapeAttribute(m_attribute1, name);
  if (std::strcmp(name, "attribute2") == 0)
    return createPerShapeAttribute(m_attribute2, name);
  if (std::strcmp(name, "attribute3") == 0)
    return createPerShapeAttribute(m_attribute3, name);

  if (std::strcmp(name, "primitiveId") == 0)
  {
    Attribute *attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }

  return nullptr;
}

} // namespace anari::rpr
