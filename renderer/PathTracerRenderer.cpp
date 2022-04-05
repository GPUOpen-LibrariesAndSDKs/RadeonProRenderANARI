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

#include "PathTracerRenderer.h"

namespace anari::rpr {

PathTracerRenderer::PathTracerRenderer(rpr_context context) : Renderer(context){}

void PathTracerRenderer::commit()
{
  float bgColor[4]   = {0, 0, 0, 0};
  auto  bgColorData3 = getParam<vec3>("backgroundColor", vec3(-1));
  auto  bgColorData4 = getParam<vec4>("backgroundColor", vec4(-1));

  if (bgColorData3.r >= 0 && bgColorData3.g >= 0 && bgColorData3.b >= 0)
  { // detection of missed parameter and protection from negative values
    bgColor[0] = bgColorData3.r;
    bgColor[1] = bgColorData3.g;
    bgColor[2] = bgColorData3.b;
    bgColor[3] = 1.f;
  }

  else if (bgColorData4.r >= 0 && bgColorData4.g >= 0 && bgColorData4.b >= 0 && bgColorData4.a >= 0)
  {
    bgColor[0] = bgColorData4.r;
    bgColor[1] = bgColorData4.g;
    bgColor[2] = bgColorData4.b;
    bgColor[3] = bgColorData4.a;
  }

  m_pixel_samples = getParam<int>("pixelSamples", 1);

  if (m_background_image != nullptr)
  {
    CHECK(rprObjectDelete(m_background_image))
    m_background_image = nullptr;
  }
  rpr_image_format format = {4, RPR_COMPONENT_TYPE_FLOAT32};
  rpr_image_desc   desc   = {1, 1, 0, 3 * sizeof(rpr_float), 0};
  CHECK(rprContextCreateImage(m_context, format, &desc, bgColor, &m_background_image))
}

} // namespace anari::rpr
