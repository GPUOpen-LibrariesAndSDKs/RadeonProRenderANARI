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

#include "../../sampler/image/Image1D.h"
#include "SpatialField.h"
#include "Volume.h"

namespace anari::rpr {

struct ScivisVolume : public Volume
{
 public:
  ScivisVolume(rpr_context context, rpr_material_system matsys);
  ~ScivisVolume() override;

  void commit() override;

 private:
  void           clear();
  mat4           getVolumeTransform() override;
  static float32 processDensity(float32 index, Array1D *densityLookup, rpr_image_filter_type filter);

  rpr_image         m_color_map{};
  rpr_material_node m_color_material{};
  rpr_material_node m_color_grid_material{};
  rpr_material_node m_density_grid_material{};
  rpr_grid          m_color_grid{};
  rpr_grid          m_density_grid{};
  mat4              m_transform{};
};

} // namespace anari::rpr
