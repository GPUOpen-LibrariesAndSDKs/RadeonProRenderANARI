#include "Image3D.h"

namespace anari::rpr{

Image3D::Image3D(rpr_context context, rpr_material_system matsys) : Image(context, matsys) {}

void Image3D::commit()
{
  Image::commit();
  m_wrap_mode_u = processWrap(getParam("wrapMode1", "clampToEdge"));
  m_wrap_mode_v = processWrap(getParam("wrapMode2", "clampToEdge"));
  m_wrap_mode_w = processWrap(getParam("wrapMode3", "clampToEdge"));
  auto image_data = getParamObject<Array3D>("image");

  if(!image_data){
    throw std::runtime_error("'image' is a required parameter");
  }

  rpr_image_desc imageDesc;
  imageDesc.image_width = image_data->size(0);
  imageDesc.image_height = image_data->size(1);
  imageDesc.image_depth = image_data->size(2);
  imageDesc.image_row_pitch = image_data->size(0) * 3 * sizeof(rpr_float);
  imageDesc.image_slice_pitch = image_data->size(0) * image_data->size(1) * 3 * sizeof(rpr_float);
  CHECK(rprContextCreateImage(m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &imageDesc, image_data->dataAs<vec3>(), &m_image))
  CHECK(rprImageSetFilter(m_image, m_filter))
}

rpr_material_node Image3D::generateMaterial(Geometry *geometry)
{
  rpr_material_node material = Image::generateMaterial(geometry);
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_U,m_wrap_mode_u))
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_V,m_wrap_mode_v))
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_W,m_wrap_mode_w))
  return material;
}

}
