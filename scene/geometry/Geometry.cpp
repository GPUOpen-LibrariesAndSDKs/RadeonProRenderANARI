// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// specific types
#include "Mesh.h"
#include "Spheres.h"

namespace anari {
namespace rpr {


Geometry *Geometry::createInstance(rpr_context &context, const char *type)
{
  if(std::strcmp(type, "triangle")==0){
    return new Mesh(context);
  }
  if(std::strcmp(type, "sphere")==0){
    return new Spheres(context);
  }
  throw std::runtime_error("could not create geometry");

}

void Geometry::addToScene(rpr_scene scene){
    for(rpr_shape shape : m_shapes){
        CHECK(rprSceneAttachShape(scene, shape))
    }
}

void Geometry::applyMaterial(rpr_material_node material) {
    for(rpr_shape shape : m_shapes){
        CHECK(rprShapeSetMaterial(shape, material))
    }
    markUpdated();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Geometry *);

} // namespace anari
