#pragma once
#include "Primitives.h"

namespace anari::rpr{

struct Cones : public Primitives
{
public:
  Cones(rpr_context context, rpr_material_system materialSystem);
  void commit() override;
  Attribute *getAttribute(const char* name) override;

protected:
  // can be used to create cylinder shape if up and down radius are same
  rpr_shape createConeShape(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius, bool upCap, bool downCap, mat4 externalTransform, int numSegments);
  static box3 calculateConeBounds(vec3 upVertex, vec3 downVertex, float upRadius, float downRadius);
  void processConeAttributes(rpr_shape cone, uvec2 index);
  static void processConeAttribute(rpr_shape cone, const std::vector<vec4> &colors, uvec2 index, int key, int numSegments);

private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  Array1D *m_indices{};
  Array1D *m_radius{};
  Array1D *m_vertices{};
  Array1D *m_caps{};

  int m_numSegments = 32;
};

}