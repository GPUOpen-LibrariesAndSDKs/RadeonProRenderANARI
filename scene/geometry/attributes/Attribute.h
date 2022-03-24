#pragma once
#include "../../rpr_common.h"
#include "../../Array.h"

namespace anari::rpr{

struct Attribute{

public:

  explicit Attribute(rpr_material_node materialNode);
  Attribute() = default;
  static Attribute* fromType(rpr_material_system matsys, rpr_material_node_lookup_value type);
  rpr_material_node getMaterial();
  virtual ~Attribute();

  static vec4 processColor(Array1D *array, size_t index);

protected:
  rpr_material_node m_material{};
};

}
