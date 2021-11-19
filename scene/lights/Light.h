// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../rpr_common.h"
#include "../../Object.h"

namespace anari {
namespace rpr {

struct Light : public Object
{
  friend struct Instance;

  explicit Light(rpr_context &context) : m_context(context) {};

  static Light *createInstance(rpr_context &context, rpr_material_system matsys, const char *type);

  inline box3 bounds() const override{
    return {m_lower_bound, m_upper_bound};
  }

  inline rpr_light get_light() {
    return m_light;
  }

  inline void addToScene(rpr_scene) override{};

  ~Light() override;

  protected:
    rpr_context m_context; // we need to store the context so we can create at commit
    void * m_light = nullptr;  // this could be rpr_light or rpr_shape
    vec3 m_lower_bound{};
    vec3 m_upper_bound{};

};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Light *, ANARI_LIGHT)

} // namespace anari
