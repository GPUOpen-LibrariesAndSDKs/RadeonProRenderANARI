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

#include "Group.h"
#include "geometry/Surface.h"
#include "lights/Light.h"
#include "volume/Volume.h"

namespace anari {
namespace rpr {

Group::Group()
{
  setCommitPriority(COMMIT_PRIORITY_GROUP);
}

void Group::commit()
{
  auto surfaces = getParamObject<ObjectArray>("surface");
  auto lights   = getParamObject<ObjectArray>("light");
  auto volumes  = getParamObject<ObjectArray>("volume");
  m_surfaces.clear();
  m_volumes.clear();
  m_lights.clear();

  resetBounds();

  if (surfaces)
  {
    for (int surface_number = 0; surface_number < surfaces->size(); surface_number++)
    {
      Surface *surface = ((Surface **)surfaces->handles())[surface_number];
      extendBounds(surface->bounds());
      m_surfaces.push_back(surface);
    }
  }

  if (lights)
  {
    for (int light_number = 0; light_number < lights->size(); light_number++)
    {
      Light *light = ((Light **)lights->handles())[light_number];
      m_lights.push_back(light);
    }
  }

  if (volumes)
  {
    for (int volume_number = 0; volume_number < volumes->size(); volume_number++)
    {
      Volume *volume = ((Volume **)volumes->handles())[volume_number];
      extendBounds(volume->bounds());
      m_volumes.push_back(volume);
    }
  }
}

void Group::attachToScene(rpr_scene scene, mat4 transform)
{
  for (Light *light : m_lights)
  {
    light->attachToScene(scene, transform);
  }
  for (Surface *surface : m_surfaces)
  {
    surface->attachToScene(scene, transform);
  }
  for (Volume *volume : m_volumes)
  {
    volume->attachToScene(scene, transform);
  }
}

} // namespace rpr
ANARI_TYPEFOR_DEFINITION(rpr::Group *);
} // namespace anari
