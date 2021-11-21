// Copyright 2020 The Khronos Triangles
// SPDX-License-Identifier: Apache-2.0

#include "AreaLight.h"

namespace anari::rpr {

AreaLight::AreaLight(rpr_context &context, rpr_material_system& matsys)  : Light(context){
  m_matsys = matsys;
}

void AreaLight::commit(){
  if(m_light){
    CHECK(rprObjectDelete(m_light))
    m_light = nullptr;
  }
  if(m_material){
    CHECK(rprObjectDelete(m_material))
    m_material = nullptr;
  }

  if(!hasParam("position") || !hasParam("edge1") || !hasParam("edge2")){
    throw std::runtime_error("missing mandatory parameters");
  }

  auto position = getParam<vec3>("position", vec3());
  auto edge1 = getParam<vec3>("edge1", vec3());
  auto edge2 = getParam<vec3>("edge2", vec3());
  auto intensity = getParam<float>("intensity", 1.f);
  vec3 color = getParam<vec3>("color", vec3(1.f)) * intensity;
  bool visible = getParam<bool>("visible", true);

  std::vector<vec3> vertex;
  vertex.push_back(position);
  vertex.push_back(position + edge1);
  vertex.push_back(position + edge2);
  vertex.push_back(position + edge1 + edge2);

  rpr_int index[] = {0, 1, 3, 2};

  std::vector<rpr_int> faces(1, 4);

  CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertex.data(), 4, sizeof(rpr_float)*3, nullptr, 0, 0, nullptr, 0, 0, index, sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), 1, &m_light))

  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_EMISSIVE, &m_material))
  CHECK(rprMaterialNodeSetInputFByKey(m_material, RPR_MATERIAL_INPUT_COLOR, color.r, color.g, color.b, 1.f))

  CHECK(rprShapeSetMaterial(m_light, m_material))

  CHECK(rprShapeSetVisibility(m_light, visible))

  markUpdated();
}

void AreaLight::addToScene(rpr_scene scene){
  CHECK(rprSceneAttachShape(scene, m_light))
  markAttached();
}

AreaLight::~AreaLight(){
  if(m_material){
    CHECK(rprObjectDelete(m_material))
    m_material = nullptr;
  }
}


} // namespace anari
