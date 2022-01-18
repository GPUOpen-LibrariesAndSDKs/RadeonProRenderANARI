// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "Mesh.h"

namespace anari::rpr {

Mesh::Mesh(rpr_context &context) : Geometry(context) {}

void Mesh::commit()
{
  auto index = getParamObject<Array1D>("primitive.index");
  auto vertex = getParamObject<Array1D>("vertex.position");
  auto color = getParamObject<Array1D>("vertex.color");

  if (!vertex)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

  //bounds calculation
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

  if(index)
  {
    std::size_t num_faces = index->size();
    std::vector<rpr_int> faces(num_faces, 3);

    CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertex->dataAs<vec3>(), vertex->size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int*) index->dataAs<uvec3>(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_shape))
  }
  else  // 'index' array is missed. We should use default indices ((0,1,2), (3,4,5), ...)
  {
    std::vector<uvec3> default_index;
    for (uint32_t i = 0; i < vertex->size(); i += 3)
    {
      default_index.push_back(uvec3(0, 1, 2) + i);
    }

    std::size_t num_faces = default_index.size();
    std::vector<rpr_int> faces(num_faces, 3);

    CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertex->dataAs<vec3>(), vertex->size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int*) default_index.data(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_shape))
  }


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

  CHECK(rprShapeSetVisibility(m_base_shape, false))  // base shape is always invisible
}

void Mesh::addToScene(rpr_scene scene) {
  rpr_shape instance;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
  CHECK(rprSceneAttachShape(scene, instance))
  CHECK(rprSceneAttachShape(scene, m_base_shape))
}

void Mesh::getInstances(std::vector<rpr_shape> &out_shapes)
{
  rpr_shape instance;
  CHECK(rprContextCreateInstance(m_context, m_base_shape, &instance))
  out_shapes.push_back(instance);
}

Mesh::~Mesh()
{
  if(m_base_shape){
    CHECK(rprObjectDelete(m_base_shape))
  }
}


} // namespace anari