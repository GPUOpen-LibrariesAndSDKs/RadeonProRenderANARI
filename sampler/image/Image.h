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
#include <map>
#include "../Sampler.h"

namespace anari::rpr {

struct Image : public Sampler
{
 public:
  Image(rpr_context context, rpr_material_system matsys);
  void commit() override;
  ~Image() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

 protected:
  static rpr_image_filter_type processFilter(const std::string &name);
  static rpr_image_wrap_type   processWrap(const std::string &name);

  rpr_material_node applyTransformNode(mat4 transform, rpr_material_node input);
  void              clearInstances();

  std::string           m_input_attribute;
  mat4                  m_input_transform{};
  mat4                  m_output_transform{};
  rpr_image             m_image{};
  rpr_image_filter_type m_filter{};

  std::vector<TransformNode *>   m_transform_nodes;
  std::vector<rpr_material_node> m_instances;
};

} // namespace anari::rpr
