// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari {
namespace rpr {

struct AnalyticLight : public Light
{
  AnalyticLight(rpr_context & context, const char *type);

  void commit() override;

  protected:
    const char * m_type;

    void addToScene(rpr_scene scene) override;
};

} // namespace rpr
} // namespace anari
