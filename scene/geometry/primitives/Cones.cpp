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
  processAttributeParameters(nullptr);
}

rpr_shape Cones::createConeShape(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius, bool upCap, bool downCap, mat4 externalTransform, int numSegments)
{
  vec3 direction = normalize(upVertex - downVertex);
  mat4 transform = translate(mat4(1), (upVertex + downVertex) * 0.5f); // middle point between vertices
  transform *= calculateRotation(direction, vec3(0,1,0));
  transform *= externalTransform;
  const float height = length(upVertex - downVertex);

  std::vector<vec3> vertices;
  std::vector<vec3> normals;
  std::vector<rpr_int> indices;
  std::vector<rpr_int> normalIndices;
  std::vector<rpr_int> faces(numSegments, 4);
  vertices.resize(numSegments * 2);
  normals.resize(numSegments * 2);
  indices.resize(numSegments * 4);
  normalIndices.resize(numSegments * 4);

  float angle = 0;
  const float sectorSize = pi<float>() * 2.f / (float)numSegments;
  for(int segmentNumber = 0; segmentNumber < numSegments; segmentNumber++)
  {
    vertices[segmentNumber] = vec3(downRadius * sin(angle), -height * 0.5f, downRadius * cos(angle));
    vertices[segmentNumber + numSegments] = vec3(upRadius * sin(angle), height * 0.5f, upRadius * cos(angle));

    normals[segmentNumber] = vertices[segmentNumber];
    normals[segmentNumber].y = downRadius * (downRadius - upRadius) / height;
    normals[segmentNumber + numSegments] = normals[segmentNumber];

    indices[segmentNumber * 4] = segmentNumber;
    indices[segmentNumber * 4 + 1] = segmentNumber + 1;
    indices[segmentNumber * 4 + 2] = segmentNumber + numSegments + 1;
    indices[segmentNumber * 4 + 3] = segmentNumber + numSegments;

    normalIndices[segmentNumber * 4] = segmentNumber;
    normalIndices[segmentNumber * 4 + 1] = segmentNumber + 1;
    normalIndices[segmentNumber * 4 + 2] = segmentNumber + numSegments + 1;
    normalIndices[segmentNumber * 4 + 3] = segmentNumber + numSegments;

    angle += sectorSize;
  }
  indices[numSegments * 4 - 2] = numSegments;
  indices[numSegments * 4 - 3] = 0;
  normalIndices[numSegments * 4 - 2] = numSegments;
  normalIndices[numSegments * 4 - 3] = 0;

  // central points for caps
  vertices.emplace_back(0, -height * 0.5f, 0);
  vertices.emplace_back(0, height * 0.5f, 0);
  normals.emplace_back(0, -1, 0);
  normals.emplace_back(0, 1, 0);

  if(downCap && downRadius > 0)
  {
    for(int i=0; i<numSegments; i++){
      indices.push_back(i);
      indices.push_back(i == numSegments - 1 ? 0 : i + 1);
      indices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      normalIndices.push_back(numSegments * 2);
      faces.push_back(3);
    }
  }

  if(upCap && upRadius > 0)
  {
    for(int i=0; i<numSegments; i++){
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
  CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertices.data(), vertices.size(), sizeof(rpr_float) * 3, (rpr_float*) normals.data(), normals.size(), sizeof(rpr_float) * 3, nullptr, 0, 0, indices.data(), sizeof(rpr_int), normalIndices.data(), sizeof(rpr_int), nullptr, 0, faces.data(), faces.size(), &shape))
  CHECK(rprShapeSetTransform(shape, false, value_ptr(transform)))

  return shape;
}

box3 Cones::calculateConeBounds(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius)
{
  vec3 direction = normalize(upVertex - downVertex);
  mat4 transform = translate(mat4(1), (upVertex + downVertex) * 0.5f); // middle point between vertices
  transform *= calculateRotation(direction, vec3(0,1,0));
  const float height = length(upVertex - downVertex);
  const float maxRadius = max(upRadius, downRadius);

  box3 coneBounds;
  coneBounds.upper = vec3(maxRadius, height / 2, maxRadius);
  coneBounds.lower = -coneBounds.upper;

  return xfmBox(transform, coneBounds);
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
  rpr_shape cone = createConeShape(upVertex, downVertex, upRadius, downRadius, upCap, downCap, externalTransform, m_numSegments);
  processConeAttributes(cone, index);
  return cone;
}

Attribute *Cones::getAttribute(const char *name)
{
  Attribute *attribute = Geometry::getAttribute(name);
  if(attribute) return attribute;
  if(std::strcmp(name, "attribute0") == 0) return createPrimVarAttribute(0, name);
  if(std::strcmp(name, "attribute1") == 0) return createPrimVarAttribute(1, name);
  if(std::strcmp(name, "attribute2") == 0) return createPrimVarAttribute(2, name);
  if(std::strcmp(name, "attribute3") == 0) return createPrimVarAttribute(3, name);
  if(std::strcmp(name, "color") == 0) return createPrimVarAttribute(4, name);
  return nullptr;
}

void Cones::processConeAttribute(rpr_shape cone, const std::vector<vec4> &colors, uvec2 index, int key, int numSegments)
{
  vec4 downColor = colors[index.x];
  vec4 upColor = colors[index.y];

  auto data = std::vector<vec4>(numSegments, downColor);
  auto upData = std::vector<vec4>(numSegments, upColor);
  data.insert(data.end(), upData.begin(), upData.end());
  data.push_back(downColor);
  data.push_back(upColor);

  CHECK(rprShapeSetPrimvar(cone, key, (rpr_float *)data.data(), data.size() * 4, 4, RPR_PRIMVAR_INTERPOLATION_VERTEX))
}

void Cones::processConeAttributes(rpr_shape cone, uvec2 index)
{
  if(!m_attribute0.empty())
    processConeAttribute(cone, m_attribute0, index, 0, m_numSegments);
  if(!m_attribute1.empty())
    processConeAttribute(cone, m_attribute1, index, 1, m_numSegments);
  if(!m_attribute2.empty())
    processConeAttribute(cone, m_attribute2, index, 2, m_numSegments);
  if(!m_attribute3.empty())
    processConeAttribute(cone, m_attribute3, index, 3, m_numSegments);
  if(!m_colors.empty())
    processConeAttribute(cone, m_colors, index, 4, m_numSegments);
}

}