#pragma once

#include "Geometry.h"
#include "Math/mathutils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace anari {
namespace rpr {

struct Spheres : public Geometry
{
  explicit Spheres(rpr_context & context);

  void commit() override;

  ~Spheres();

 private:

  void createBaseSphere();

  rpr_shape m_base_sphere = nullptr;

  int sectors=16;

  int rings=16;

};

} // namespace rpr
} // namespace anari