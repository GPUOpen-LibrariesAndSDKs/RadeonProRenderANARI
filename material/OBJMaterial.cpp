// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "OBJMaterial.h"

namespace anari::rpr{

OBJMaterial::OBJMaterial(rpr_material_system materialSystem){
  m_matsys = materialSystem;
}

void OBJMaterial::commit(){
  float SHININESS_MAX_VALUE = 10000; //defined in anari specification
  /**
   * all default values of obj material are defined in ANARI specification
   */
  m_diffuse_color = getParam<vec3>("kd", vec3(0.8f));
  m_specular_color = getParam<vec3>("ks", vec3(0.f));
  m_transparency_color = getParam<vec3>("tf", vec3(0.f));
  m_roughness = std::pow(1.f - ((getParam<rpr_float>("ns", 10.f)) / SHININESS_MAX_VALUE), 2);
  m_transparency = 1.f - getParam<rpr_float>("d", 1.f);

}

rpr_material_node OBJMaterial::generateMaterial(rpr_material_node vertex_color){

  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material))

  if(vertex_color){
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, vertex_color))
  }
  else{
    CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, m_diffuse_color.x, m_diffuse_color.y, m_diffuse_color.z, 1.f))
  }
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.f, 1.f, 1.f, 1.f))

  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, m_specular_color.x, m_specular_color.y, m_specular_color.z, 1.f))
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, m_roughness, m_roughness, m_roughness, m_roughness))
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.f, 1.f, 1.f, 1.f))

  if(vertex_color){
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, vertex_color))
  }
  else{
    CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR,
                                        m_transparency_color.x,
                                        m_transparency_color.y,
                                        m_transparency_color.z, 1))
  }
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, m_transparency, m_transparency, m_transparency, m_transparency))
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 1))

  return material;
}


}