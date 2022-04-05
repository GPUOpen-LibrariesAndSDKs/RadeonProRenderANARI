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

#include "SpotLight.h"

namespace anari::rpr {

SpotLight::SpotLight(rpr_context context) : AnalyticLight(context) {}

void SpotLight::commit()
{
  Light::commit();
  m_position        = getParam<vec3>("position", vec3(0));
  m_direction       = getParam<vec3>("direction", vec3(0, 0, -1));
  m_outerAngle      = getParam<float32>("openingAngle", pi<float32>());
  auto falloffAngle = getParam<float32>("falloffAngle", 0.1f);
  auto intensity    = getParam<float32>("intensity", 1.f);
  auto power        = getParam<float32>("power", 1.f);

  m_innerAngle = m_outerAngle - 2 * falloffAngle;

  if (hasParam("intensity") || !hasParam("power"))
  {
    m_power = intensity * calculateSteradianAngle(m_outerAngle);
  } else
  {
    m_power = power;
  }
}

void SpotLight::getInstances(std::vector<void *> &outInstances, mat4 transform)
{
  rpr_light light;
  CHECK(rprContextCreateSpotLight(m_context, &light))
  CHECK(rprSpotLightSetConeShape(light, m_innerAngle, m_outerAngle))
  CHECK(rprSpotLightSetRadiantPower3f(light, m_color.r * m_power, m_color.g * m_power, m_color.b * m_power))

  mat4 translation     = translate(mat4(1), m_position);
  mat4 rotation        = calculateRotation(m_direction);
  mat4 transformResult = translation * rotation;
  transformResult *= transform;
  CHECK(rprLightSetTransform(light, false, value_ptr(transformResult)))
  outInstances.push_back(light);
}

} // namespace anari::rpr