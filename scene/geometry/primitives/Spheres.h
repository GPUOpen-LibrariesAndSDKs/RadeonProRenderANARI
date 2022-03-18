#pragma once

#include "Primitives.h"

namespace anari::rpr {

struct Spheres : public Primitives
{
  Spheres(rpr_context context, rpr_material_system materialSystem);
  void commit() override;

 private:

  void createBaseShape();
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  int m_sectors = 32;
  int m_rings = 32;

  std::vector<float> m_radius;
  std::vector<vec3> m_positions;
  std::vector<int> m_indices;

};

} // namespace anari
