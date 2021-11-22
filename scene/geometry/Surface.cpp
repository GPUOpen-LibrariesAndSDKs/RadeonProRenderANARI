// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#include "Surface.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

void Surface::commit()
{
  m_material = getParamObject<Material>("material");
  m_geometry = getParamObject<Geometry>("geometry");

  if(!m_geometry) throw std::runtime_error("'geometry' is a required attribute");
  if(!m_material) throw std::runtime_error("'material' is a required attribute");

  m_bounds = m_geometry->bounds();
  markUpdated();
}

void Surface::addToScene(rpr_scene scene) {
    if(m_material_instance){
        CHECK(rprObjectDelete(m_material_instance));
    }
    if(m_geometry->hasVertexColor && !m_vertex_color){
        generateVertexColorNode();
    } else if(!m_geometry->hasVertexColor){
        m_vertex_color = nullptr;
    }
    m_material_instance = m_material->generateMaterial(m_vertex_color);
    m_geometry->applyMaterial(m_material_instance);
    m_geometry->addToScene(scene);
}

void Surface::generateVertexColorNode() {
    clearMaterialNodesVertex();
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_r))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_g))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_b))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_vertex_color_lookup_a))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_r))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_g))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_b))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_a))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_rg))
    CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_vertex_color_rgb))

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
  if(m_material_instance){
      CHECK(rprObjectDelete(m_material_instance));
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Surface *);
} // namespace anari

