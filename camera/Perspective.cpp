/**********************************************************************
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

#include "Perspective.h"

namespace anari::rpr {

Perspective::Perspective(rpr_camera &camera) : Camera(camera)
{
  CHECK(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_PERSPECTIVE))
}

void Perspective::commit()
{
  Camera::commit();

  auto sensorHeight   = getParam<float>("sensorHeight", 24.f); // additional parameter
  auto fovy           = getParam<float>("fovy", radians(60.f));
  auto focusDistance  = getParam<float>("focusDistance", 1.f);
  auto apertureRadius = getParam<float>("apertureRadius", 0.f);
  auto aspect         = getParam<float>("aspect", 1.f);

  if (apertureRadius > 0)
  {
    float fstop = focusDistance / (apertureRadius * 2);
    CHECK(rprCameraSetFStop(m_camera, fstop))
    CHECK(rprCameraSetFocusDistance(m_camera, focusDistance))
  }

  float sensorWidth = sensorHeight * aspect;
  CHECK(rprCameraSetSensorSize(m_camera, sensorWidth, sensorHeight))

  float focal_length = sensorHeight / (2 * tan(fovy * 0.5f));
  CHECK(rprCameraSetFocalLength(m_camera, focal_length))
}

} // namespace anari::rpr
