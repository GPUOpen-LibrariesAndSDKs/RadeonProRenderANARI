// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"
#include "OBJMaterial.h"
#include "SimpleMaterial.h"
// specific types

namespace anari {
namespace rpr {


Material *Material::createInstance(const char *type, rpr_material_system matsys)
{
  if(strcmp(type, "matte") == 0 || strcmp(type, "transparentMatte") == 0){
      return new SimpleMaterial(matsys);
  }
  if(strcmp(type, "obj") == 0){
      return new OBJMaterial(matsys);
  }
  throw std::runtime_error("cannot create material");
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Material *);

} // namespace anari