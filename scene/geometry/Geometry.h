// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../../Array.h"
#include "../../rpr_common.h"

namespace anari {
namespace rpr {

struct Geometry : public Object
{
  friend struct Surface;

  explicit Geometry(rpr_context &context) : m_context(context) {};

  static Geometry *createInstance(rpr_context &context, const char *type);

  void addToScene(rpr_scene scene) override;


  protected:
    rpr_context m_context; // we need to store the context so we can create mesh at commit
    std::vector<rpr_shape> m_shapes;
    bool hasVertexColor = false;

    void applyMaterial(rpr_material_node material);
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
