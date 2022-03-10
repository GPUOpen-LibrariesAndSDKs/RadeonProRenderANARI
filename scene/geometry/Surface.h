// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../rpr_common.h"
#include "../SceneObject.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

struct Surface : public SceneObject
{
  explicit Surface(rpr_material_system materialSystem);

  void commit() override;

  void addToScene(rpr_scene scene) override;

  void addToScene(rpr_scene scene, mat4 transform);

  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform);

  rpr_shape getBaseShape();

  ~Surface() override;

 private:
  IntrusivePtr<Geometry> m_geometry;

  std::vector<rpr_shape> m_instances;

  rpr_material_system m_matsys;
  rpr_material_node m_material_instance{};

  void clearInstances();
};


} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Surface *, ANARI_SURFACE);

} // namespace anari
