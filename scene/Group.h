#pragma once

#include "../rpr_common.h"
#include "SceneObject.h"

namespace anari {
namespace rpr {

struct Surface;
struct Light;

struct Group : public SceneObject
{
  Group();

  void commit() override;

  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform);

  void getBaseShapes(std::vector<rpr_shape> &outShapes);

  void addLightsToScene(rpr_scene scene);

private:
  std::vector<Surface *> m_surfaces;
  std::vector<Light *>m_lights;
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::Group *, ANARI_GROUP)
} // namespace anari
