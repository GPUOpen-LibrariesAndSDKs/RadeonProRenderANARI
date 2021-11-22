#pragma once

#include "../Array.h"
#include "../rpr_common.h"

namespace anari {
namespace rpr {

struct Surface;
struct Light;

struct World : public Object
{
  friend struct Frame;

  World() = default;

  void commit() override;

 private:
  std::vector<Surface*> m_surfaces;
  std::vector<Light*> m_lights;

  void addToScene(rpr_scene scene) override;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::World *, ANARI_WORLD);

} // namespace anari
