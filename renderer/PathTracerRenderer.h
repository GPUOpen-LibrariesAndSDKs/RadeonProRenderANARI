// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari {
namespace rpr {

struct PathTracerRenderer : public Renderer{

  PathTracerRenderer(rpr_context context);

  void commit() override;

};

}
}
