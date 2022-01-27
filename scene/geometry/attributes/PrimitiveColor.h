#pragma once
#include "Attribute.h"

namespace anari::rpr{

struct PrimitiveColor : public Attribute
{
public:
   PrimitiveColor(rpr_context context, rpr_material_system matsys, rpr_uint num_primitives, const float *colors);

   ~PrimitiveColor();

private:
  rpr_material_node m_lookupNode{};
  rpr_buffer m_buffer{};
};

}
