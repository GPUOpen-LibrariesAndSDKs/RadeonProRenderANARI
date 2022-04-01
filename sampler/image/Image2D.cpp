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

#include "Image2D.h"

namespace anari::rpr {

Image2D::Image2D(rpr_context context, rpr_material_system matsys) : Image(context, matsys) {}

void Image2D::commit()
{
  Image::commit();
  m_wrap_mode_u   = processWrap(getParam("wrapMode1", "clampToEdge"));
  m_wrap_mode_v   = processWrap(getParam("wrapMode2", "clampToEdge"));
  auto image_data = getParamObject<Array2D>("image");

  if (!image_data)
  {
    throw std::runtime_error("'image' is a required parameter");
  }

  rpr_image_desc imageDesc;
  imageDesc.image_width       = image_data->size(0);
  imageDesc.image_height      = image_data->size(1);
  imageDesc.image_depth       = 0;
  imageDesc.image_row_pitch   = image_data->size(0) * 3 * sizeof(rpr_float);
  imageDesc.image_slice_pitch = 0;
  CHECK(rprContextCreateImage(m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &imageDesc,
      (float32 *)image_data->dataAs<vec3>(), &m_image))
  CHECK(rprImageSetFilter(m_image, m_filter))
}

rpr_material_node Image2D::generateMaterial(Geometry *geometry)
{
  rpr_material_node material = Image::generateMaterial(geometry);
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_U, m_wrap_mode_u))
  CHECK(rprMaterialNodeSetInputUByKey(material, RPR_MATERIAL_INPUT_WRAP_V, m_wrap_mode_v))
  return applyTransformNode(m_output_transform, material);
}

} // namespace anari::rpr
