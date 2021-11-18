// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"
// specific types
#include "AreaLight.h"
#include "AnalyticLight.h"
#include "EnvironmentLight.h"

namespace anari {
namespace rpr {


Light *Light::createInstance(rpr_context &context, rpr_material_system matsys, const char *type)
{
  if(std::strcmp(type, "quad")==0){
    return new AreaLight(context, matsys);
  }
  if(std::strcmp(type, "hdri")==0){
    return new EnvironmentLight(context, type);
  }
  else {
    return new AnalyticLight(context, type);
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
