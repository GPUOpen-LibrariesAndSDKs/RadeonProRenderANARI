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

#include "HdriLight.h"

namespace anari::rpr {

HdriLight::HdriLight(rpr_context context) : AnalyticLight(context) {}

void HdriLight::commit()
{
  Light::commit();
  clear();

  m_direction    = getParam<vec3>("direction", vec3(1, 0, 0));
  m_up           = getParam<vec3>("up", vec3(1, 0, 0));
  m_scale        = getParam<float32>("scale", 1.f);
  auto imageData = getParamObject<Array2D>("radiance");

  if (!imageData) // image array has not been provided, use color parameter
  {
    rpr_image_desc desc        = {1, 1, 0, 3 * sizeof(rpr_float), 0};
    float          bg_color[3] = {m_color.r, m_color.g, m_color.b};
    CHECK(rprContextCreateImage(m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &desc, bg_color, &m_image))
  } else
  {
    rpr_image_desc desc;
    desc.image_width       = imageData->size(0);
    desc.image_height      = imageData->size(1);
    desc.image_depth       = 0;
    desc.image_row_pitch   = imageData->size(0) * 3 * sizeof(rpr_float);
    desc.image_slice_pitch = 0;
    CHECK(rprContextCreateImage(
        m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &desc, imageData->dataAs<vec3>(), &m_image))
  }
}

void HdriLight::getInstances(std::vector<void *> &outInstances, mat4 transform)
{
  rpr_light light;
  CHECK(rprContextCreateEnvironmentLight(m_context, &light))
  CHECK(rprEnvironmentLightSetImage(light, m_image))
  CHECK(rprEnvironmentLightSetIntensityScale(light, m_scale))
  mat4 upRotation      = calculateRotation(m_up, vec3(1, 0, 0));
  mat4 dirRotation     = calculateRotation(m_direction, vec3(1, 0, 0));
  mat4 transformResult = upRotation * dirRotation;
  transformResult *= transform;
  CHECK(rprLightSetTransform(light, false, value_ptr(transformResult)))
  outInstances.push_back(light);
}

void HdriLight::clear()
{
  if (m_image)
  {
    CHECK(rprObjectDelete(m_image))
    m_image = nullptr;
  }
}

HdriLight::~HdriLight()
{
  clear();
}

} // namespace anari::rpr