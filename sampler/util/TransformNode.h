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

#include "../../glm_extras.h"
#include "../../rpr_common.h"

namespace anari::rpr {

struct TransformNode
{
 public:
  TransformNode(rpr_material_system matsys, mat4 transform_matrix, rpr_material_node input);
  ~TransformNode();

  rpr_material_node getMaterial();

 private:
  rpr_material_node m_dot0{};
  rpr_material_node m_dot1{};
  rpr_material_node m_dot2{};
  rpr_material_node m_dot3{};
  rpr_material_node m_transform{};
  rpr_material_node m_r{};
  rpr_material_node m_g{};
  rpr_material_node m_b{};
  rpr_material_node m_a{};
  rpr_material_node m_rg{};
  rpr_material_node m_rgb{};
};

} // namespace anari::rpr
