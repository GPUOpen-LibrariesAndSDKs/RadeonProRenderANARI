#include "DirectionalLight.h"

namespace anari::rpr
{

DirectionalLight::DirectionalLight(rpr_context context) : AnalyticLight(context){}

void DirectionalLight::commit()
{
  Light::commit();
  m_direction = getParam<vec3>("direction", vec3(0, 0, -1));
  auto irradiance = getParam<float32>("irradiance", 1.f);
  m_angularDiameter = getParam<float32>("angularDiameter", 0.f);
  auto radiance = getParam<float32>("radiance", 1.f);

  if(hasParam("irradiance") || !hasParam("radiance")){
    m_power = irradiance;
  }
  else
  {
    m_power = radiance * m_angularDiameter;
  }
}

void DirectionalLight::getInstances(std::vector<void *> &outInstances, mat4 transform)
{
  rpr_light light;
  CHECK(rprContextCreateDirectionalLight(m_context, &light))
  CHECK(rprDirectionalLightSetRadiantPower3f(light, m_color.r * m_power, m_color.g * m_power, m_color.b * m_power))
  CHECK(rprDirectionalLightSetShadowSoftnessAngle(light, m_angularDiameter));
  mat4 transformResult = calculateRotation(m_direction) * transform;
  CHECK(rprLightSetTransform(light, false, value_ptr(transformResult)))
  outInstances.push_back(light);
}

}
