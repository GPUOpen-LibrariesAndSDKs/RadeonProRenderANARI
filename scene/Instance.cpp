// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Instance.h"
#include "geometry/GeometricModel.h"

namespace anari {
namespace rpr {

void Instance::commit(){
  DataView<GeometricModel *> models(getParamObject<Data>("geometry"));
  DataView<Light *> lights(getParamObject<Data>("light"));

  m_shapes.clear();
  m_lights.clear();

  m_lower_bound = vec3(std::numeric_limits<float>::max());
  m_upper_bound = vec3(-std::numeric_limits<float>::max());

  for(int model_number=0; model_number< models.size(); model_number++){
    GeometricModel* model = models.data()[model_number];

    //bounds calculating
    m_upper_bound.x = max(m_upper_bound.x, model->m_upper_bound.x);
    m_upper_bound.y = max(m_upper_bound.y, model->m_upper_bound.y);
    m_upper_bound.z = max(m_upper_bound.z, model->m_upper_bound.z);

    m_lower_bound.x = min(m_lower_bound.x, model->m_lower_bound.x);
    m_lower_bound.y = min(m_lower_bound.y, model->m_lower_bound.y);
    m_lower_bound.z = min(m_lower_bound.z, model->m_lower_bound.z);

    for(rpr_shape shape : model->m_shapes){
      m_shapes.push_back(shape);
    }
    //TODO skip non unique shapes
  }

  for(int light_number=0; light_number<lights.size(); light_number++){
    m_lights.push_back(lights.data()[light_number]);
  }
}

void Instance::addToScene(rpr_scene scene){

  for(rpr_shape shape : m_shapes){
    CHECK(rprSceneAttachShape(scene, shape))
  }

  for(Light* light : m_lights){
    light->addToScene(scene);
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Instance *);

} // namespace anari
