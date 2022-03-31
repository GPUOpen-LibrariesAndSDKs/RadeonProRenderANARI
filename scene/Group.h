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

#include "../rpr_common.h"
#include "SceneObject.h"

namespace anari {
namespace rpr {

struct Surface;
struct Light;
struct Volume;

struct Group : public SceneObject
{
 public:
  Group();
  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform);

 private:
  std::vector<Surface *> m_surfaces;
  std::vector<Volume *>  m_volumes;
  std::vector<Light *>   m_lights;
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::Group *, ANARI_GROUP)
} // namespace anari
