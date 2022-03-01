// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "EnvironmentLight.h"

namespace anari::rpr {

EnvironmentLight::EnvironmentLight(rpr_context &context, const char *type)  : AnalyticLightOld(context, type){}

void EnvironmentLight::commit(){

  if(m_color_texture){
    CHECK(rprObjectDelete(m_color_texture))
    m_color_texture = nullptr;
  }

  if(m_light){
    CHECK(rprObjectDelete(m_light))
    m_light = nullptr;
  }

  auto intensity = getParam<float>("intensity", 1.f);
  vec3 color = getParam<vec3>("color", vec3(1.f)) * intensity;
  bool visible = getParam<bool>("visible", true);

  if(strcmp(m_type, "ambient") == 0) {
    CHECK(rprContextCreateEnvironmentLight(m_context, &m_light))

    rpr_image_format format = {4, RPR_COMPONENT_TYPE_FLOAT32};
    rpr_image_desc desc = {1, 1, 0, 3 * sizeof(rpr_float), 0};
    float bg_color[4] = {color.r, color.g, color.b, 1.f};
    CHECK(rprContextCreateImage(m_context, format, &desc, bg_color, &m_color_texture))

    CHECK(rprEnvironmentLightSetImage(m_light, m_color_texture))
    CHECK(rprEnvironmentLightSetIntensityScale(m_light, intensity))
  }

  else if(strcmp(m_type, "hdri") == 0) {
    CHECK(rprContextCreateEnvironmentLight(m_context, &m_light))

    // TODO create color texture

    CHECK(rprEnvironmentLightSetIntensityScale(m_light, intensity))
  }

  else {
    throw std::runtime_error("Unhandled Light type!");
  }
  markUpdated();

}

EnvironmentLight::~EnvironmentLight(){
  if(m_color_texture){
    CHECK(rprObjectDelete(m_color_texture))
  }
}


} // namespace anari