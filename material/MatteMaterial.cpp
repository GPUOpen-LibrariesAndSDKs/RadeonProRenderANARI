/**********************************************************************
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

#include "MatteMaterial.h"

namespace anari::rpr {

MatteMaterial::MatteMaterial(rpr_material_system materialSystem)
{
  m_matsys = materialSystem;
}

void MatteMaterial::commit()
{
  // STRING values
  m_diffuse_color_attribute = getParam<std::string>("color", "");
  m_opacity_attribute       = getParam<std::string>("opacity", "");

  // FLOAT_VEC3 values
  m_diffuse_color = getParam<vec3>("color", vec3(0.8f));
  m_opacity       = getParam<rpr_float>("opacity", 1.f);

  // Sampler values
  m_diffuse_color_sampler = getParamObject<Sampler>("color");
  m_opacity_sampler       = getParamObject<Sampler>("opacity");
}

rpr_material_node MatteMaterial::generateMaterial(Geometry *geometry)
{
  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material))

  // color
  if (m_diffuse_color_sampler) // color is a sampler
  {
    CHECK(rprMaterialNodeSetInputNByKey(
        material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, m_diffuse_color_sampler->generateMaterial(geometry)))
  } else if (!m_diffuse_color_attribute.empty()
      && geometry->hasAttribute(m_diffuse_color_attribute.c_str())) // color is a geometry attribute
  {
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR,
        geometry->getAttribute(m_diffuse_color_attribute.c_str())->getMaterial()))
  } else
  {
    CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, m_diffuse_color.x,
        m_diffuse_color.y, m_diffuse_color.z, 1.f))
  }
  CHECK(rprMaterialNodeSetInputFByKey(material, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.f, 1.f, 1.f, 1.f))

  // opacity
  if ((!m_opacity_attribute.empty() && geometry->hasAttribute(m_opacity_attribute.c_str()))
      || m_opacity_sampler) // opacity is a material node
  {
    rpr_material_node opacityNode = m_opacity_sampler
        ? m_opacity_sampler->generateMaterial(geometry)
        : geometry->getAttribute(m_opacity_attribute.c_str())->getMaterial();
    if (!m_transparency_node)
    {
      CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &m_transparency_node))
    }
    CHECK(rprMaterialNodeSetInputFByKey(m_transparency_node, RPR_MATERIAL_INPUT_COLOR0, 1, 1, 1, 1))
    CHECK(rprMaterialNodeSetInputNByKey(m_transparency_node, RPR_MATERIAL_INPUT_COLOR1, opacityNode))
    CHECK(rprMaterialNodeSetInputUByKey(m_transparency_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB))
    CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, m_transparency_node))
  } else // opacity is a float value
  {
    float transparency = 1.f - m_opacity;
    CHECK(rprMaterialNodeSetInputFByKey(
        material, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, transparency, transparency, transparency, 1.f))
  }
  return material;
}

MatteMaterial::~MatteMaterial()
{
  if (m_transparency_node)
  {
    CHECK(rprObjectDelete(m_transparency_node))
    m_transparency_node = nullptr;
  }
}

} // namespace anari::rpr
