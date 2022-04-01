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
#include "Sampler.h"
#include "util/TransformNode.h"

namespace anari::rpr {

struct TransformSampler : public Sampler
{
 public:
  TransformSampler(rpr_context context, rpr_material_system matsys);
  void              commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;
  ~TransformSampler() override;

 private:
  std::vector<TransformNode *> m_transform_nodes;
  mat4                         m_transform{};
  std::string                  m_input_attribute;

  void clearNodes();
};

} // namespace anari::rpr
