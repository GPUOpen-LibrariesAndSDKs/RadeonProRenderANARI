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

  Array1D *m_indices{};
  Array1D *m_radius{};
  Array1D *m_vertices{};
  Array1D *m_caps{};
  float32 m_globalRadius{};

  int m_numSegments = 32;
};

}
