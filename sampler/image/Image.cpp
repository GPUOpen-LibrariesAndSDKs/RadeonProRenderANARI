#include "Image.h"

namespace anari::rpr{

const static std::map<std::string, rpr_image_filter_type> image_filter_types = {
    {"nearest", RPR_IMAGE_FILTER_TYPE_NEAREST},
    {"linear", RPR_IMAGE_FILTER_TYPE_LINEAR}};

const static std::map<std::string, rpr_image_wrap_type> image_wrap_types = {
    {"clampToEdge", RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE},
    {"repeat", RPR_IMAGE_WRAP_TYPE_REPEAT},
    {"mirrorRepeat", RPR_IMAGE_WRAP_TYPE_MIRRORED_REPEAT}};

Image::Image(rpr_context context, rpr_material_system matsys) : Sampler(context, matsys){}

void Image::commit()
{
  clearInstances();
  if(m_image)
  {
    CHECK(rprObjectDelete(m_image))
    m_image = nullptr;
  }

  m_input_attribute = getParam<std::string>("inAttribute", "attribute0");
  m_filter = processFilter(getParam<std::string>("filter", "nearest"));
  // TODO in and out transforms
}

rpr_material_node Image::generateMaterial(Geometry *geometry)
{
  if(!geometry->hasAttribute(m_input_attribute.c_str())){
    throw std::runtime_error("attribute is not supported by geometry");
  }
  rpr_material_node material;
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &material))
  CHECK(rprMaterialNodeSetInputImageDataByKey(material, RPR_MATERIAL_INPUT_DATA, m_image))
  CHECK(rprMaterialNodeSetInputNByKey(material, RPR_MATERIAL_INPUT_UV, geometry->getAttribute(m_input_attribute.c_str())->getMaterial()))

  m_instances.push_back(material);
  return material;
}

rpr_image_filter_type Image::processFilter(const std::string& name)
{
  if(image_filter_types.find(name) != image_filter_types.end()){
    return image_filter_types.find(name)->second;
  }
  throw std::runtime_error("unknown filter type");
}

rpr_image_wrap_type Image::processWrap(const std::string& name)
{
  if(image_wrap_types.find(name) != image_wrap_types.end()){
    return image_wrap_types.find(name)->second;
  }
  throw std::runtime_error("unknown wrap type");
}

Image::~Image()
{
  if(m_image)
  {
    CHECK(rprObjectDelete(m_image))
  }
}
}
