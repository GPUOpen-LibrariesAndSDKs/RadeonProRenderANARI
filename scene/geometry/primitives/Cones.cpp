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

#include "Cones.h"

namespace anari::rpr {

const static std::map<std::string, uvec2> cone_cap_types = {
    {"none", {0, 0}}, {"first", {1, 0}}, {"second", {0, 1}}, {"both", {1, 1}}};

Cones::Cones(rpr_context context, rpr_material_system materialSystem) : Primitives(context, materialSystem) {}

void Cones::commit()
{
  m_vertices   = getParamObject<Array1D>("vertex.position");
  m_radius     = getParamObject<Array1D>("vertex.radius");
  m_indices    = getParamObject<Array1D>("primitive.index");
  m_caps       = getParamObject<Array1D>("vertex.cap");
  m_globalCaps = processConeCaps(getParam<std::string>("caps", "none"));

  if (!m_vertices)
    throw std::runtime_error("missing 'vertex.position' on cone geometry");
  if (!m_radius)
    throw std::runtime_error("missing 'vertex.radius' on cone geometry");

  clearAttributes();
  resetBounds();
  m_num_primitives = m_indices ? m_indices->size() : m_vertices->size() / 2;

  for (int primitiveNumber = 0; primitiveNumber < m_num_primitives; primitiveNumber++)
  {
    uvec2   index        = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber]
                                     : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
    vec3    firstVertex  = m_vertices->dataAs<vec3>()[index.x];
    vec3    secondVertex = m_vertices->dataAs<vec3>()[index.y];
    float32 firstRadius  = m_radius->dataAs<float32>()[index.x];
    float32 secondRadius = m_radius->dataAs<float32>()[index.y];
    extendBounds(calculateConeBounds(firstVertex, secondVertex, firstRadius, secondRadius));
  }
  processAttributeParameters(nullptr);
}

rpr_shape Cones::createConeShape(vec3 firstVertex, vec3 secondVertex, float firstRadius, float secondRadius,
    bool firstCap, bool secondCap, mat4 externalTransform, int numSegments)
{
  vec3 direction = normalize(secondVertex - firstVertex);
  mat4 transform = translate(mat4(1), (secondVertex + firstVertex) * 0.5f); // middle point between vertices
  transform *= calculateRotation(direction, vec3(0, 1, 0));
  transform *= externalTransform;
  const float height = length(secondVertex - firstVertex);

  std::vector<vec3>    vertices;
  std::vector<vec3>    normals;
  std::vector<rpr_int> indices;
  std::vector<rpr_int> normalIndices;
  std::vector<rpr_int> faces(numSegments, 4);
  vertices.resize(numSegments * 2);
  normals.resize(numSegments * 2);
  indices.resize(numSegments * 4);
  normalIndices.resize(numSegments * 4);

  float       angle      = 0;
  const float sectorSize = pi<float>() * 2.f / (float)numSegments;
  for (int segmentNumber = 0; segmentNumber < numSegments; segmentNumber++)
  {
    vertices[segmentNumber] = vec3(firstRadius * sin(angle), -height * 0.5f, firstRadius * cos(angle));
    vertices[segmentNumber + numSegments] =
        vec3(secondRadius * sin(angle), height * 0.5f, secondRadius * cos(angle));

    normals[segmentNumber]               = vertices[segmentNumber];
    normals[segmentNumber].y             = firstRadius * (firstRadius - secondRadius) / height;
    normals[segmentNumber + numSegments] = normals[segmentNumber];

    indices[segmentNumber * 4]     = segmentNumber;
    indices[segmentNumber * 4 + 1] = segmentNumber + 1;
    indices[segmentNumber * 4 + 2] = segmentNumber + numSegments + 1;
    indices[segmentNumber * 4 + 3] = segmentNumber + numSegments;

    normalIndices[segmentNumber * 4]     = segmentNumber;
    normalIndices[segmentNumber * 4 + 1] = segmentNumber + 1;
    normalIndices[segmentNumber * 4 + 2] = segmentNumber + numSegments + 1;
    normalIndices[segmentNumber * 4 + 3] = segmentNumber + numSegments;

    angle += sectorSize;
  }
  indices[numSegments * 4 - 2]       = numSegments;
  indices[numSegments * 4 - 3]       = 0;
  normalIndices[numSegments * 4 - 2] = numSegments;
  normalIndices[numSegments * 4 - 3] = 0;

  // central points for caps
  vertices.emplace_back(0, -height * 0.5f, 0);
  vertices.emplace_back(0, height * 0.5f, 0);
  normals.emplace_back(0, -1, 0);
  normals.emplace_back(0, 1, 0);

  if (firstCap && firstRadius > 0)
  {
    for (int i = 0; i < numSegments; i++)
    {
      indices.push_back(i);
      indices.push_back(i == numSegments - 1 ? 0 : i + 1);
      indices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      faces.push_back(3);
    }
  }

  if (secondCap && secondRadius > 0)
  {
    for (int i = 0; i < numSegments; i++)
    {
      indices.push_back(i + numSegments);
      indices.push_back(i == numSegments - 1 ? numSegments : i + 1 + numSegments);
      indices.push_back(numSegments * 2 + 1);
      normalIndices.push_back(numSegments * 2 + 1);
      normalIndices.push_back(numSegments * 2 + 1);
      normalIndices.push_back(numSegments * 2 + 1);
      faces.push_back(3);
    }
  }

  rpr_shape shape;
  CHECK(rprContextCreateMesh(m_context, (rpr_float *)vertices.data(), vertices.size(), sizeof(rpr_float) * 3,
      (rpr_float *)normals.data(), normals.size(), sizeof(rpr_float) * 3, nullptr, 0, 0, indices.data(),
      sizeof(rpr_int), normalIndices.data(), sizeof(rpr_int), nullptr, 0, faces.data(), faces.size(), &shape))
  CHECK(rprShapeSetTransform(shape, false, value_ptr(transform)))

  return shape;
}

