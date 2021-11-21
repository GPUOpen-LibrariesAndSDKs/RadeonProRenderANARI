// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "AnalyticLight.h"
#include "Math/mathutils.h"

namespace anari::rpr {

RadeonProRender::matrix calculateRotation(vec3 direction, vec3 startDirection=vec3(0,0,1)){
  float angle = acos(dot(normalize(direction), normalize(startDirection)));
  vec3 axis = cross(direction, startDirection);
  return RadeonProRender::rotation(RadeonProRender::float3(axis.x, axis.y, axis.z), angle);
}

AnalyticLight::AnalyticLight(rpr_context &context, const char *type) : Light(context) {
  m_type = type;
}

void AnalyticLight::commit()
{
  if(m_light){
    CHECK(rprObjectDelete(m_light))
    m_light = nullptr;
  }

  bool visible = getParam<bool>("visible", true);
  vec3 color = getParam<vec3>("color", vec3(1.f));

  if(strcmp(m_type, "directional") == 0) {
    auto irradiance = getParam<float>("irradiance", 1.f) * (float)visible;
    auto direction = getParam<vec3>( "direction", vec3(0, 0, 1));
    auto angle = getParam<float>("angularDiameter", 0);

    RadeonProRender::matrix transform = calculateRotation(vec3(direction.x, direction.y, -direction.z));

    CHECK(rprContextCreateDirectionalLight(m_context, &m_light))
    CHECK(rprDirectionalLightSetShadowSoftnessAngle(m_light, angle))
    CHECK(rprDirectionalLightSetRadiantPower3f(m_light, color.r * irradiance, color.g * irradiance, color.b * irradiance))
    CHECK(rprLightSetTransform(m_light, RPR_TRUE, &transform.m00))
  }

  else if(strcmp(m_type, "point") == 0) {

    auto position = getParam<vec3>( "position", vec3(0, 0, 0));
    auto radius = getParam<float>("radius", 0);
    auto intensity = getParam<float>("intensity", 1.f) * (float)visible;

    RadeonProRender::matrix transform = RadeonProRender::translation(RadeonProRender::float3(position.x, position.y, position.z));

    if(radius>0){
      //Sphere light only available with Northstar. Tahoe plugin DOESN'T support sphere lights
      CHECK(rprContextCreateSphereLight(m_context, &m_light))
      CHECK(rprSphereLightSetRadius(m_light, radius))
      CHECK(rprSphereLightSetRadiantPower3f(m_light, color.r * intensity, color.g * intensity, color.b * intensity))
    }
    else{
      CHECK(rprContextCreatePointLight(m_context, &m_light))
      CHECK(rprPointLightSetRadiantPower3f(m_light, color.r * intensity, color.g * intensity, color.b * intensity))
    }
    CHECK(rprLightSetTransform(m_light, RPR_TRUE, &transform.m00))
  }

  else if(strcmp(m_type, "spot") == 0) {

    auto position = getParam<vec3>( "position", vec3(0, 0, 0));
    auto direction = getParam<vec3>( "direction", vec3(0, 0, 1));
    auto openingAngle = getParam<float>("openingAngle", M_PI);
    auto falloffAngle = getParam<float>("falloffAngle", 0.1);
    auto intensity = getParam<float>("intensity", 1.f) * (float)visible;

    RadeonProRender::matrix transform = RadeonProRender::translation(RadeonProRender::float3(position.x, position.y, position.z));
    transform *= calculateRotation(vec3(direction.x, direction.y, -direction.z));

    CHECK(rprContextCreateSpotLight(m_context, &m_light))
    CHECK(rprSpotLightSetConeShape(m_light, falloffAngle, openingAngle))
    CHECK(rprSpotLightSetRadiantPower3f(m_light, color.r * intensity, color.g * intensity, color.b * intensity))
    CHECK(rprLightSetTransform(m_light, RPR_TRUE, &transform.m00))
  }

  else {
    throw std::runtime_error("could not create light");
  }

  markUpdated();


}

void AnalyticLight::addToScene(rpr_scene scene){
  CHECK(rprSceneAttachLight(scene, m_light))
  markAttached();
}


} // namespace anari
