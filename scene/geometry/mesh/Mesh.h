#pragma once

#include "../Geometry.h"

namespace anari::rpr {

// triangles and quad geometries
struct Mesh : public Geometry
{
public:
  Mesh(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;

protected:
  void calculateBounds(Array1D *vertex);
  void applyColor(Array1D *color);
};
}