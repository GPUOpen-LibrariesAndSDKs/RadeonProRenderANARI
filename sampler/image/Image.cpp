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

#include "Image.h"

namespace anari::rpr {

const static std::map<std::string, rpr_image_filter_type> image_filter_types = {
    {"nearest", RPR_IMAGE_FILTER_TYPE_NEAREST}, {"linear", RPR_IMAGE_FILTER_TYPE_LINEAR}};

const static std::map<std::string, rpr_image_wrap_type> image_wrap_types = {
    {"clampToEdge", RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE}, {"repeat", RPR_IMAGE_WRAP_TYPE_REPEAT},
    {"mirrorRepeat", RPR_IMAGE_WRAP_TYPE_MIRRORED_REPEAT}};

Image::Image(rpr_context context, rpr_material_system matsys) : Sampler(context, matsys) {}

void Image::commit()
{
  clearInstances();
  if (m_image)
  {
    CHECK(rprObjectDelete(m_image))
    m_image = nullptr;
  }

  m_input_attribute  = getParam<std::string>("inAttribute", "attribute0");
  m_input_transform  = getParam<mat4x4>("inTransform", mat4(1));
  m_output_transform = getParam<mat4x4>("outTransform", mat4(1));
  m_filter           = processFilter(getParam<std::string>("filter", "nearest"));
  // TODO in and out transforms
}

void Image::clearInstances()
{
  for (rpr_material_node materialNode : m_instances)
  {
    CHECK(rprObjectDelete(materialNode))
  }
  m_instances.clear();

  for (TransformNode *transformNode : m_transform_nodes)
  {
    delete transformNode;
  }
  m_transform_nodes.clear();
}

rpr_material_node Image::generateMaterial(Geometry *geometry)
{
  if (!geometry->hasAttribute(m_input_attribute.c_str()))
  {
    throw std::runtime_error("attribute is not supported by geometry");
  }
  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &material))
  CHECK(rprMaterialNodeSetInputImageDataByKey(material, RPR_MATERIAL_INPUT_DATA, m_image))
  rpr_material_node transformed_input =
      applyTransformNode(m_input_transform, geometry->getAttribute(m_input_attribute.c_str())->getMaterial());
  CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UV, transformed_input))

  m_instances.push_back(material);
  return material;
}

rpr_material_node Image::applyTransformNode(mat4 transform, rpr_material_node input)
{
  if (transform == mat4(1)) // skip default transform
  {
    return input;
  }
  auto *transformNode = new TransformNode(m_matsys, transform, input);
  m_transform_nodes.push_back(transformNode);
  return transformNode->getMaterial();
}

rpr_image_filter_type Image::processFilter(const std::string &name)
{
  if (image_filter_types.find(name) != image_filter_types.end())
  {
    return image_filter_types.find(name)->second;
  }
  throw std::runtime_error("unknown filter type");
}

rpr_image_wrap_type Image::processWrap(const std::string &name)
{
  if (image_wrap_types.find(name) != image_wrap_types.end())
  {
    return image_wrap_types.find(name)->second;
  }
  throw std::runtime_error("unknown wrap type");
}

Image::~Image()
{
  clearInstances();
  if (m_image)
  {
    CHECK(rprObjectDelete(m_image))
  }
}
} // namespace anari::rpr
