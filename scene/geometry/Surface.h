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

#include "../../material/Material.h"
#include "../../rpr_common.h"
#include "../SceneObject.h"
#include "Geometry.h"

namespace anari {
namespace rpr {

struct Surface : public SceneObject
{
  explicit Surface(rpr_material_system materialSystem);
  ~Surface() override;

  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform = mat4(1));
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform);

 private:
  IntrusivePtr<Geometry> m_geometry;
  std::vector<rpr_shape> m_instances;

  rpr_material_system m_matsys;
  rpr_material_node   m_material_instance{};

  void clearInstances();
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Surface *, ANARI_SURFACE);

} // namespace anari
