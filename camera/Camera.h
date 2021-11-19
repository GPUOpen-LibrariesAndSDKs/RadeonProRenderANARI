// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
#include "../rpr_common.h"

namespace anari {
namespace rpr {

struct Camera : public Object
{
  friend struct Frame;

  explicit Camera(rpr_camera &camera);

  void commit() override;

  static Camera *createInstance(rpr_context &context, const char *type);

  ~Camera() override;

  protected:
  rpr_camera m_camera;

  void addToScene(rpr_scene scene) override;

};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Camera *, ANARI_CAMERA)

} // namespace anari
