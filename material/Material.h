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

#include "../Object.h"
#include "../rpr_common.h"
#include "../scene/geometry/Geometry.h"

namespace anari {
namespace rpr {

struct Material : public Object
{
  Material() = default;
  static Material *createInstance(const char *type, rpr_material_system matsys);

  virtual inline rpr_material_node generateMaterial(Geometry *geometry)
  {
    return nullptr;
  };

 protected:
  rpr_material_system m_matsys{};
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Material *, ANARI_MATERIAL)

} // namespace anari
