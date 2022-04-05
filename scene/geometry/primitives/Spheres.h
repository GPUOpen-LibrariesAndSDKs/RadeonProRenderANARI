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

struct Spheres : public Primitives
{
  Spheres(rpr_context context, rpr_material_system materialSystem);
  void       commit() override;
  Attribute *getAttribute(const char *name) override;

 private:
  void      createBaseShape();
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  int m_sectors = 32;
  int m_rings   = 32;

  Array1D *m_vertices{};
  Array1D *m_radius{};
  Array1D *m_indices{};
  float32  m_globalRadius{};
};

} // namespace anari::rpr
