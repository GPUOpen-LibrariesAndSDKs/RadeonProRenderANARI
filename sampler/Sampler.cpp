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

#include "Sampler.h"
#include "image/Image1D.h"
#include "image/Image2D.h"
#include "image/Image3D.h"
#include "PrimitiveSampler.h"
#include "TransformSampler.h"

namespace anari {
namespace rpr {

Sampler::Sampler(rpr_context context, rpr_material_system matsys) : m_context(context), m_matsys(matsys) {}

Sampler *Sampler::createInstance(const char *type, rpr_context context, rpr_material_system matsys)
{
  if (strcmp(type, "image1D") == 0)
  {
    return new Image1D(context, matsys);
  }
  if (strcmp(type, "image2D") == 0)
  {
    return new Image2D(context, matsys);
  }
  if (strcmp(type, "image3D") == 0)
  {
    return new Image3D(context, matsys);
  }
  if (strcmp(type, "transform") == 0)
  {
    return new TransformSampler(context, matsys);
  }
  if (strcmp(type, "primitive") == 0)
  {
    return new PrimitiveSampler(context, matsys);
  }
  throw std::runtime_error("cannot create sampler");
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Sampler *);

} // namespace anari
