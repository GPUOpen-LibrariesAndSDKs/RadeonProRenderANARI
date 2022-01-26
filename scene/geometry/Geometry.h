// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../SceneObject.h"
#include "../../rpr_common.h"

namespace anari {
namespace rpr {

struct Geometry : public SceneObject
{
  Geometry(rpr_context context, rpr_material_system  m_matsys);

  static Geometry *createInstance(rpr_context context, rpr_material_system  materialSystem, const char *type);

  virtual void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) {};

  rpr_shape getBaseShape();

  bool hasVertexColor = false;

  ~Geometry() override;

protected:
    rpr_context m_context;
    rpr_material_system m_matsys; // material system is using to create lookup nodes for attributes
    rpr_shape m_base_shape{};
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
