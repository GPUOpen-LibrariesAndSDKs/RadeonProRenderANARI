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

#include "../../rpr_common.h"
#include "../SceneObject.h"

namespace anari {
namespace rpr {

struct Light : public SceneObject
{
 public:
  explicit Light(rpr_context &context);
  ~Light() override;

  static Light       *createInstance(rpr_context &context, rpr_material_system matsys, const char *type);
  void                commit() override;
  virtual inline void attachToScene(rpr_scene scene, mat4 transform){};
  void                attachToScene(rpr_scene scene);
  virtual inline void getInstances(std::vector<void *> &outInstances, mat4 transform){};

 protected:
  rpr_context         m_context;
  std::vector<void *> m_instances; // this could be rpr_light or rpr_shape

  vec3 m_color{};
  bool m_visible{};

 private:
  void clear();
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Light *, ANARI_LIGHT)

} // namespace anari
