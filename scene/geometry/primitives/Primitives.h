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
  void processAttributeParameters(const std::vector<int> &index);

  size_t m_num_primitives{};

  std::vector<vec4> m_colors;
  std::vector<vec4> m_attribute0;
  std::vector<vec4> m_attribute1;
  std::vector<vec4> m_attribute2;
  std::vector<vec4> m_attribute3;

private:
  static void processAttributeArray(Array1D *data, const std::vector<int>& index, std::vector<vec4> &outVector);
  Attribute *createPerShapeAttribute(const std::vector<vec4> &data, const char *name);
};

}
