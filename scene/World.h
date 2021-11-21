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

  inline box3 bounds() const override{
    return {m_lower_bound, m_upper_bound};
  }

 private:
  std::vector<Surface*> m_surfaces;
  std::vector<Light*> m_lights;
  vec3 m_upper_bound{};
  vec3 m_lower_bound{};

  void addToScene(rpr_scene scene) override;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::World *, ANARI_WORLD);

} // namespace anari
