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

#include "../sampler/Sampler.h"
#include "Material.h"

namespace anari::rpr {

struct MatteMaterial : public Material
{
  explicit MatteMaterial(rpr_material_system);
  ~MatteMaterial() override;

  void commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

 private:
  vec3                  m_diffuse_color{};
  std::string           m_diffuse_color_attribute;
  IntrusivePtr<Sampler> m_diffuse_color_sampler;

  float                 m_opacity{};
  std::string           m_opacity_attribute;
  IntrusivePtr<Sampler> m_opacity_sampler;
  rpr_material_node     m_transparency_node{};
};

} // namespace anari::rpr
