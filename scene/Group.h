#pragma once

#include "../rpr_common.h"
#include "SceneObject.h"

namespace anari {
namespace rpr {

struct Surface;
struct Light;
struct Volume;

struct Group : public SceneObject
{
public:
  Group();
  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform);

private:
  std::vector<Surface *> m_surfaces;
  std::vector<Volume *> m_volumes;
  std::vector<Light *>m_lights;
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::Group *, ANARI_GROUP)
} // namespace anari
