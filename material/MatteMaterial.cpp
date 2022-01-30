#include "MatteMaterial.h"

namespace anari::rpr{

MatteMaterial::MatteMaterial(rpr_material_system materialSystem){
    m_matsys = materialSystem;
}

void MatteMaterial::commit() {
    m_diffuse_color_attribute = getParam<std::string>("color", "");
    m_opacity_attribute = getParam<std::string>("opacity", "");
    m_diffuse_color = getParam<vec3>("color", vec3(0.8f));
    m_opacity = getParam<rpr_float>("opacity", 1.f);
}

rpr_material_node MatteMaterial::generateMaterial(Geometry *geometry){

  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material))

  // color
  if(!m_diffuse_color_attribute.empty() && geometry->hasAttribute(m_diffuse_color_attribute.c_str()))
  {
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, geometry->getAttribute(m_diffuse_color_attribute.c_str())->getMaterial()))
  }
  else{
    CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, m_diffuse_color.x, m_diffuse_color.y, m_diffuse_color.z, 1.f))
  }
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.f, 1.f, 1.f, 1.f))

  // opacity
  if(!m_opacity_attribute.empty() && geometry->hasAttribute(m_opacity_attribute.c_str()))
  {
    if(!m_transparency_node)
    {
      CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_transparency_node))
    }
    CHECK(rprMaterialNodeSetInputFByKey(m_transparency_node, RPR_MATERIAL_INPUT_COLOR0, 1,1,1,1))
    CHECK(rprMaterialNodeSetInputNByKey(m_transparency_node, RPR_MATERIAL_INPUT_COLOR1, geometry->getAttribute(m_opacity_attribute.c_str())->getMaterial()))
    CHECK(rprMaterialNodeSetInputUByKey(m_transparency_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB))
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, m_transparency_node))
  }
  float transparency = 1.f - m_opacity;
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY,transparency, transparency, transparency, 1.f))

  return material;
}

MatteMaterial::~MatteMaterial()
{
  if(m_transparency_node)
  {
    CHECK(rprObjectDelete(m_transparency_node))
    m_transparency_node = nullptr;
  }
}

}
