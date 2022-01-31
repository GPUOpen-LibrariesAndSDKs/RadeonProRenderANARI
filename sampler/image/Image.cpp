#include "Image.h"

namespace anari::rpr{

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

rpr_int Image::processFilter(const std::string& name)
{
  if(strcmp(name.c_str(), "nearest") == 0){
    return RPR_IMAGE_FILTER_TYPE_NEAREST;
  }
  if(strcmp(name.c_str(), "linear") == 0){
    return RPR_IMAGE_FILTER_TYPE_LINEAR;
  }
  throw std::runtime_error("unknown filter type");
}

rpr_int Image::processWrap(const std::string& name)
{
  if(strcmp(name.c_str(), "clampToEdge") == 0){
    return RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE;
  }
  if(strcmp(name.c_str(), "repeat") == 0){
    return RPR_IMAGE_WRAP_TYPE_REPEAT;
  }
  if(strcmp(name.c_str(), "mirrorRepeat") == 0){
    return RPR_IMAGE_WRAP_TYPE_MIRRORED_REPEAT;
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
