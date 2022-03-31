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

#include "../SceneObject.h"

namespace anari {
namespace rpr {

struct SpatialField : public SceneObject
{
  friend struct ScivisVolume;

  SpatialField() = default;
  static SpatialField *createInstance(const char *type);

 protected:
  static rpr_image_filter_type processFilter(const std::string &name);

  std::vector<float32>  m_grid;
  std::vector<uint32>   m_indices;
  uvec3                 m_size{};
  vec3                  m_position{};
  vec3                  m_voxelSize{};
  rpr_image_filter_type m_filterType{};
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::SpatialField *, ANARI_SPATIAL_FIELD)
} // namespace anari
