#pragma once

#include "Primitives.h"

namespace anari::rpr {

struct Spheres : public Primitives
{
  Spheres(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
  Attribute *getAttribute(const char* name) override;
  bool hasAttribute(const char *name) override;

 private:

  void createBaseShape();
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;
  void processAttributeArray(Array1D *data, Array1D *index, std::vector<vec4> &outVector) const;
  void processAttributeParameters(Array1D *index);

  int m_sectors = 32;
  int m_rings = 32;

  Array1D *m_vertices{};
  Array1D *m_radius{};
  Array1D *m_indices{};
  float32 m_globalRadius{};

  std::vector<vec4> m_colors;
  std::vector<vec4> m_attribute0;
  std::vector<vec4> m_attribute1;
  std::vector<vec4> m_attribute2;
  std::vector<vec4> m_attribute3;

};

} // namespace anari
