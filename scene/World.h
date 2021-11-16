// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
#include "../rpr_common.h"
#include "../RPRDevice.h"

namespace anari {
namespace rpr {

struct Instance;

struct World : public Object
{
  friend struct Frame;

  World() = default;

  void commit() override;

  inline box3 bounds() const override{
    return box3(m_lower_bound, m_upper_bound);
  }

 private:
  std::vector<Instance*> m_instances;
  vec3 m_upper_bound;
  vec3 m_lower_bound;

  void addToScene(rpr_scene scene) override;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::World *, ANARI_WORLD);

} // namespace anari
