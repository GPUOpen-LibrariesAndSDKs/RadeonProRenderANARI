#pragma once
#include "Attribute.h"

namespace anari::rpr{

struct PerShapeAttribute : public Attribute
{
public:
  PerShapeAttribute(rpr_context context, rpr_material_system matsys, rpr_uint num_primitives, const float *data);

   ~PerShapeAttribute() override;

private:
  rpr_material_node m_lookupNode{};
  rpr_buffer m_buffer{};
};

}
