// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari::rpr {

struct AreaLight : public Light
{
  AreaLight(rpr_context & context, rpr_material_system & matsys);

  void commit() override;

  void addToScene(rpr_scene scene) override;

  ~AreaLight() override;

 private:
  rpr_material_node m_material = nullptr;
  rpr_material_system m_matsys; //we need to store matsys to use it in commit
};

} // namespace anari
