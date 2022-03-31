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

#include "../Geometry.h"

namespace anari::rpr {

// triangles and quad geometries
struct Mesh : public Geometry
{
 public:
  Mesh(rpr_context context, rpr_material_system materialSystem);
  void       getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) override;
  Attribute *getAttribute(const char *name) override;
  bool       hasAttribute(const char *name) override;

 protected:
  template <typename vecType, int polySize>
  void meshCommit(vecType defaultVector);

 private:
  void calculateBounds(Array1D *vertex_data);
  void processAttributes(Array1D *vertex_data);
  void processAttributeArray(Array1D *data, int key);
};
} // namespace anari::rpr
