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

#include "PrimitiveSampler.h"

namespace anari::rpr {

PrimitiveSampler::PrimitiveSampler(rpr_context context, rpr_material_system matsys) : Sampler(context, matsys)
{}

void PrimitiveSampler::commit()
{
  delete m_attribute;

  auto *data = getParamObject<Array1D>("array");
  if (!data)
  {
    throw std::runtime_error("'array' is a required parameter");
  }
  std::vector<vec4> processedData;
  processedData.reserve(data->size());

  for (int indexNumber = 0; indexNumber < data->size(); indexNumber++)
  {
    processedData.push_back(Attribute::processColor(data, indexNumber));
  }
  m_attribute =
      new PerShapeAttribute(m_context, m_matsys, processedData.size(), (float *)processedData.data());
}

rpr_material_node PrimitiveSampler::generateMaterial(Geometry *geometry)
{
  return m_attribute->getMaterial();
}

PrimitiveSampler::~PrimitiveSampler()
{
  delete m_attribute;
}

} // namespace anari::rpr