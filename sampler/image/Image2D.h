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
#include "Image.h"

namespace anari::rpr {

struct Image2D : public Image
{
 public:
  Image2D(rpr_context context, rpr_material_system matsys);
  void              commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

 private:
  rpr_image_wrap_type m_wrap_mode_u{};
  rpr_image_wrap_type m_wrap_mode_v{};
};

} // namespace anari::rpr
