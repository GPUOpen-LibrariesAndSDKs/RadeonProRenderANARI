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

void Primitives::processAttributeArray(Array1D *data, Array1D *index, std::vector<vec4> &outVector)
{
  if(!data) return;

  size_t numVertex = index ? index->size() : data->size();
  for(int indexNumber=0; indexNumber < numVertex; indexNumber++){
    uint32 primitiveIndex = index ? index->dataAs<uint32>()[indexNumber] : indexNumber;
    outVector.emplace_back(Attribute::processColor(data, primitiveIndex));
  }
}

void Primitives::processAttributeParameters(Array1D *index)
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
}
}
