#pragma once

#include "../Geometry.h"

namespace anari::rpr{

// sphere, cone, cylinder geometries
struct Primitives : public Geometry
{
public:
  Primitives(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;
  Attribute *getAttribute(const char* name) override;
  bool hasAttribute(const char *name) override;

protected:

  virtual mat4 generatePrimitiveTransform(int primitive_number){ return {}; };

  size_t m_num_primitives{};

  std::vector<vec4> m_colors;
};

}
