// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
#include "../Data.h"
#include "World.h"
#include "lights/Light.h"
#include "../rpr_common.h"

namespace anari {
namespace rpr {

struct Instance : public Object
{
  friend struct World;

  Instance() = default;

  void commit() override;

  inline box3 bounds() const override{
    return box3(m_lower_bound, m_upper_bound);
  }

 private:
  std::vector<rpr_shape> m_shapes;
  std::vector<Light *> m_lights;
  vec3 m_upper_bound;
  vec3 m_lower_bound;

  void addToScene(rpr_scene scene) override;
};

// Inlined defintions /////////////////////////////////////////////////////////


} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Instance *, ANARI_INSTANCE);

} // namespace anari
