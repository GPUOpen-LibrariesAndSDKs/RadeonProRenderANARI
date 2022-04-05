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

#include "ScivisVolume.h"

namespace anari::rpr {

ScivisVolume::ScivisVolume(rpr_context context, rpr_material_system matsys) : Volume(context, matsys) {}

void ScivisVolume::commit()
{
  clear();
  Volume::commit();

  auto *field = getParamObject<SpatialField>("field");
  if (!field)
    throw std::runtime_error("'field' is a required parameter");

  auto *color = getParamObject<Array1D>("color");
  if (!color)
    throw std::runtime_error("'color' is a required parameter");

  auto *opacity = getParamObject<Array1D>("opacity");
  if (!opacity)
    throw std::runtime_error("'opacity' is a required parameter");

  auto valueRange   = getParam<vec2>("valueRange", vec2(0, 1));
  auto densityScale = getParam<float32>("densityScale", 1.f);

  extendBounds(field->bounds());

  // normalize values of grid and calculate density grid values
  std::vector<float32> normalizedGridData;
  std::vector<float32> densityGridData;
  normalizedGridData.reserve(field->m_grid.size());
  densityGridData.reserve(field->m_grid.size());
  float32 normalizedValue;
  for (float32 voxelValue : field->m_grid)
  {
    if (voxelValue > valueRange.y)
    {
      normalizedValue = valueRange.y;
    } else if (voxelValue < valueRange.x)
    {
      normalizedValue = valueRange.x;
    } else
    {
      normalizedValue = (voxelValue - valueRange.x) / (valueRange.y - valueRange.x);
    }
    normalizedGridData.push_back(normalizedValue);
    densityGridData.push_back(processDensity(normalizedValue, opacity, field->m_filterType));
  }

  CHECK(rprContextCreateGrid(m_context, &m_color_grid, field->m_size.x, field->m_size.y, field->m_size.z,
      (rpr_int *)field->m_indices.data(), field->m_indices.size() / 3, RPR_GRID_INDICES_TOPOLOGY_XYZ_U32,
      normalizedGridData.data(), sizeof(normalizedGridData[0]) * normalizedGridData.size(), 0))
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_GRID_SAMPLER, &m_color_grid_material))
  CHECK(rprMaterialNodeSetInputGridDataByKey(m_color_grid_material, RPR_MATERIAL_INPUT_DATA, m_color_grid))

  CHECK(rprContextCreateGrid(m_context, &m_density_grid, field->m_size.x, field->m_size.y, field->m_size.z,
      (rpr_int *)field->m_indices.data(), field->m_indices.size() / 3, RPR_GRID_INDICES_TOPOLOGY_XYZ_U32,
      densityGridData.data(), sizeof(densityGridData[0]) * densityGridData.size(), 0))
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_GRID_SAMPLER, &m_density_grid_material))
  CHECK(
      rprMaterialNodeSetInputGridDataByKey(m_density_grid_material, RPR_MATERIAL_INPUT_DATA, m_density_grid))

  rpr_image_desc imageDesc;
  imageDesc.image_width       = color->size();
  imageDesc.image_height      = 1;
  imageDesc.image_depth       = 0;
  imageDesc.image_row_pitch   = color->size() * 3 * sizeof(rpr_float);
  imageDesc.image_slice_pitch = 0;
  CHECK(rprContextCreateImage(
      m_context, {3, RPR_COMPONENT_TYPE_FLOAT32}, &imageDesc, (float32 *)color->dataAs<vec3>(), &m_color_map))
  CHECK(rprImageSetFilter(m_color_map, field->m_filterType))
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &m_color_material));
  CHECK(rprMaterialNodeSetInputImageDataByKey(m_color_material, RPR_MATERIAL_INPUT_DATA, m_color_map))
  CHECK(rprMaterialNodeSetInputNByKey(m_color_material, RPR_MATERIAL_INPUT_UV, m_color_grid_material))
  CHECK(rprMaterialNodeSetInputUByKey(
      m_color_material, RPR_MATERIAL_INPUT_WRAP_U, RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE))
  CHECK(rprMaterialNodeSetInputUByKey(
      m_color_material, RPR_MATERIAL_INPUT_WRAP_V, RPR_IMAGE_WRAP_TYPE_CLAMP_TO_EDGE))

  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_VOLUME, &m_volume_material))
  CHECK(rprMaterialNodeSetInputNByKey(
      m_volume_material, RPR_MATERIAL_INPUT_DENSITYGRID, m_density_grid_material))
  CHECK(rprMaterialNodeSetInputFByKey(
      m_volume_material, RPR_MATERIAL_INPUT_DENSITY, densityScale, densityScale, densityScale, 1))
  CHECK(rprMaterialNodeSetInputNByKey(m_volume_material, RPR_MATERIAL_INPUT_COLOR, m_color_material))

  // calculate transform
  mat4 volumeTranslation = translate(mat4(1), (field->bounds().lower + field->bounds().upper) * 0.5f);
  // rotations and minus in scale are for compensation of different coordinate systems
  mat4 volumeScale    = scale(mat4(1), -(field->bounds().upper - field->bounds().lower));
  mat4 volumeRotation = rotate(mat4(1), radians(90.f), vec3(0, 1, 0));
  volumeRotation      = rotate(volumeRotation, radians(180.f), vec3(1, 0, 0));
  m_transform         = volumeScale * volumeRotation * volumeTranslation;
}

float32 ScivisVolume::processDensity(float32 index, Array1D *densityLookup, rpr_image_filter_type filter)
{
  if (densityLookup->size() == 1) // only one value in lookup table
  {
    return densityLookup->dataAs<float32>()[0];
  }
  if (index >= 1) // last value of lookup table
  {
    return densityLookup->dataAs<float32>()[densityLookup->size() - 1];
  }
  if (filter == RPR_IMAGE_FILTER_TYPE_NEAREST)
  {
    auto lookupIdx = (size_t)round(index * (densityLookup->size() - 1));
    return densityLookup->dataAs<float32>()[lookupIdx];
  }
  // linear interpolation
  size_t lookupIdx   = floor(index * (densityLookup->size() - 1));
  float  firstValue  = densityLookup->dataAs<float32>()[lookupIdx];
  float  secondValue = densityLookup->dataAs<float32>()[lookupIdx + 1];
  return firstValue + (secondValue - firstValue) * (index * (densityLookup->size() - 1) - lookupIdx);
}

mat4 ScivisVolume::getVolumeTransform()
{
  return m_transform;
}

void ScivisVolume::clear()
{
  if (m_color_grid_material)
  {
    CHECK(rprObjectDelete(m_color_grid_material))
  }
  if (m_density_grid_material)
  {
    CHECK(rprObjectDelete(m_density_grid_material))
  }
  if (m_color_material)
  {
    CHECK(rprObjectDelete(m_color_material))
  }
  if (m_color_map)
  {
    CHECK(rprObjectDelete(m_color_map))
  }
  if (m_color_grid)
  {
    CHECK(rprObjectDelete(m_color_grid))
  }
  if (m_density_grid)
  {
    CHECK(rprObjectDelete(m_density_grid))
  }
}

ScivisVolume::~ScivisVolume()
{
  clear();
}

} // namespace anari::rpr
