// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../rpr_common.h"
#include "../Object.h"
#include "../RPRDevice.h"

namespace anari {
namespace rpr {

struct Renderer : public Object
{
  friend struct Frame;

  Renderer(rpr_context context);

  void commit();

  static Renderer *createInstance(rpr_context context, const char *type);

  ~Renderer();

protected:
  rpr_context m_context;
  rpr_image m_background_image = nullptr;
  int m_pixel_samples = 1;

  void addToScene(rpr_scene scene) override;
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Renderer *, ANARI_RENDERER);

} // namespace anari