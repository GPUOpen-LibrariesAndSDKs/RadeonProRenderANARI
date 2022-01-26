#include "Mesh.h"

namespace anari::rpr{

Mesh::Mesh(rpr_context context, rpr_material_system materialSystem) : Geometry(context, materialSystem) {}

void Mesh::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  rpr_shape instance;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
  CHECK(rprShapeSetTransform(instance, false, value_ptr(transform)))
  out_shapes.push_back(instance);
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
    rpr_int num_color_vertex = color->size();
    std::vector<rpr_float> r;
    std::vector<rpr_float> g;
    std::vector<rpr_float> b;
    std::vector<rpr_float> a;
    std::vector<rpr_int> color_index;
    vec4 *colorData = color->dataAs<vec4>();
    for(int i=0; i<num_color_vertex; i++){
      r.push_back(colorData[i].r);
      g.push_back(colorData[i].g);
      b.push_back(colorData[i].b);
      a.push_back(colorData[i].a);
      color_index.push_back(i);
    }
    CHECK(rprShapeSetVertexValue(m_base_shape, 0, color_index.data(), r.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_base_shape, 1, color_index.data(), g.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_base_shape, 2, color_index.data(), b.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_base_shape, 3, color_index.data(), a.data(), num_color_vertex))

    hasVertexColor = true;
  }
  else
  {
    hasVertexColor = false;
  }
}

} //namespace anari::rpr
