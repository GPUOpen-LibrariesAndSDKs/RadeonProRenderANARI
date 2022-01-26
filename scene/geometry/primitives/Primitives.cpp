#include "Primitives.h"

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
}
