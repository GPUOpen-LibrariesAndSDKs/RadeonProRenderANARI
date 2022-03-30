#pragma once
#include "Cones.h"

namespace anari::rpr{

struct Cylinders : public Cones
{
public:
  Cylinders(rpr_context context, rpr_material_system materialSystem);
  void commit() override;

private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  float32 m_globalRadius{};
};

}
