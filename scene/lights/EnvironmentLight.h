// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "AnalyticLight.h"

namespace anari::rpr {

struct EnvironmentLight : public AnalyticLight
{
  EnvironmentLight(rpr_context &context, const char *type);

  void commit() override;

  ~EnvironmentLight() override;

 private:
  rpr_image m_color_texture = nullptr;

};

} // namespace anari