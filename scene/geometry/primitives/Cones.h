#pragma once
#include "Primitives.h"

namespace anari::rpr{

struct Cones : public Primitives
{
public:
  Cones(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
  inline bool hasAttribute(const char *name) override {return false;};

private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  Array1D *m_indices{};
  Array1D *m_radius{};
  Array1D *m_vertices{};
  Array1D *m_caps{};
};

}