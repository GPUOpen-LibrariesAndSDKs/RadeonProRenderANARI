// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../Data.h"
#include "../../rpr_common.h"
#include "GeometricModel.h"

namespace anari {
namespace rpr {

struct Geometry : public Object
{
  friend struct GeometricModel;

  Geometry(rpr_context &context) : m_context(context) {};

  static Geometry *createInstance(rpr_context &context, const char *type);

  inline box3 bounds() const override{
    return box3(m_lower_bound, m_upper_bound);
  }

  protected:
    rpr_context m_context; // we need to store the context so we can create mesh at commit
    std::vector<rpr_shape> m_shapes;
    vec3 m_lower_bound;
    vec3 m_upper_bound;
    bool hasVertexColor = false;
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY);

} // namespace anari
