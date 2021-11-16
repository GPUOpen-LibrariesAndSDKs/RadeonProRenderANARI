// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"
#include "OBJMaterial.h"
// specific types

namespace anari {
namespace rpr {

/*static FactoryMap<Material> g_cameras = {

    {"obj", []() -> Material * { return new OBJ; }}

}; */

Material *Material::createInstance(const char *type, rpr_material_system matsys)
{
  return new OBJMaterial(matsys);
  /*auto *fcn = g_cameras[type];

  if (fcn)
    return fcn();
  else {
    throw std::runtime_error("could not create material");
  } */
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Material *);

} // namespace anari
