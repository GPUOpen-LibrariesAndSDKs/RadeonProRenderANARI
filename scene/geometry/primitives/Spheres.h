#pragma once

#include "Primitives.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace anari::rpr {

struct Spheres : public Primitives
{
  Spheres(rpr_context context, rpr_material_system materialSystem);
  void commit() override;

 private:

  void createBaseShape();
  mat4 generatePrimitiveTransform(int primitive_number) override;

  int sectors=32;
  int rings=32;

  std::vector<float> m_radius;
  std::vector<vec3> m_positions;
  std::vector<int> m_indices;

};

} // namespace anari
