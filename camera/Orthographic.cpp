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

#include "Orthographic.h"

namespace anari::rpr {

Orthographic::Orthographic(rpr_camera camera) : Camera(camera)
{
  CHECK(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_ORTHOGRAPHIC))
}

void Orthographic::commit()
{
  Camera::commit();

  auto sensorHeight = getParam<float>("sensorHeight", 24.f); // additional parameter
  auto orthoHeight  = getParam<float>("orthoHeight", 1.f); // additional parameter
  auto aspect       = getParam<float>("aspect", 1.f);

  float sensorWidth = sensorHeight * aspect;
  CHECK(rprCameraSetSensorSize(m_camera, sensorWidth, sensorHeight))

  float orthoWidth = orthoHeight * aspect;
  CHECK(rprCameraSetOrthoHeight(m_camera, orthoHeight))
  CHECK(rprCameraSetOrthoWidth(m_camera, orthoWidth))
}

} // namespace anari::rpr
