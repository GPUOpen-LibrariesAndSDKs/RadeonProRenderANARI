// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "World.h"
#include "geometry/Surface.h"
#include "lights/Light.h"

namespace anari {
namespace rpr {

void World::commit(){

  auto surfaces = getParamObject<ObjectArray>("surface");
  auto lights = getParamObject<ObjectArray>("light");
  m_surfaces.clear();
  m_lights.clear();

  resetBounds();

  for(int surface_number=0; surface_number < surfaces->size(); surface_number++){
    Surface * surface = surfaces->dataAs<Surface>() + surface_number;
    extendBounds(surface->bounds());
    m_surfaces.push_back(surface);
  }

  for(int light_number=0; light_number < lights->size(); light_number++){
    Light* light = lights->dataAs<Light>() + light_number;
    m_lights.push_back(light);
  }
  markUpdated();
}

void World::addToScene(rpr_scene scene){

    size_t prev_shapes_count;
    size_t prev_lights_count;
    rprSceneGetInfo(scene, RPR_SCENE_SHAPE_COUNT, sizeof(size_t), &prev_shapes_count, nullptr);
    rprSceneGetInfo(scene, RPR_SCENE_LIGHT_COUNT, sizeof(size_t), &prev_lights_count, nullptr);


    //detach all previous attached shapes
    if(prev_shapes_count > 0) {
      std::vector<rpr_shape> prev_shapes;
      prev_shapes.resize(prev_shapes_count);
      rprSceneGetInfo(scene, RPR_SCENE_SHAPE_LIST,
                      prev_shapes_count * sizeof(rpr_shape), prev_shapes.data(), nullptr);
      for(rpr_shape shape : prev_shapes){
        CHECK(rprSceneDetachShape(scene, shape))
      }
    }

    //detach all previous attached lights
    if(prev_lights_count > 0){
      std::vector<rpr_light> prev_lights;
      prev_lights.resize(prev_lights_count);
      rprSceneGetInfo(scene, RPR_SCENE_LIGHT_LIST,
                      prev_lights_count * sizeof(rpr_light), prev_lights.data(), nullptr);
      for(rpr_light light : prev_lights){
        CHECK(rprSceneDetachLight(scene, light))
      }
    }

    for(Surface* surface : m_surfaces){
        surface->addToScene(scene);
    }

    for(Light* light : m_lights){
        light->addToScene(scene);
    }
    markAttached();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::World *);

} // namespace anari