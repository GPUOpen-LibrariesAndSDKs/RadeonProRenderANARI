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

#include "AnalyticLight.h"

namespace anari::rpr {

AnalyticLight::AnalyticLight(rpr_context &context) : Light(context) {}

void AnalyticLight::attachToScene(rpr_scene scene, mat4 transform)
{
  std::vector<void *> instances;
  getInstances(instances, transform);
  for (void *instance : instances)
  {
    CHECK(rprSceneAttachLight(scene, instance));
    m_instances.push_back(instance);
  }
}

float32 AnalyticLight::calculateSteradianAngle(float32 angle)
{
  return (2 * pi<float32>() * (1 - cos(angle / 2)));
}

} // namespace anari::rpr