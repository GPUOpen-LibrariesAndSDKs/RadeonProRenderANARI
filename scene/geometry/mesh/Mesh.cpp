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

#include "Quad.h"
#include "Triangles.h"

namespace anari::rpr {

Mesh::Mesh(rpr_context context, rpr_material_system materialSystem) : Geometry(context, materialSystem) {}

void Mesh::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  rpr_shape instance;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
  CHECK(rprShapeSetTransform(instance, false, value_ptr(transform)))
  out_shapes.push_back(instance);
}

template <typename vecType, int polySize>
void Mesh::meshCommit(vecType defaultVector)
{
  auto index_data  = getParamObject<Array1D>("primitive.index");
  auto normal_data = getParamObject<Array1D>("vertex.normal");
  auto vertex_data = getParamObject<Array1D>("vertex.position");

  if (!vertex_data)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

  calculateBounds(vertex_data);

  // process indices
  rpr_int             *index;
  std::size_t          num_faces;
  std::vector<vecType> default_index;

  if (index_data)
  {
    num_faces = index_data->size();
    index     = (rpr_int *)index_data->dataAs<vecType>();
  } else // 'index_data' array is missed. We should use default indices
  {
    default_index.reserve(vertex_data->size() / polySize);
    for (uint32_t i = 0; i < vertex_data->size(); i += polySize)
    {
      default_index.push_back(defaultVector + i);
    }
    num_faces = default_index.size();
    index     = (rpr_int *)default_index.data();
  }
  std::vector<rpr_int> faces(num_faces, polySize);

  // process normals
  rpr_float *normal         = normal_data ? (rpr_float *)normal_data->dataAs<vec3>() : nullptr;
  size_t     num_normals    = normal_data ? normal_data->size() : 0;
  size_t     normal_stride  = normal_data ? sizeof(rpr_float) * 3 : 0;
  rpr_int   *normal_indices = normal_data ? index : nullptr;
  size_t     nidx_stride    = normal_data ? sizeof(rpr_int) : 0;

  CHECK(rprContextCreateMesh(m_context, (rpr_float *)vertex_data->dataAs<vec3>(), vertex_data->size(),
      sizeof(rpr_float) * 3, normal, num_normals, normal_stride, nullptr, 0, 0, index, sizeof(rpr_int),
      normal_indices, nidx_stride, nullptr, 0, faces.data(), num_faces, &m_base_shape))

  processAttributes(vertex_data);

  CHECK(rprShapeSetVisibility(m_base_shape, false)) // base shape is always invisible
}

Attribute *Mesh::getAttribute(const char *name)
{
  Attribute *attribute = Geometry::getAttribute(name);
  if (attribute)
    return attribute;
  if (std::strcmp(name, "attribute0") == 0)
    return createPrimVarAttribute(0, name);
  if (std::strcmp(name, "attribute1") == 0)
    return createPrimVarAttribute(1, name);
  if (std::strcmp(name, "attribute2") == 0)
    return createPrimVarAttribute(2, name);
  if (std::strcmp(name, "attribute3") == 0)
    return createPrimVarAttribute(3, name);
  if (std::strcmp(name, "color") == 0)
    return createPrimVarAttribute(4, name);
  return nullptr;
}

bool Mesh::hasAttribute(const char *name)
{
  if (std::strcmp(name, "color") == 0 || std::strcmp(name, "attribute0") == 0
      || std::strcmp(name, "attribute1") == 0 || std::strcmp(name, "attribute2") == 0
      || std::strcmp(name, "attribute3") == 0)
  {
    return true;
  }
  return Geometry::hasAttribute(name);
}

void Mesh::calculateBounds(Array1D *vertex_data)
{
  resetBounds();

  for (int i = 0; i < vertex_data->size() * 3; i++)
  {
    float coordinate = ((float *)vertex_data->dataAs<vec3>())[i];
    if (i % 3 == 0)
    { // x coordinate
      m_bounds.upper.x = max(m_bounds.upper.x, coordinate);
      m_bounds.lower.x = min(m_bounds.lower.x, coordinate);
    }
    if (i % 3 == 1)
    { // y coordinate
      m_bounds.upper.y = max(m_bounds.upper.y, coordinate);
      m_bounds.lower.y = min(m_bounds.lower.y, coordinate);
    }
    if (i % 3 == 2)
    { // z coordinate
      m_bounds.upper.z = max(m_bounds.upper.z, coordinate);
      m_bounds.lower.z = min(m_bounds.lower.z, coordinate);
    }
  }
}

void Mesh::processAttributeArray(Array1D *data, int key)
{
  if (!data)
  {
    return;
  }
  std::vector<vec4> attributeData;
  attributeData.reserve(data->size());
  for (int i = 0; i < data->size(); i++)
  {
    attributeData.emplace_back(Attribute::processColor(data, i));
  }
  CHECK(rprShapeSetPrimvar(m_base_shape, key, (rpr_float *)attributeData.data(), attributeData.size() * 4, 4,
      RPR_PRIMVAR_INTERPOLATION_VERTEX))
}

void Mesh::processAttributes(Array1D *vertex_data)
{
  auto color_data      = getParamObject<Array1D>("vertex.color");
  auto attribute0_data = getParamObject<Array1D>("vertex.attribute0");
  auto attribute1_data = getParamObject<Array1D>("vertex.attribute1");
  auto attribute2_data = getParamObject<Array1D>("vertex.attribute2");
  auto attribute3_data = getParamObject<Array1D>("vertex.attribute3");

  checkArraySizes(color_data, vertex_data->size(),
      std::runtime_error("'vertex.position' and 'vertex.color' sizes are incompatible"));
  checkArraySizes(attribute0_data, vertex_data->size(),
      std::runtime_error("'vertex.position' and 'vertex.attribute0' sizes are incompatible"));
  checkArraySizes(attribute1_data, vertex_data->size(),
      std::runtime_error("'vertex.position' and 'vertex.attribute1' sizes are incompatible"));
  checkArraySizes(attribute2_data, vertex_data->size(),
      std::runtime_error("'vertex.position' and 'vertex.attribute2' sizes are incompatible"));
  checkArraySizes(attribute3_data, vertex_data->size(),
      std::runtime_error("'vertex.position' and 'vertex.attribute3' sizes are incompatible"));

  processAttributeArray(attribute0_data, 0);
  processAttributeArray(attribute1_data, 1);
  processAttributeArray(attribute2_data, 2);
  processAttributeArray(attribute3_data, 3);
  processAttributeArray(color_data, 4);
}

void Triangles::commit()
{
  Mesh::meshCommit<uvec3, 3>(uvec3(0, 1, 2));
}

void Quad::commit()
{
  Mesh::meshCommit<uvec4, 4>(uvec4(0, 1, 2, 3));
}

} // namespace anari::rpr