box3 Cones::calculateConeBounds(vec3 firstVertex, vec3 secondVertex, float firstRadius, float secondRadius)
{
  vec3 direction = normalize(secondVertex - firstVertex);
  mat4 transform = translate(mat4(1), (secondVertex + firstVertex) * 0.5f); // middle point between vertices
  transform *= calculateRotation(direction, vec3(0, 1, 0));
  const float height    = length(secondVertex - firstVertex);
  const float maxRadius = max(secondRadius, firstRadius);

  box3 coneBounds;
  coneBounds.upper = vec3(maxRadius, height / 2, maxRadius);
  coneBounds.lower = -coneBounds.upper;

  return xfmBox(transform, coneBounds);
}

rpr_shape Cones::getPrimitive(int primitiveNumber, mat4 externalTransform)
{
  uvec2     index        = m_indices ? m_indices->dataAs<uvec2>()[primitiveNumber]
                                     : uvec2(primitiveNumber * 2, primitiveNumber * 2 + 1);
  vec3      firstVertex  = m_vertices->dataAs<vec3>()[index.x];
  vec3      secondVertex = m_vertices->dataAs<vec3>()[index.y];
  float32   firstRadius  = m_radius->dataAs<float32>()[index.x];
  float32   secondRadius = m_radius->dataAs<float32>()[index.y];
  uint8     firstCap     = m_caps ? m_caps->dataAs<uint8>()[index.x] : m_globalCaps.x;
  uint8     secondCap    = m_caps ? m_caps->dataAs<uint8>()[index.y] : m_globalCaps.y;
  rpr_shape cone = createConeShape(firstVertex, secondVertex, firstRadius, secondRadius, firstCap, secondCap,
      externalTransform, m_numSegments);
  CHECK(rprShapeSetObjectID(cone, primitiveNumber))
  processConeAttributes(cone, index);
  return cone;
}

Attribute *Cones::getAttribute(const char *name)
{
  Attribute *geometryAttribute = Geometry::getAttribute(name);
  if (geometryAttribute)
    return geometryAttribute;
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

  if (std::strcmp(name, "primitiveId") == 0)
  {
    Attribute *attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }

  return nullptr;
}

void Cones::processConeAttribute(
    rpr_shape cone, const std::vector<vec4> &colors, uvec2 index, int key, int numSegments)
{
  vec4 downColor = colors[index.x];
  vec4 upColor   = colors[index.y];

  auto data   = std::vector<vec4>(numSegments, downColor);
  auto upData = std::vector<vec4>(numSegments, upColor);
  data.insert(data.end(), upData.begin(), upData.end());
  data.push_back(downColor);
  data.push_back(upColor);

  CHECK(rprShapeSetPrimvar(
      cone, key, (rpr_float *)data.data(), data.size() * 4, 4, RPR_PRIMVAR_INTERPOLATION_VERTEX))
}

void Cones::processConeAttributes(rpr_shape cone, uvec2 index)
{
  if (!m_attribute0.empty())
    processConeAttribute(cone, m_attribute0, index, 0, m_numSegments);
  if (!m_attribute1.empty())
    processConeAttribute(cone, m_attribute1, index, 1, m_numSegments);
  if (!m_attribute2.empty())
    processConeAttribute(cone, m_attribute2, index, 2, m_numSegments);
  if (!m_attribute3.empty())
    processConeAttribute(cone, m_attribute3, index, 3, m_numSegments);
  if (!m_colors.empty())
    processConeAttribute(cone, m_colors, index, 4, m_numSegments);
}

uvec2 Cones::processConeCaps(const std::string &name)
{
  if (cone_cap_types.find(name) != cone_cap_types.end())
  {
    return cone_cap_types.find(name)->second;
  }
  throw std::runtime_error("unknown cap type");
}

} // namespace anari::rpr
