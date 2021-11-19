// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace anari::rpr {

struct AnalyticLight : public Light
{
  AnalyticLight(rpr_context & context, const char *type);

  void commit() override;

  protected:
    const char * m_type;

    void addToScene(rpr_scene scene) override;
};

} // namespace anari
