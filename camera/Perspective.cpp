// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace anari::rpr {

Perspective::Perspective(rpr_camera &camera) : Camera(camera) {
  CHECK(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_PERSPECTIVE))
}

void Perspective::commit()
{
  Camera::commit();

  auto sensorHeight = getParam<float>("sensorHeight", 24.f); // additional parameter
  auto fovy = getParam<float>("fovy", radians(60.f));
  auto focusDistance = getParam<float>("focusDistance", 1.f);
  auto apertureRadius = getParam<float>("apertureRadius", 0.f);
  auto aspect = getParam<float>("aspect", 1.f);

  if(apertureRadius > 0){
    float fstop = focusDistance / (apertureRadius * 2);
    CHECK(rprCameraSetFStop(m_camera, fstop))
    CHECK(rprCameraSetFocusDistance(m_camera, focusDistance))
  }

  float sensorWidth = sensorHeight * aspect;
  CHECK(rprCameraSetSensorSize(m_camera, sensorWidth, sensorHeight))

  float focal_length = sensorHeight/(2 * tan(fovy * 0.5f));
  CHECK(rprCameraSetFocalLength(m_camera, focal_length))

}

} // namespace anari::rpr
