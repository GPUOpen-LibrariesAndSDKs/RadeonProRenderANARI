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

#include "../Geometry.h"

namespace anari::rpr {

// sphere, cone, cylinder geometries
struct Primitives : public Geometry
{
 public:
  Primitives(rpr_context context, rpr_material_system materialSystem);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;
  bool hasAttribute(const char *name) override;

 protected:
  virtual rpr_shape getPrimitive(int primitive_number, mat4 externalTransform)
  {
    return nullptr;
  };
  static void processAttributeArray(Array1D *data, Array1D *index, std::vector<vec4> &outVector);
  void        processAttributeParameters(Array1D *index);

  size_t            m_num_primitives{};
  std::vector<vec4> m_colors;
  std::vector<vec4> m_attribute0;
  std::vector<vec4> m_attribute1;
  std::vector<vec4> m_attribute2;
  std::vector<vec4> m_attribute3;
};

} // namespace anari::rpr
