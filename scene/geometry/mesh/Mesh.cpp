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
  // mock while primvars is not supported in core
  // TODO remove it after primvars will have been implemented
  if(std::strcmp(name, "color") == 0)
  {
    Attribute* attribute = new PrimVarAttribute(m_matsys, 4);
    m_attributes.push_back(attribute);
    return attribute;
  }
  if(std::strcmp(name, "attribute0") == 0)
  {
    Attribute* attribute = Attribute::fromType(m_matsys, RPR_MATERIAL_NODE_LOOKUP_UV);
    m_attributes.push_back(attribute);
    return attribute;
  }
  return Geometry::getAttribute(name);
}

bool Mesh::hasAttribute(const char *name) {
  if (std::strcmp(name, "attribute0") == 0 || std::strcmp(name, "color") == 0)
  {
    return true;
  }
  return Geometry::hasAttribute(name);
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

void Mesh::applyColor(Array1D *color)
{
  if(color){
    CHECK(rprShapeSetPrimvar(m_base_shape, 4, (rpr_float *)color->dataAs<vec4>(), color->size() * 4, 4, RPR_PRIMVAR_INTERPOLATION_VERTEX))
  }
}

} //namespace anari::rpr
