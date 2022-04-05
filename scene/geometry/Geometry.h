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

#include <map>
#include "../../rpr_common.h"
#include "../SceneObject.h"
#include "attributes/Attribute.h"

namespace anari {
namespace rpr {

struct Geometry : public SceneObject
{
  Geometry(rpr_context context, rpr_material_system m_matsys);
  ~Geometry() override;

  static Geometry *createInstance(rpr_context context, rpr_material_system materialSystem, const char *type);
  virtual void     getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform){};
  virtual Attribute *getAttribute(const char *name);
  virtual bool       hasAttribute(const char *name);
  rpr_shape          getBaseShape();

 protected:
  rpr_context         m_context;
  rpr_material_system m_matsys; // material system is using to create lookup nodes for attributes
  rpr_shape           m_base_shape{};
  std::map<std::string, Attribute *> m_attribute_map;

  void        clearAttributes();
  static void checkArraySizes(Array1D *array, size_t size, std::runtime_error exception);
  Attribute  *createPerShapeAttribute(const std::vector<vec4> &data, const char *name);
  Attribute  *createPrimVarAttribute(int key, const char *name);
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
