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
#include "../../scene/geometry/Geometry.h"
#include "../Object.h"
#include "util/TransformNode.h"

namespace anari {
namespace rpr {

struct Sampler : public Object
{
 public:
  Sampler(rpr_context context, rpr_material_system matsys);
  static Sampler *createInstance(const char *type, rpr_context context, rpr_material_system matsys);
  inline virtual rpr_material_node generateMaterial(Geometry *geometry)
  {
    return nullptr;
  };

 protected:
  rpr_context         m_context;
  rpr_material_system m_matsys;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Sampler *, ANARI_SAMPLER);
ANARI_TYPEFOR_SPECIALIZATION(glm::mat4, ANARI_FLOAT32_MAT4)
} // namespace anari
