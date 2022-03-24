#pragma once

#include "Primitives.h"

namespace anari::rpr {

struct Spheres : public Primitives
{
  Spheres(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
  Attribute *getAttribute(const char* name) override;

 private:

  void createBaseShape();
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  int m_sectors = 32;
  int m_rings = 32;

  Array1D *m_vertices{};
  Array1D *m_radius{};
  Array1D *m_indices{};
  float32 m_globalRadius{};

};

} // namespace anari
