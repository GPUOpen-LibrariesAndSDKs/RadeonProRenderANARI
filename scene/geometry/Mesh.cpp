// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "Mesh.h"

namespace anari {
namespace rpr {

Mesh::Mesh(rpr_context &context) : Geometry(context) {
  m_shapes.resize(1);
}

void Mesh::commit()
{
  if (!hasParam("index") || !hasParam("vertex.position"))
    throw std::runtime_error("missing 'mesh' data arrays!");

  DataView<uvec3> index(getParamObject<Data>("index"));
  DataView<vec3> vertex(getParamObject<Data>("vertex.position"));
  DataView<vec4> color(getParamObject<Data>("vertex.color"));

  if (!index || !vertex)
    throw std::runtime_error("'mesh' data array are incorrect type!");

  std::size_t num_faces = index.size();
  std::vector<rpr_int> faces(num_faces, 3);

  //bounds calculation
  m_lower_bound = vec3(std::numeric_limits<float>::max());
  m_upper_bound = vec3(-std::numeric_limits<float>::max());
  for(int i=0; i<vertex.size()*3; i++){
    float coordinate = ((rpr_float *) vertex.data())[i];
    if(i%3==0){  //x coordinate
      m_upper_bound.x = max(m_upper_bound.x, coordinate);
      m_lower_bound.x = min(m_lower_bound.x, coordinate);
    }
    if(i%3==1){  //y coordinate
      m_upper_bound.y = max(m_upper_bound.y, coordinate);
      m_lower_bound.y = min(m_lower_bound.y, coordinate);
    }
    if(i%3==2){  //z coordinate
      m_upper_bound.z = max(m_upper_bound.z, coordinate);
      m_lower_bound.z = min(m_lower_bound.z, coordinate);
    }
  }

  CHECK(rprContextCreateMesh(m_context, (rpr_float *) vertex.data(), vertex.size(), sizeof(rpr_float) * 3, NULL, 0, 0, NULL, 0, 0, (rpr_int * ) index.data(), sizeof(rpr_int), NULL, 0, NULL, 0, faces.data(), num_faces, &(m_shapes[0])))
  CHECK(rprShapeSetObjectID(m_shapes[0], 0))

  if(color){
    rpr_int num_color_vertex = color.size();
    std::vector<rpr_float> x;
    std::vector<rpr_float> y;
    std::vector<rpr_float> z;
    std::vector<rpr_float> w;
    std::vector<rpr_int> color_index;
    for(int i=0; i<num_color_vertex; i++){
      x.push_back(color.data()[i].x);
      y.push_back(color.data()[i].y);
      z.push_back(color.data()[i].z);
      w.push_back(color.data()[i].w);
      color_index.push_back(i);
    }
    CHECK(rprShapeSetVertexValue(m_shapes[0], 0, color_index.data(), x.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 1, color_index.data(), y.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 2, color_index.data(), z.data(), num_color_vertex))
    CHECK(rprShapeSetVertexValue(m_shapes[0], 3, color_index.data(), w.data(), num_color_vertex))

    hasVertexColor = true;
  }

}

Mesh::~Mesh(){
  CHECK(rprObjectDelete(m_shapes[0]))
}


} // namespace rpr
} // namespace anari
