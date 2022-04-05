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

#pragma once

#include "AnalyticLight.h"

namespace anari::rpr {

struct SpotLight : public AnalyticLight
{
 public:
  explicit SpotLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void *> &outInstances, mat4 transform) override;

 private:
  vec3    m_position{};
  vec3    m_direction{};
  float32 m_innerAngle{};
  float32 m_outerAngle{};
  float32 m_power{};
};

} // namespace anari::rpr