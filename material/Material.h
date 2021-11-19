// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
#include "../rpr_common.h"
#include "../scene/geometry/Surface.h"

namespace anari {
namespace rpr {

struct Material : public Object
{
  friend struct Surface;

  Material() = default;

  static Material *createInstance(const char *type, rpr_material_system matsys);

 protected:
  rpr_material_system m_matsys;
  virtual inline rpr_material_node generateMaterial(rpr_material_node vertex_color = nullptr){return nullptr;};
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Material *, ANARI_MATERIAL);

} // namespace anari
