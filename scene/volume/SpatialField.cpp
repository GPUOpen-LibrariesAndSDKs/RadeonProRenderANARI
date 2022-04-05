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

#include "SpatialField.h"
#include "StucturedRegularSpatialField.h"

namespace anari {
namespace rpr {

const static std::map<std::string, rpr_image_filter_type> image_filter_types = {
    {"nearest", RPR_IMAGE_FILTER_TYPE_NEAREST}, {"linear", RPR_IMAGE_FILTER_TYPE_LINEAR}};

SpatialField *SpatialField::createInstance(const char *type)
{
  if (strcmp(type, "structuredRegular") == 0)
  {
    return new StructuredRegularSpatialField();
  }

  throw std::runtime_error("could not create spatial field");
}

rpr_image_filter_type SpatialField::processFilter(const std::string &name)
{
  if (image_filter_types.find(name) != image_filter_types.end())
  {
    return image_filter_types.find(name)->second;
  }
  throw std::runtime_error("unknown filter type");
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::SpatialField *);

} // namespace anari
