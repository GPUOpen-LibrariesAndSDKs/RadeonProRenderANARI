#pragma once

#include "../Geometry.h"

namespace anari::rpr{

// sphere, cone, cylinder geometries
struct Primitives : public Geometry
{
public:
  Primitives(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;

protected:
  virtual rpr_shape getPrimitive(int primitive_number, mat4 externalTransform){return nullptr;};

  // can be used to create cylinder shape if up and down radius are same
  rpr_shape createConeShape(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius, bool upCap, bool downCap, mat4 externalTransform, int numSegments);
  static box3 calculateConeBounds(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius);


  size_t m_num_primitives{};
};

}
