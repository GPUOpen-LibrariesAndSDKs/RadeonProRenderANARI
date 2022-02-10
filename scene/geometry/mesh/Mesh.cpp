#include "Mesh.h"
#include "../attributes/PrimVarAttribute.h"

namespace anari::rpr{

Mesh::Mesh(rpr_context context, rpr_material_system materialSystem) : Geometry(context, materialSystem) {}

void Mesh::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  rpr_shape instance;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
  CHECK(rprShapeSetTransform(instance, false, value_ptr(transform)))
  out_shapes.push_back(instance);
}

Attribute *Mesh::getAttribute(const char *name)
{
  if(std::strcmp(name, "attribute0") == 0) return createPrimVarAttribute(0);
  if(std::strcmp(name, "attribute1") == 0) return createPrimVarAttribute(1);
  if(std::strcmp(name, "attribute2") == 0) return createPrimVarAttribute(2);
  if(std::strcmp(name, "attribute3") == 0) return createPrimVarAttribute(3);
  if(std::strcmp(name, "color") == 0) return createPrimVarAttribute(4);
  return Geometry::getAttribute(name);
}

bool Mesh::hasAttribute(const char *name) {
  if (std::strcmp(name, "color") == 0 || std::strcmp(name, "attribute0") == 0 ||
      std::strcmp(name, "attribute1") == 0 || std::strcmp(name, "attribute2") == 0 ||
      std::strcmp(name, "attribute3") == 0)
  {
    return true;
  }
  return Geometry::hasAttribute(name);
}

Attribute *Mesh::createPrimVarAttribute(int key) {
  Attribute* attribute = new PrimVarAttribute(m_matsys, key);
  m_attributes.push_back(attribute);
  return attribute;
}

void Mesh::calculateBounds(Array1D *vertex)
{
  resetBounds();

  for(int i=0; i<vertex->size()*3; i++){
    float coordinate = ((float*) vertex->dataAs<vec3>())[i];
    if(i%3==0){  //x coordinate
      m_bounds.upper.x = max(m_bounds.upper.x, coordinate);
      m_bounds.lower.x = min(m_bounds.lower.x, coordinate);
    }
    if(i%3==1){  //y coordinate
      m_bounds.upper.y = max(m_bounds.upper.y, coordinate);
      m_bounds.lower.y = min(m_bounds.lower.y, coordinate);
    }
    if(i%3==2){  //z coordinate
      m_bounds.upper.z = max(m_bounds.upper.z, coordinate);
      m_bounds.lower.z = min(m_bounds.lower.z, coordinate);
    }
  }
}

void Mesh::applyAttribute(Array1D *data, int key)
{
  if(!data){
    return;
  }

  switch (data->elementType()) {
  case ANARI_FLOAT32:{
    CHECK(rprShapeSetPrimvar(m_base_shape, key, data->dataAs<float>(), data->size(), 1, RPR_PRIMVAR_INTERPOLATION_VERTEX))
    return;
  }
  case ANARI_FLOAT32_VEC2:{
    CHECK(rprShapeSetPrimvar(m_base_shape, key, (rpr_float *)data->dataAs<vec2>(), data->size() * 2, 2, RPR_PRIMVAR_INTERPOLATION_VERTEX))
    return;
  }
  case ANARI_FLOAT32_VEC3:{
    CHECK(rprShapeSetPrimvar(m_base_shape, key, (rpr_float *)data->dataAs<vec3>(), data->size() * 3, 3, RPR_PRIMVAR_INTERPOLATION_VERTEX))
    return;
  }
  case ANARI_FLOAT32_VEC4:{
    CHECK(rprShapeSetPrimvar(m_base_shape, key, (rpr_float *)data->dataAs<vec4>(), data->size() * 4, 4, RPR_PRIMVAR_INTERPOLATION_VERTEX))
    return;
  }

  }
}

} //namespace anari::rpr
