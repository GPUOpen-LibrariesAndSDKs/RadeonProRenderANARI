// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "World.h"
#include "Instance.h"

namespace anari {
namespace rpr {

void World::commit(){
  DataView<Instance *> instances(getParamObject<Data>("instance"));

  m_instances.clear();

  m_lower_bound = vec3(std::numeric_limits<float>::max());
  m_upper_bound = vec3(-std::numeric_limits<float>::max());

  for(int instance_number=0; instance_number<instances.size(); instance_number++){
    Instance * instance = instances.data()[instance_number];

    m_upper_bound.x = max(m_upper_bound.x, instance->m_upper_bound.x);
    m_upper_bound.y = max(m_upper_bound.y, instance->m_upper_bound.y);
    m_upper_bound.z = max(m_upper_bound.z, instance->m_upper_bound.z);

    m_lower_bound.x = min(m_lower_bound.x, instance->m_lower_bound.x);
    m_lower_bound.y = min(m_lower_bound.y, instance->m_lower_bound.y);
    m_lower_bound.z = min(m_lower_bound.z, instance->m_lower_bound.z);

    m_instances.push_back(instance);
  }
  is_changed = true;
}

void World::addToScene(rpr_scene scene){
  if (is_changed) {
    size_t prev_shapes_count;
    size_t prev_lights_count;
    rprSceneGetInfo(scene, RPR_SCENE_SHAPE_COUNT, sizeof(size_t), &prev_shapes_count, NULL);
    rprSceneGetInfo(scene, RPR_SCENE_LIGHT_COUNT, sizeof(size_t), &prev_lights_count, NULL);


    //detach all previous attached shapes
    if(prev_shapes_count > 0) {
      std::vector<rpr_shape> prev_shapes;
      prev_shapes.resize(prev_shapes_count);
      rprSceneGetInfo(scene, RPR_SCENE_SHAPE_LIST,
                      prev_shapes_count * sizeof(rpr_shape), prev_shapes.data(), NULL);
      for(rpr_shape shape : prev_shapes){
        CHECK(rprSceneDetachShape(scene, shape))
      }
    }

    //detach all previous attached lights
    if(prev_lights_count > 0){
      std::vector<rpr_light> prev_lights;
      prev_lights.resize(prev_lights_count);
      rprSceneGetInfo(scene, RPR_SCENE_LIGHT_LIST,
                      prev_lights_count * sizeof(rpr_light), prev_lights.data(), NULL);
      for(rpr_light light : prev_lights){
        CHECK(rprSceneDetachLight(scene, light))
      }
    }

    for(Instance* instance : m_instances){
      instance->addToScene(scene);
    }
    is_changed = false;
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::World *);

} // namespace anari
