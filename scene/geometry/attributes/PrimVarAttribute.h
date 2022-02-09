#pragma once
#include "Attribute.h"

namespace anari::rpr{

struct PrimVarAttribute : public Attribute
{
  PrimVarAttribute(rpr_material_system matsys, int key);
};

}