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

}
