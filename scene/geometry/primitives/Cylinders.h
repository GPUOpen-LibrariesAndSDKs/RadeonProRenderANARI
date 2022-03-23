#pragma once
#include "Primitives.h"

namespace anari::rpr{

struct Cylinders : public Primitives
{
public:
  Cylinders(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
  inline bool hasAttribute(const char *name) override {return false;};

private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  Array1D *m_indices{};
  Array1D *m_radius{};
  Array1D *m_vertices{};
  Array1D *m_caps{};
  float32 m_globalRadius{};
};

}
