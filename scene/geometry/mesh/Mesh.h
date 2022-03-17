#pragma once

#include "../Geometry.h"

namespace anari::rpr {

// triangles and quad geometries
struct Mesh : public Geometry
{
public:
  Mesh(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;
  Attribute *getAttribute(const char *name) override;
  bool hasAttribute(const char *name) override;

protected:
  template <typename vecType, int polySize>
  void meshCommit(vecType defaultVector);

private:
  void calculateBounds(Array1D *vertex_data);
  void processAttributes(Array1D *vertex_data);
  void processAttributeArray(Array1D *data, int key);
  Attribute *createPrimVarAttribute(int key, const char *name);
};
}
