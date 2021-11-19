// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#include "Surface.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

void Surface::commit()
{
  auto material = getParamObject<Material>("material");
  auto geometry = getParamObject<Geometry>("geometry");

  if(!geometry) throw std::runtime_error("'geometry' is a required attribute");
  if(!material) throw std::runtime_error("'material' is a required attribute");

  m_shapes = geometry->m_shapes;
  m_upper_bound = geometry->m_upper_bound;
  m_lower_bound = geometry->m_lower_bound;

  //vertex colors
  if(geometry->hasVertexColor){
    clearMaterialNodesVertex();
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_r))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_g))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_b))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_a))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_r))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_g))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_b))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_a))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_rg))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_rgb))

    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_lookup_r, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_VERTEX_VALUE0))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_lookup_g, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_VERTEX_VALUE1))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_lookup_b, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_VERTEX_VALUE2))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_lookup_a, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_VERTEX_VALUE3))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_r, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_lookup_r))
    CHECK(rprMaterialNodeSetInputFByKey(m_vertex_color_r, RPR_MATERIAL_INPUT_COLOR1, 1,0,0,0))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_r, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_g, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_lookup_g))
    CHECK(rprMaterialNodeSetInputFByKey(m_vertex_color_g, RPR_MATERIAL_INPUT_COLOR1, 0,1,0,0))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_g, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_b, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_lookup_b))
    CHECK(rprMaterialNodeSetInputFByKey(m_vertex_color_b, RPR_MATERIAL_INPUT_COLOR1, 0,0,1,0))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_b, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_a, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_lookup_a))
    CHECK(rprMaterialNodeSetInputFByKey(m_vertex_color_a, RPR_MATERIAL_INPUT_COLOR1, 0,0,0,1))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_a, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_rg, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_r))
    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_rg, RPR_MATERIAL_INPUT_COLOR1, m_vertex_color_g))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_rg, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_rgb, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_rg))
    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color_rgb, RPR_MATERIAL_INPUT_COLOR1, m_vertex_color_b))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color_rgb, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))

    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color, RPR_MATERIAL_INPUT_COLOR0, m_vertex_color_rgb))
    CHECK(rprMaterialNodeSetInputNByKey(m_vertex_color, RPR_MATERIAL_INPUT_COLOR1, m_vertex_color_a))
    CHECK(rprMaterialNodeSetInputUByKey(m_vertex_color, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD))

    m_material = material->generateMaterial(m_vertex_color);
  }
  else{
    m_material = material->generateMaterial();
  }

  for(rpr_shape shape : m_shapes){
    CHECK(rprShapeSetMaterial(shape, m_material))
  }
}

void Surface::clearMaterialNodesVertex(){
  if(m_vertex_color){
    CHECK(rprObjectDelete(m_vertex_color))
    m_vertex_color = nullptr;
  }
  if(m_vertex_color_lookup_r){
    CHECK(rprObjectDelete(m_vertex_color_lookup_r))
    m_vertex_color_lookup_r = nullptr;
  }
  if(m_vertex_color_lookup_g){
    CHECK(rprObjectDelete(m_vertex_color_lookup_g))
    m_vertex_color_lookup_g = nullptr;
  }
  if(m_vertex_color_lookup_b){
    CHECK(rprObjectDelete(m_vertex_color_lookup_b))
    m_vertex_color_lookup_b = nullptr;
  }
  if(m_vertex_color_lookup_a){
    CHECK(rprObjectDelete(m_vertex_color_lookup_a))
    m_vertex_color_lookup_a = nullptr;
  }
  if(m_vertex_color_r){
    CHECK(rprObjectDelete(m_vertex_color_r))
    m_vertex_color_lookup_r = nullptr;
  }
  if(m_vertex_color_g){
    CHECK(rprObjectDelete(m_vertex_color_g))
    m_vertex_color_lookup_g = nullptr;
  }
  if(m_vertex_color_b){
    CHECK(rprObjectDelete(m_vertex_color_b))
    m_vertex_color_lookup_b = nullptr;
  }
  if(m_vertex_color_a){
    CHECK(rprObjectDelete(m_vertex_color_a))
    m_vertex_color_lookup_a = nullptr;
  }
  if(m_vertex_color_rg){
    CHECK(rprObjectDelete(m_vertex_color_rg))
    m_vertex_color_lookup_a = nullptr;
  }
  if(m_vertex_color_rgb){
    CHECK(rprObjectDelete(m_vertex_color_rgb))
    m_vertex_color_lookup_a = nullptr;
  }
}

Surface::~Surface(){
  clearMaterialNodesVertex();
  CHECK(rprObjectDelete(m_material))
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Surface *);
} // namespace anari

