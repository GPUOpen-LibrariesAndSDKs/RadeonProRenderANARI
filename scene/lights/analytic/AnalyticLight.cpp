#include "AnalyticLight.h"

namespace anari::rpr{

AnalyticLight::AnalyticLight(rpr_context &context) : Light(context){}

void AnalyticLight::attachToScene(rpr_scene scene, mat4 transform)
{
  std::vector<void *> instances;
  getInstances(instances, transform);
  for(void *instance : instances)
  {
    CHECK(rprSceneAttachLight(scene, instance));
    m_instances.push_back(instance);
  }
}

float32 AnalyticLight::calculateSteradianAngle(float32 angle)
{
  return  (2 * pi<float32>() * (1 - cos(angle / 2)));
}

}