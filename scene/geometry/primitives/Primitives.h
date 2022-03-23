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
  static void processAttributeArray(Array1D *data, const std::vector<int>& index, std::vector<vec4> &outVector);

protected:
  virtual rpr_shape getPrimitive(int primitive_number, mat4 externalTransform){return nullptr;};
  void processAttributeParameters(const std::vector<int> &index);

  // can be used to create cylinder shape if up and down radius are same
  rpr_shape createConeShape(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius, bool upCap, bool downCap, mat4 externalTransform, int numSegments);
  box3 calculateConeBorders(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius);


  size_t m_num_primitives{};

  std::vector<vec4> m_colors;
  std::vector<vec4> m_attribute0;
  std::vector<vec4> m_attribute1;
  std::vector<vec4> m_attribute2;
  std::vector<vec4> m_attribute3;

private:
  Attribute *createPerShapeAttribute(const std::vector<vec4> &data, const char *name);
};

}
