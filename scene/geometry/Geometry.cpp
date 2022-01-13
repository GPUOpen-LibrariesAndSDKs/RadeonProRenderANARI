// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// specific types
#include "Mesh.h"
#include "Spheres.h"

namespace anari {
namespace rpr {

Geometry::Geometry(rpr_context &context) : m_context(context){
    setCommitPriority(COMMIT_PRIORITY_GEOMETRY);
}

Geometry *Geometry::createInstance(rpr_context &context, const char *type)
{
  if(std::strcmp(type, "triangle")==0 || std::strcmp(type, "mesh")==0){
    return new Mesh(context);
  }
  if(std::strcmp(type, "sphere")==0){
    return new Spheres(context);
  }
  throw std::runtime_error("could not create geometry");

}

rpr_shape Geometry::getBaseShape()
{
  return m_base_shape;
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Geometry *);

} // namespace anari
