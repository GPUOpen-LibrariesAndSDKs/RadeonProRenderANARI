#include "RingLight.h"

namespace anari::rpr{

RingLight::RingLight(rpr_context context) : AnalyticLight(context){}

void RingLight::commit()
{
  Light::commit();
  m_position = getParam<vec3>("position", vec3(0));
  m_direction = getParam<vec3>("direction", vec3(0, 0, -1));
  m_outerAngle = getParam<float32>("openingAngle", pi<float32>());
  m_radius = getParam<float32>("radius", 0);
  auto intensity = getParam<float32>("intensity", 1.f);
  auto power = getParam<float32>("power", 1.f);
  auto radiance = getParam<float32>("radiance", 1.f);
  auto falloffAngle = getParam<float32>("falloffAngle", 0.1f);

  m_innerAngle = m_outerAngle - 2 * falloffAngle;

  if(hasParam("intensity") || !(hasParam("power") || !hasParam("radiance")))
  {
    m_power = intensity;
  }
  else if(hasParam("power") || !hasParam("radiance"))
  {
    m_power = power;
  }
  else
  {
    float32 area = pi<float32>() * m_radius * m_radius;
    m_power = radiance * area;
  }
}

void RingLight::getInstances(std::vector<void *> &outInstances, mat4 transform)
{
  rpr_light light;
  CHECK(rprContextCreateDiskLight(m_context, &light))
  CHECK(rprDiskLightSetRadius(light, m_radius))
  CHECK(rprDiskLightSetAngle(light, m_outerAngle))
  CHECK(rprDiskLightSetInnerAngle(light, m_innerAngle))
  CHECK(rprDiskLightSetRadiantPower3f(light, m_color.r * m_power, m_color.g * m_power, m_color.b * m_power))

  mat4 translation = translate(mat4(1), m_position);
  mat4 rotation = calculateRotation(m_direction);
  mat4 transformResult = translation * rotation;
  transformResult *= transform;
  CHECK(rprLightSetTransform(light, false, value_ptr(transformResult)))
  outInstances.push_back(light);
}
}
