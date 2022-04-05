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

#include "Cones.h"

namespace anari::rpr {

struct Cylinders : public Cones
{
 public:
  Cylinders(rpr_context context, rpr_material_system materialSystem);
  void commit() override;

 private:
  rpr_shape getPrimitive(int primitive_number, mat4 externalTransform) override;

  float32 m_globalRadius{};
};

} // namespace anari::rpr
