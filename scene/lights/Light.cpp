// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"
// specific types
#include "area/AreaLight.h"
#include "analytic/AnalyticLightOld.h"
#include "analytic/EnvironmentLight.h"

namespace anari {
namespace rpr {

Light::Light(rpr_context &context)  : m_context(context) {}


Light *Light::createInstance(rpr_context &context, rpr_material_system matsys, const char *type)
{
  if(std::strcmp(type, "quad")==0){
    return new AreaLight(context, matsys);
  }
  if(std::strcmp(type, "hdri")==0){
    return new EnvironmentLight(context, type);
  }
  else {
    return new AnalyticLightOld(context, type);
  }

}

Light::~Light(){
  if(m_light){
    CHECK(rprObjectDelete(m_light))
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Light *);

} // namespace anari
