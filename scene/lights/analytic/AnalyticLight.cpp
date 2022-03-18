#include "AnalyticLight.h"

namespace anari::rpr{

AnalyticLight::AnalyticLight(rpr_context &context) : Light(context){}

void AnalyticLight::addToScene(rpr_scene scene, mat4 transform)
{
  std::vector<void *> instances;
  getInstances(instances, transform);
  for(void *instance : instances)
  {
    CHECK(rprSceneAttachLight(scene, instance));
    m_instances.push_back(instance);
  }
}

mat4 AnalyticLight::calculateRotation(vec3 direction, vec3 startDirection){
  if(direction == startDirection)
  {
    return mat4(1); // no rotation
  }
  if(direction == -startDirection)
  {
    return rotate(mat4(1), pi<float32>(), vec3(1,0,0));  // reverse on 180 degrees
  }
  float angle = acos(dot(normalize(direction), normalize(startDirection)));
  vec3 axis = cross( startDirection, direction);
  return rotate(mat4(1), angle, axis);
}

float32 AnalyticLight::calculateSteradianAngle(float32 angle)
{
  return  (2 * pi<float32>() * (1 - cos(angle / 2)));
}

}