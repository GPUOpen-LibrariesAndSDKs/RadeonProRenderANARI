#include "Orthographic.h"

namespace anari::rpr{

Orthographic::Orthographic(rpr_camera camera) : Camera(camera){
  CHECK(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_ORTHOGRAPHIC))
}

void Orthographic::commit()
{
  Camera::commit();

  auto sensorHeight = getParam<float>("sensorHeight", 24.f); // additional parameter
  auto orthoHeight = getParam<float>("orthoHeight", 1.f); // additional parameter
  auto aspect = getParam<float>("aspect", 1.f);

  float sensorWidth = sensorHeight * aspect;
  CHECK(rprCameraSetSensorSize(m_camera, sensorWidth, sensorHeight))

  float orthoWidth = orthoHeight * aspect;
  CHECK(rprCameraSetOrthoHeight(m_camera, orthoHeight))
  CHECK(rprCameraSetOrthoWidth(m_camera, orthoWidth))
}

}
