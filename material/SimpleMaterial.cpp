#include "SimpleMaterial.h"

namespace anari::rpr{

SimpleMaterial::SimpleMaterial(rpr_material_system materialSystem){
    m_matsys = materialSystem;
}

void SimpleMaterial::commit() {
    m_diffuse_color = getParam<vec3>("color", vec3(0.8f));
    m_transparency = 1.f - getParam<rpr_float>("opacity", 1.f);
    markUpdated();
}

rpr_material_node SimpleMaterial::generateMaterial(rpr_material_node vertex_color){

  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material))

  if(vertex_color){
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, vertex_color))
  }
  else{
    CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, m_diffuse_color.x, m_diffuse_color.y, m_diffuse_color.z, 1.f))
  }
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.f, 1.f, 1.f, 1.f))
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, m_transparency, m_transparency, m_transparency, 1.f))

  return material;
}

}
