#pragma once

#include "SceneObject.h"
#include "../rpr_common.h"

namespace anari {
namespace rpr {

struct Surface;
struct Light;
struct Instance;

struct World : public SceneObject
{
  friend struct Frame;

  World();

  void commit() override;

 private:
  std::vector<Surface*> m_surfaces;
  std::vector<Light*> m_lights;
  std::vector<Instance*> m_instances;

  void addToScene(rpr_scene scene) override;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::World *, ANARI_WORLD);

} // namespace anari
