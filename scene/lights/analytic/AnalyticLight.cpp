#include "AnalyticLight.h"

namespace anari::rpr{

AnalyticLight::AnalyticLight(rpr_context &context) : Light(context){}

void AnalyticLight::addToScene(rpr_scene scene)
{
  std::vector<void *> instances;
  getInstances(instances, mat4(1));
  for(void *instance : instances)
  {
    CHECK(rprSceneAttachLight(scene, instance));
    m_instances.push_back(instance);
  }
}

mat4 AnalyticLight::calculateRotation(vec3 direction, vec3 startDirection){
  float angle = acos(dot(normalize(direction), normalize(startDirection)));
  vec3 axis = cross( startDirection, direction);
  if(direction == startDirection)
  {
    return mat4(1); // no rotation
  }
  if(direction == -startDirection){
    return rotate(mat4(-1), pi<float>(), vec3(0,0,1)); // reverse
  }
  return rotate(mat4(1), angle, axis);
}

}