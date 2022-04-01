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

#include "../Light.h"

namespace anari::rpr {

struct QuadLight : public Light
{
  QuadLight(rpr_context &context, rpr_material_system &matsys);
  ~QuadLight() override;

  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform) override;

 private:
  rpr_material_node   m_material{};
  rpr_material_system m_matsys;

  std::vector<vec3> m_vertex;
  std::vector<vec3> m_normal;
  rpr_int           m_faces[1] = {4};
  rpr_int           m_index[4] = {0, 1, 3, 2};

  void clear();
};

} // namespace anari::rpr
