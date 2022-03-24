#include "Primitives.h"

namespace anari::rpr{

Primitives::Primitives(rpr_context context, rpr_material_system materialSystem) : Geometry(context, materialSystem){}

void Primitives::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  for(int primitive_number = 0; primitive_number < m_num_primitives; primitive_number++)
  {
    rpr_shape instance = getPrimitive(primitive_number, transform);
    CHECK(rprShapeSetObjectID(instance, primitive_number))
    out_shapes.push_back(instance);
  }
}

rpr_shape Primitives::createConeShape(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius, bool upCap, bool downCap, mat4 externalTransform, int numSegments)
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

box3 Primitives::calculateConeBounds(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius)
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

}
