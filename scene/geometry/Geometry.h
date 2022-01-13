// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../SceneObject.h"
#include "../../rpr_common.h"

//std
#include <set>

namespace anari {
namespace rpr {

struct Geometry : public SceneObject
{
  friend struct Surface;

  explicit Geometry(rpr_context &context);

  static Geometry *createInstance(rpr_context &context, const char *type);

  virtual void getInstances(std::set<rpr_shape> &out_shapes) {};

  rpr_shape getBaseShape();

  protected:
    rpr_context m_context; // we need to store the context, so we can create mesh at commit
    rpr_shape m_base_shape{};
    bool hasVertexColor = false;
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
