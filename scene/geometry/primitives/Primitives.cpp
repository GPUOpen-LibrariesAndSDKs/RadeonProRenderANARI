#include "Primitives.h"
#include "../attributes/PerShapeAttribute.h"

namespace anari::rpr{

Primitives::Primitives(rpr_context context, rpr_material_system materialSystem) : Geometry(context, materialSystem){}

void Primitives::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  for(int primitive_number = 0; primitive_number < m_num_primitives; primitive_number++)
  {
    //transform
    mat4 primitive_transform =
        generatePrimitiveTransform(primitive_number) * transform;

    rpr_shape instance;
    CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
    CHECK(rprShapeSetTransform(instance, false, value_ptr(primitive_transform)))
    CHECK(rprShapeSetObjectID(instance, primitive_number))

    out_shapes.push_back(instance);
  }
}

Attribute *Primitives::getAttribute(const char *name)
{
  if(std::strcmp(name, "color") == 0)
  {
    if(m_colors.empty()) return nullptr;

    Attribute* attribute = new PerShapeAttribute(m_context, m_matsys, m_num_primitives, (float*) m_colors.data());
    m_attributes.push_back(attribute);
    return attribute;
  }
  if(std::strcmp(name, "primitiveId") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID);
    m_attributes.push_back(attribute);
    return attribute;
  }
  return Geometry::getAttribute(name);
}

bool Primitives::hasAttribute(const char *name)
{
  if(std::strcmp(name, "color") == 0 || std::strcmp(name, "primitiveId") == 0)
  {
    return true;
  }
  return Geometry::hasAttribute(name);
}

}
