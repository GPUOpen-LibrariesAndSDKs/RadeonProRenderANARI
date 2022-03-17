// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// specific types
#include "mesh/Triangles.h"
#include "mesh/Quad.h"
#include "primitives/Spheres.h"

namespace anari {
namespace rpr {

Geometry::Geometry(rpr_context context, rpr_material_system materialSystem) : m_context(context), m_matsys(materialSystem){
    setCommitPriority(COMMIT_PRIORITY_GEOMETRY);
}

Geometry *Geometry::createInstance(rpr_context context, rpr_material_system materialSystem, const char *type)
{
  if(std::strcmp(type, "triangle")==0){
    return new Triangles(context, materialSystem);
  }
  if(std::strcmp(type, "quad")==0){
    return new Quad(context, materialSystem);
  }
  if(std::strcmp(type, "sphere")==0){
    return new Spheres(context, materialSystem);
  }
  throw std::runtime_error("could not create geometry");

}

rpr_shape Geometry::getBaseShape()
{
  return m_base_shape;
}

Attribute *Geometry::getAttribute(const char *name)
{
  if(m_attribute_map.find(name) != m_attribute_map.end()){
    return m_attribute_map[name];
  }
  if(std::strcmp(name, "worldPosition") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_P);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }
  if(std::strcmp(name, "objectPosition") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_P_LOCAL);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }
  if(std::strcmp(name, "objectNormal") == 0 || std::strcmp(name, "worldNormal") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_N);
    m_attribute_map.emplace(name, attribute);
    return attribute;
  }
  return nullptr;
}

bool Geometry::hasAttribute(const char *name)
{
  return std::strcmp(name, "worldPosition") == 0 || std::strcmp(name, "worldNormal") == 0
      || std::strcmp(name, "objectPosition") == 0 || std::strcmp(name, "objectNormal") == 0;
}

void Geometry::clearAttributes()
{
  for(std::pair<std::string, Attribute*> p : m_attribute_map)
  {
    delete p.second;
  }
  m_attribute_map.clear();
}

void Geometry::checkArraySizes(Array1D *array, size_t size, std::runtime_error exception)
{
  if(array && array->size() != size){
    throw exception;
  }
}

Geometry::~Geometry(){
  if(m_base_shape)
  {
    CHECK(rprObjectDelete(m_base_shape))
  }
  clearAttributes();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Geometry *);

} // namespace anari
