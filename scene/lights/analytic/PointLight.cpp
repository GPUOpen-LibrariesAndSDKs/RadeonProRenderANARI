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

#include "PointLight.h"

namespace anari::rpr {

PointLight::PointLight(rpr_context context) : AnalyticLight(context) {}

void PointLight::commit()
{
  Light::commit();
  m_position     = getParam<vec3>("position", vec3(0));
  m_radius       = getParam<float32>("radius", 0.f);
  auto intensity = getParam<float32>("intensity", 1.f);
  auto power     = getParam<float32>("power", 1.f);
  auto radiance  = getParam<float32>("radiance", 1.f);

  if (hasParam("intensity") || !(hasParam("power") || !hasParam("radiance")))
  {
    m_power = intensity * pi<float32>() * 4.f;
  } else if (hasParam("power") || !hasParam("radiance"))
  {
    m_power = power;
  } else
  {
    float32 area = 4 * pi<float32>() * m_radius * m_radius;
    m_power      = radiance * area * pi<float32>() * 4.f;
  }
}

void PointLight::getInstances(std::vector<void *> &outInstances, mat4 transform)
{
  rpr_light light;
  if (m_radius == 0.f)
  {
    CHECK(rprContextCreatePointLight(m_context, &light))
    CHECK(
        rprPointLightSetRadiantPower3f(light, m_color.r * m_power, m_color.g * m_power, m_color.b * m_power))

  } else
  {
    CHECK(rprContextCreateSphereLight(m_context, &light))
    CHECK(
        rprSphereLightSetRadiantPower3f(light, m_color.r * m_power, m_color.g * m_power, m_color.b * m_power))
    CHECK(rprSphereLightSetRadius(light, m_radius))
  }
  mat4 transformResult = translate(mat4(1), m_position) * transform;
  CHECK(rprLightSetTransform(light, false, value_ptr(transformResult)))
  outInstances.push_back(light);
}

} // namespace anari::rpr