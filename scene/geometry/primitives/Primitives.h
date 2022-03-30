#pragma once

#include "../Geometry.h"

namespace anari::rpr{

// sphere, cone, cylinder geometries
struct Primitives : public Geometry
{
public:
  Primitives(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;
  bool hasAttribute(const char *name) override;

protected:
  virtual rpr_shape getPrimitive(int primitive_number, mat4 externalTransform){return nullptr;};
  static void processAttributeArray(Array1D *data, Array1D *index, std::vector<vec4> &outVector) ;
  void processAttributeParameters(Array1D *index);

  size_t m_num_primitives{};
  std::vector<vec4> m_colors;
  std::vector<vec4> m_attribute0;
  std::vector<vec4> m_attribute1;
  std::vector<vec4> m_attribute2;
  std::vector<vec4> m_attribute3;
};

}
