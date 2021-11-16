// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "AnalyticLight.h"

namespace anari {
namespace rpr {

struct EnvironmentLight : public AnalyticLight
{
  EnvironmentLight(rpr_context &context, const char *type);

  void commit() override;

  ~EnvironmentLight();

 private:
  rpr_image m_color_texture = nullptr;

};

} // namespace rpr
} // namespace anari