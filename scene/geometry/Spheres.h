#pragma once

#include "Geometry.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace anari::rpr {

struct Spheres : public Geometry
{
  explicit Spheres(rpr_context & context);

  void commit() override;

  ~Spheres() override;

 private:

  void createBaseSphere();

  rpr_shape m_base_sphere = nullptr;

  int sectors=16;

  int rings=16;

};

} // namespace anari