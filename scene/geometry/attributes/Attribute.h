#pragma once
#include "../../rpr_common.h"

namespace anari::rpr{

struct Attribute{

public:

  explicit Attribute(rpr_material_node materialNode);

  Attribute() = default;

  static Attribute* fromType(rpr_material_system matsys, rpr_material_node_lookup_value type);

  rpr_material_node getMaterial();

  ~Attribute();

protected:
  rpr_material_node m_material{};
};

}
