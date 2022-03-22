#include "Primitives.h"
#include "../attributes/PerShapeAttribute.h"

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

Attribute *Primitives::getAttribute(const char *name)
{
  Attribute *geometryAttribute = Geometry::getAttribute(name);
  if(geometryAttribute) return geometryAttribute;

  if(std::strcmp(name, "color") == 0) return createPerShapeAttribute(m_colors, name);
  if(std::strcmp(name, "attribute0") == 0) return createPerShapeAttribute(m_attribute0, name);
  if(std::strcmp(name, "attribute1") == 0) return createPerShapeAttribute(m_attribute1, name);
  if(std::strcmp(name, "attribute2") == 0) return createPerShapeAttribute(m_attribute2, name);
  if(std::strcmp(name, "attribute3") == 0) return createPerShapeAttribute(m_attribute3, name);

  if(std::strcmp(name, "primitiveId") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }

  return nullptr;
}

Attribute *Primitives::createPerShapeAttribute(const std::vector<vec4> &data, const char *name)
{
  if(data.empty())
  {
    return nullptr;
  }
  Attribute *attribute = new PerShapeAttribute(m_context, m_matsys, data.size(), (float*) data.data());
  m_attribute_map.emplace(name, attribute);
  return attribute;
}

void Primitives::processAttributeParameters(const std::vector<int> &index)
{
  m_colors.clear();
  m_attribute0.clear();
  m_attribute1.clear();
  m_attribute2.clear();
  m_attribute3.clear();
  processAttributeArray(getParamObject<Array1D>("vertex.color"), index, m_colors);
  processAttributeArray(getParamObject<Array1D>("vertex.attribute0"), index, m_attribute0);
  processAttributeArray(getParamObject<Array1D>("vertex.attribute1"), index, m_attribute1);
  processAttributeArray(getParamObject<Array1D>("vertex.attribute2"), index, m_attribute2);
  processAttributeArray(getParamObject<Array1D>("vertex.attribute3"), index, m_attribute3);
}

bool Primitives::hasAttribute(const char *name)
{
  if((std::strcmp(name, "color") == 0 && !m_colors.empty()) ||
      (std::strcmp(name, "attribute0") == 0 && !m_attribute0.empty()) ||
      (std::strcmp(name, "attribute1") == 0 && !m_attribute1.empty()) ||
      (std::strcmp(name, "attribute2") == 0 && !m_attribute2.empty()) ||
      (std::strcmp(name, "attribute3") == 0 && !m_attribute3.empty()) ||
      std::strcmp(name, "primitiveId") == 0)
  {
    return true;
  }
  return Geometry::hasAttribute(name);
}

void Primitives::processAttributeArray(Array1D *data, const std::vector<int>& index, std::vector<vec4> &outVector)
{
  if(!data) return;

  for(int primitive_number : index){
    switch (data->elementType()) {
      case ANARI_FLOAT32:{
        outVector.emplace_back(data->dataAs<float>()[primitive_number], 0, 0, 1);
        break;
      }
      case ANARI_FLOAT32_VEC2:{
        outVector.emplace_back(data->dataAs<vec2>()[primitive_number], 0, 1);
        break;
      }
      case ANARI_FLOAT32_VEC3:{
        outVector.emplace_back(data->dataAs<vec3>()[primitive_number], 1);
        break;
      }
      case ANARI_FLOAT32_VEC4:{
        outVector.emplace_back(data->dataAs<vec4>()[primitive_number]);
        break;
      }
      }
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
  std::vector<rpr_int> indices;
  std::vector<rpr_int> faces(numSegments, 4);
  vertices.resize(numSegments * 2);
  indices.resize(numSegments * 4);

  float angle = 0;
  const float sectorSize = pi<float>() * 2.f / (float)numSegments;
  for(int segmentNumber = 0; segmentNumber < numSegments; segmentNumber++)
  {
    vertices[segmentNumber] = vec3(downRadius * sin(angle), -height * 0.5f, downRadius * cos(angle));
    vertices[segmentNumber + numSegments] = vec3(upRadius * sin(angle), height * 0.5f, upRadius * cos(angle));
    indices[segmentNumber * 4] = segmentNumber;
    indices[segmentNumber * 4 + 1] = segmentNumber + 1;
    indices[segmentNumber * 4 + 2] = segmentNumber + numSegments + 1;
    indices[segmentNumber * 4 + 3] = segmentNumber + numSegments;

    angle += sectorSize;
  }
  indices[numSegments * 4 - 2] = numSegments;
  indices[numSegments * 4 - 3] = 0;

  rpr_shape shape;
  CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertices.data(), vertices.size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, indices.data(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), faces.size(), &shape))
  CHECK(rprShapeSetTransform(shape, false, value_ptr(transform)))

  return shape;

}

}
