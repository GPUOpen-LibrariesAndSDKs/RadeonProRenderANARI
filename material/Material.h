// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
#include "../rpr_common.h"
#include "../scene/geometry/Geometry.h"

namespace anari {
namespace rpr {

struct Material : public Object
{
  Material() = default;

  static Material *createInstance(const char *type, rpr_material_system matsys);

  virtual inline rpr_material_node generateMaterial(Geometry *geometry){return nullptr;};

 protected:
  rpr_material_system m_matsys{};
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Material *, ANARI_MATERIAL);

} // namespace anari
