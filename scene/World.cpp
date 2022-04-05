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

#include "World.h"
#include "geometry/Surface.h"
#include "Instance.h"
#include "lights/Light.h"
#include "volume/Volume.h"

namespace anari {
namespace rpr {

World::World()
{
  setCommitPriority(COMMIT_PRIORITY_WORLD);
}

void World::commit()
{
  auto surfaces  = getParamObject<ObjectArray>("surface");
  auto lights    = getParamObject<ObjectArray>("light");
  auto instances = getParamObject<ObjectArray>("instance");
  auto volumes   = getParamObject<ObjectArray>("volume");
  m_surfaces.clear();
  m_lights.clear();
  m_instances.clear();
  m_volumes.clear();

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

  if (volumes)
  {
    for (int volume_number = 0; volume_number < volumes->size(); volume_number++)
    {
      Volume *volume = ((Volume **)volumes->handles())[volume_number];
      extendBounds(volume->bounds());
      m_volumes.push_back(volume);
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

  if (instances)
  {
    for (int instance_number = 0; instance_number < instances->size(); instance_number++)
    {
      Instance *instance = ((Instance **)instances->handles())[instance_number];
      extendBounds(instance->bounds());
      m_instances.push_back(instance);
    }
  }

  markUpdated();
}

void World::attachToScene(rpr_scene scene)
{
  size_t prev_shapes_count;
  size_t prev_lights_count;
  rprSceneGetInfo(scene, RPR_SCENE_SHAPE_COUNT, sizeof(size_t), &prev_shapes_count, nullptr);
  rprSceneGetInfo(scene, RPR_SCENE_LIGHT_COUNT, sizeof(size_t), &prev_lights_count, nullptr);

  // detach all previous attached shapes
  if (prev_shapes_count > 0)
  {
    std::vector<rpr_shape> prev_shapes;
    prev_shapes.resize(prev_shapes_count);
    rprSceneGetInfo(
        scene, RPR_SCENE_SHAPE_LIST, prev_shapes_count * sizeof(rpr_shape), prev_shapes.data(), nullptr);
    for (rpr_shape shape : prev_shapes)
    {
      CHECK(rprSceneDetachShape(scene, shape))
    }
  }

  // detach all previous attached lights
  if (prev_lights_count > 0)
  {
    std::vector<rpr_light> prev_lights;
    prev_lights.resize(prev_lights_count);
    rprSceneGetInfo(
        scene, RPR_SCENE_LIGHT_LIST, prev_lights_count * sizeof(rpr_light), prev_lights.data(), nullptr);
    for (rpr_light light : prev_lights)
    {
      CHECK(rprSceneDetachLight(scene, light))
    }
  }

  for (Surface *surface : m_surfaces)
  {
    surface->attachToScene(scene);
  }

  for (Volume *volume : m_volumes)
  {
    volume->attachToScene(scene);
  }

  for (Light *light : m_lights)
  {
    light->attachToScene(scene);
  }

  for (Instance *instance : m_instances)
  {
    instance->attachToScene(scene);
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::World *);

} // namespace anari
