// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#include "Surface.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

Surface::Surface(rpr_material_system materialSystem) : m_matsys(materialSystem){
    setCommitPriority(COMMIT_PRIORITY_SURFACE);
}

void Surface::commit()
{
  IntrusivePtr<Material> material = getParamObject<Material>("material");
  m_geometry = getParamObject<Geometry>("geometry");

  if(!m_geometry) throw std::runtime_error("'geometry' is a required attribute");
  if(!material) throw std::runtime_error("'material' is a required attribute");

  // cleanup before new commit
  if(m_material_instance)
  {
    CHECK(rprObjectDelete(m_material_instance));
    m_material_instance = nullptr;
  }

  m_bounds = m_geometry->bounds();

  // prepare material node for vertex color
  // TODO move vertex color node to another class
  if(m_geometry->hasVertexColor && false) // skip vertex colors because now Northstar does not support it
  {
    if(!m_vertex_color)
    {
      generateVertexColorNode();
    }
    // generate new material instance
    m_material_instance = material->generateMaterial(m_vertex_color);
  }
  else
  {
    // generate new material instance
    m_material_instance = material->generateMaterial(nullptr);
  }

  markUpdated();
}

void Surface::addToScene(rpr_scene scene) {

    clearInstances();
    getInstances(m_instances);

    // attach instances to scene
    for(rpr_shape instance : m_instances){
      CHECK(rprSceneAttachShape(scene, instance));
    }
    // attach base invisible shape
    CHECK(rprSceneAttachShape(scene, m_geometry->getBaseShape()))
}

void Surface::getInstances(std::set<rpr_shape> &out_shapes)
{
  std::set<rpr_shape> instances;
  m_geometry->getInstances(instances);

  for(rpr_shape instance: instances)
  {
    CHECK(rprShapeSetMaterial(instance, m_material_instance))
    out_shapes.insert(instance);
  }
}

rpr_shape Surface::getBaseShape()
{
  return m_geometry->getBaseShape();
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

void Surface::clearInstances()
{
  for(rpr_shape instance : m_instances)
  {
    CHECK(rprObjectDelete(instance))
  }
  m_instances.clear();
}

Surface::~Surface(){
  clearInstances();
  if(m_material_instance){
    CHECK(rprObjectDelete(m_material_instance));
  }
  clearMaterialNodesVertex();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Surface *);
} // namespace anari

