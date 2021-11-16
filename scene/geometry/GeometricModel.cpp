// Copyright 2020 The Khronos GeometricModel
// SPDX-License-Identifier: Apache-2.0

#include "GeometricModel.h"
#include "Geometry.h"

namespace anari {
namespace rpr {

void GeometricModel::commit()
{
  auto* material = getParamObject<Material>("material");
  auto* geometry = getParamObject<Geometry>("geometry");
  auto singleColor = getParam<vec4>("color", vec4(-1));
  auto* multipleColor = getParamObject<Data>("color", nullptr);

  m_shapes = geometry->m_shapes;
  m_upper_bound = geometry->m_upper_bound;
  m_lower_bound = geometry->m_lower_bound;

  //instance colors
  if(singleColor.r>=0 && singleColor.g>=0 && singleColor.b>=0 && singleColor.a>=0 || multipleColor) {
    std::vector<rpr_float> colorVector{};
    if(multipleColor){
      DataView<vec4> color(multipleColor);
      for(int i=0; i<color.size(); i++){
        colorVector.push_back(color.data()[i].r);
        colorVector.push_back(color.data()[i].g);
        colorVector.push_back(color.data()[i].b);
        colorVector.push_back(color.data()[i].a);
      }
    }
    else{
      for(int i=0; i<m_shapes.size(); i++){
        colorVector.push_back(singleColor.r);
        colorVector.push_back(singleColor.g);
        colorVector.push_back(singleColor.b);
        colorVector.push_back(singleColor.a);
      }
    }
    clearMaterialNodeInstance();

    rpr_buffer_desc bufferDesc;
    bufferDesc.element_channel_size = 4;
    bufferDesc.nb_element = colorVector.size() / 4;
    bufferDesc.element_type = RPR_BUFFER_ELEMENT_TYPE_FLOAT32;

    CHECK(rprContextCreateBuffer(m_context, &bufferDesc, colorVector.data(), &m_color_buffer))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &m_id_lookup))
    CHECK(rprMaterialNodeSetInputUByKey(m_id_lookup, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_OBJECT_ID))
    CHECK(rprMaterialSystemCreateNode(material->m_matsys, RPR_MATERIAL_NODE_BUFFER_SAMPLER, &m_color_sampler))
    CHECK(rprMaterialNodeSetInputNByKey(m_color_sampler, RPR_MATERIAL_INPUT_UV, m_id_lookup))
    CHECK(rprMaterialNodeSetInputBufferDataByKey(m_color_sampler, RPR_MATERIAL_INPUT_DATA, m_color_buffer))

    m_material = material->generateMaterial(m_color_sampler);
  }

  //vertex colors
  else if(geometry->hasVertexColor){
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

void GeometricModel::clearMaterialNodesVertex(){
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

void GeometricModel::clearMaterialNodeInstance(){
  if(m_color_buffer){
    CHECK(rprObjectDelete(m_color_buffer))
    m_color_buffer = nullptr;
  }
  if(m_color_sampler){
    CHECK(rprObjectDelete(m_color_sampler))
    m_color_sampler = nullptr;
  }
  if(m_id_lookup){
    CHECK(rprObjectDelete(m_id_lookup))
    m_id_lookup = nullptr;
  }
}

GeometricModel::~GeometricModel(){
  clearMaterialNodesVertex();
  clearMaterialNodeInstance();
  CHECK(rprObjectDelete(m_material))
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::GeometricModel *);
} // namespace anari

