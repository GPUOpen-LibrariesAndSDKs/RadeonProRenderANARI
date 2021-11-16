// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace anari {
namespace rpr {

Perspective::Perspective(rpr_camera &camera) : Camera(camera) {
  CHECK(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_PERSPECTIVE));
}

void Perspective::commit()
{
  Camera::commit();

  float sensor_height = 24; //default rpr sensor height

  float fovy = getParam<float>("fovy", 60.f);
  float aspect = getParam<float>("aspect", 1.f);

  float sensor_width = sensor_height * aspect;
  CHECK(rprCameraSetSensorSize(m_camera, sensor_width, sensor_height))

  float focal_length = sensor_height/(2 * tan(radians(fovy * 0.5f)));
  CHECK(rprCameraSetFocalLength(m_camera, focal_length));

}


} // namespace rpr
} // namespace anari
