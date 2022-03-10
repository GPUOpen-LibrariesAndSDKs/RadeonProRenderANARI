// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"
// specific types
#include "area/QuadLight.h"
#include "analytic/HdriLight.h"
#include "analytic/DirectionalLight.h"
#include "analytic/PointLight.h"
#include "analytic/SpotLight.h"

namespace anari {
namespace rpr {

Light::Light(rpr_context &context)  : m_context(context) {}


Light *Light::createInstance(rpr_context &context, rpr_material_system matsys, const char *type)
{
  if(std::strcmp(type, "quad")==0){
    return new QuadLight(context, matsys);
  }
  if(std::strcmp(type, "directional")==0){
    return new DirectionalLight(context);
  }
  if(std::strcmp(type, "point")==0){
    return new PointLight(context);
  }
  if(std::strcmp(type, "spot")==0){
    return new SpotLight(context);
  }
  if(std::strcmp(type, "hdri")==0){
    return new HdriLight(context);
  }
  else {
    throw std::runtime_error("could not create light");
  }

}

void Light::addToScene(rpr_scene scene)
{
  addToScene(scene, mat4(1));
}

void Light::commit()
{
  clear();
  m_visible = getParam<bool>("visible", true);
  m_color = getParam<vec3>("color", vec3(1.f));
}

void Light::clear()
{
  for(void *light : m_instances)
  {
    CHECK(rprObjectDelete(light))
  }
  m_instances.clear();
}

Light::~Light(){
  clear();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Light *);

} // namespace anari
