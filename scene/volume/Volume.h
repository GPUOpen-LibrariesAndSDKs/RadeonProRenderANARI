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

struct Volume : public SceneObject
{
 public:
  Volume(rpr_context context, rpr_material_system matsys);
  ~Volume() override;

  static Volume *createInstance(const char *type, rpr_context context, rpr_material_system matsys);
  void           commit() override;
  void           attachToScene(rpr_scene scene, mat4 transform = mat4(1));

 protected:
  virtual mat4 getVolumeTransform()
  {
    return mat4(1);
  };

  rpr_context         m_context;
  rpr_material_system m_matsys;
  rpr_material_node   m_volume_material{};

 private:
  void clear();

  std::vector<rpr_shape> m_instances;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Volume *, ANARI_VOLUME)

} // namespace anari
