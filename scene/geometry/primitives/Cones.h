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

#include "Primitives.h"

namespace anari::rpr {

struct Cones : public Primitives
{
 public:
  Cones(rpr_context context, rpr_material_system materialSystem);
  void       commit() override;
  Attribute *getAttribute(const char *name) override;

 protected:
  // can be used to create cylinder shape if up and down radius are same
  rpr_shape   createConeShape(vec3 firstVertex, vec3 secondVertex, float firstRadius, float secondRadius,
        bool firstCap, bool secondCap, mat4 externalTransform, int numSegments);
  static box3 calculateConeBounds(vec3 firstVertex, vec3 secondVertex, float firstRadius, float secondRadius);
  void        processConeAttributes(rpr_shape cone, uvec2 index);
  static void processConeAttribute(
      rpr_shape cone, const std::vector<vec4> &colors, uvec2 index, int key, int numSegments);
  static uvec2 processConeCaps(const std::string &name);

  Array1D *m_indices{};
  Array1D *m_radius{};
  Array1D *m_vertices{};
  Array1D *m_caps{};
  uvec2    m_globalCaps{};

  int m_numSegments = 32;

 private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;
};

} // namespace anari::rpr