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

#include "../../Array.h"
#include "../../rpr_common.h"

namespace anari::rpr {

struct Attribute
{
 public:
  explicit Attribute(rpr_material_node materialNode);
  Attribute() = default;
  virtual ~Attribute();

  static Attribute *fromType(rpr_material_system matsys, rpr_material_node_lookup_value type);
  rpr_material_node getMaterial();
  static vec4       processColor(Array1D *array, size_t index);

 protected:
  rpr_material_node m_material{};
};

} // namespace anari::rpr
