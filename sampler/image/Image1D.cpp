#include "Image1D.h"

namespace anari::rpr{

Image1D::Image1D(rpr_context context, rpr_material_system matsys) : Image(context, matsys) {}

void Image1D::commit()
{
  Image::commit();
  m_wrap_mode = processWrap(getParam("wrapMode", "clampToEdge"));
  auto image_data = getParamObject<Array1D>("image");

  if(!image_data){
    throw std::runtime_error("'image' is a required parameter");
  }

  rpr_image_desc imageDesc;
  imageDesc.image_width = image_data->size();
  imageDesc.image_height = 1;
  imageDesc.image_depth = 0;
  imageDesc.image_row_pitch = image_data->size() * 3 * sizeof(rpr_float);
  imageDesc.image_slice_pitch = 0;
  CHECK(rprContextCreateImage(m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &imageDesc, image_data->dataAs<vec3>(), &m_image))
}

rpr_material_node Image1D::generateMaterial(Geometry *geometry)
{
  rpr_material_node material = Image::generateMaterial(geometry);
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_U, m_wrap_mode))
  return material;
}

}
