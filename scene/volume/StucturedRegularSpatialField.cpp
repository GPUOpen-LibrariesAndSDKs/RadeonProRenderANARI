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

#include "StucturedRegularSpatialField.h"

namespace anari::rpr {

void StructuredRegularSpatialField::commit()
{
  auto *data   = getParamObject<Array3D>("data");
  m_position   = getParam<vec3>("origin", vec3(0));
  m_voxelSize  = getParam<vec3>("spacing", vec3(1));
  m_filterType = processFilter(getParam<std::string>("filter", "linear"));

  if (!data)
    throw std::runtime_error("'data' parameter is required");

  m_size = data->size();
  resetBounds();
  extendBounds(box3(m_position, m_position + (vec3(m_size) - 1.f) * m_voxelSize));

  m_grid.clear();
  m_indices.clear();
  m_grid.reserve(m_size.x * m_size.y * m_size.z);
  m_indices.reserve(m_size.x * m_size.y * m_size.z);

  for (uint32_t x = 0; x < m_size.x; x++)
  {
    for (uint32_t y = 0; y < m_size.y; y++)
    {
      for (uint32_t z = 0; z < m_size.z; z++)
      {
        processValue(uvec3(x, y, z), data);
      }
    }
  }
}

void StructuredRegularSpatialField::processValue(uvec3 position, Array3D *data)
{
  m_indices.push_back(position.x);
  m_indices.push_back(position.y);
  m_indices.push_back(position.z);
  size_t element_number = position.x * m_size.y * m_size.z + position.y * m_size.z + position.z;
  switch (data->elementType())
  {
  case ANARI_UINT8:
    m_grid.push_back((rpr_float)data->dataAs<uint8>()[element_number]);
    break;
  case ANARI_INT16:
    m_grid.push_back((rpr_float)data->dataAs<int16>()[element_number]);
    break;
  case ANARI_UINT16:
    m_grid.push_back((rpr_float)data->dataAs<uint16>()[element_number]);
    break;
  case ANARI_FLOAT32:
    m_grid.push_back((rpr_float)data->dataAs<float32>()[element_number]);
    break;
  case ANARI_FLOAT64:
    m_grid.push_back((rpr_float)data->dataAs<float64>()[element_number]);
    break;
  }
}

} // namespace anari::rpr
