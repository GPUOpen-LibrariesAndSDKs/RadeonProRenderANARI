// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../SceneObject.h"
#include "../../rpr_common.h"

namespace anari {
namespace rpr {

struct Geometry : public SceneObject
{
  explicit Geometry(rpr_context &context);

  static Geometry *createInstance(rpr_context &context, const char *type);

  virtual void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) {};

  rpr_shape getBaseShape();

  bool hasVertexColor = false;

protected:
    rpr_context m_context; // we need to store the context, so we can create mesh at commit
    rpr_shape m_base_shape{};
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
