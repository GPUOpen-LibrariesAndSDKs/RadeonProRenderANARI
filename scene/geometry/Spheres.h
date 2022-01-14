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

  void getInstances(std::vector<rpr_shape> &out_shapes) override;

  ~Spheres() override;

 private:

  void createBaseSphere();

  int sectors=16;
  int rings=16;

  std::vector<std::pair<vec3, float>> m_sphere_data;

};

} // namespace anari