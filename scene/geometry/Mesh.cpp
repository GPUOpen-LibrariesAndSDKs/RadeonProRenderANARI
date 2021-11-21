// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "Mesh.h"

namespace anari::rpr {

Mesh::Mesh(rpr_context &context) : Geometry(context) {
  m_shapes.resize(1);
}

void Mesh::commit()
{
  if (!hasParam("primitive.index") || !hasParam("vertex.position"))
    throw std::runtime_error("missing 'mesh' data arrays!");

  auto index = getParamObject<Array1D>("primitive.index");
  auto vertex = getParamObject<Array1D>("vertex.position");
  auto color = getParamObject<Array1D>("vertex.color");

  if (!index || !vertex)
    throw std::runtime_error("'mesh' data array are incorrect type!");

  std::size_t num_faces = index->size();
  std::vector<rpr_int> faces(num_faces, 3);

  //bounds calculation
  resetBounds();

  for(int i=0; i<vertex->size()*3; i++){
    float coordinate = vertex->dataAs<rpr_float>()[i];
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

  CHECK(rprContextCreateMesh(m_context, vertex->dataAs<rpr_float>(), vertex->size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, index->dataAs<rpr_int>(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &(m_shapes[0])))
  CHECK(rprShapeSetObjectID(m_shapes[0], 0))

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
    CHECK(rprShapeSetVertexValue(m_shapes[0], 0, color_index.data(), r.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 1, color_index.data(), g.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 2, color_index.data(), b.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 3, color_index.data(), a.data(), num_color_vertex))

    hasVertexColor = true;

    markUpdated();
  }

}

Mesh::~Mesh(){
  CHECK(rprObjectDelete(m_shapes[0]))
}


} // namespace anari
