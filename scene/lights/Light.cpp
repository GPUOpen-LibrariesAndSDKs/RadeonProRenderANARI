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

#include "Light.h"
// specific types
#include "analytic/DirectionalLight.h"
#include "analytic/HdriLight.h"
#include "analytic/PointLight.h"
#include "analytic/RingLight.h"
#include "analytic/SpotLight.h"
#include "area/QuadLight.h"

namespace anari {
namespace rpr {

Light::Light(rpr_context &context) : m_context(context) {}

Light *Light::createInstance(rpr_context &context, rpr_material_system matsys, const char *type)
{
  if (std::strcmp(type, "quad") == 0)
  {
    return new QuadLight(context, matsys);
  }
  if (std::strcmp(type, "directional") == 0)
  {
    return new DirectionalLight(context);
  }
  if (std::strcmp(type, "point") == 0)
  {
    return new PointLight(context);
  }
  if (std::strcmp(type, "spot") == 0)
  {
    return new SpotLight(context);
  }
  if (std::strcmp(type, "hdri") == 0)
  {
    return new HdriLight(context);
  }
  if (std::strcmp(type, "ring") == 0)
  {
    return new RingLight(context);
  } else
  {
    throw std::runtime_error("could not create light");
  }
}

void Light::attachToScene(rpr_scene scene)
{
  attachToScene(scene, mat4(1));
}

void Light::commit()
{
  clear();
  m_visible = getParam<bool>("visible", true);
  m_color   = getParam<vec3>("color", vec3(1.f));
}

void Light::clear()
{
  for (void *light : m_instances)
  {
    CHECK(rprObjectDelete(light))
  }
  m_instances.clear();
}

Light::~Light()
{
  clear();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Light *);

} // namespace anari
