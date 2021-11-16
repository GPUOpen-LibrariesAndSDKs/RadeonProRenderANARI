// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "PathTracerRenderer.h"
#include "../Data.h"

namespace anari {
namespace rpr {

PathTracerRenderer::PathTracerRenderer(rpr_context context) : Renderer(context){};

void PathTracerRenderer::commit(){

  float bgColor[4] = {0, 0, 0, 0};
  auto bgColorData3 = getParam<vec3>("backgroundColor", vec3(-1));
  auto bgColorData4 = getParam<vec4>("backgroundColor", vec4(-1));

  if(bgColorData3.r>=0 && bgColorData3.g>=0 && bgColorData3.b>=0){  //detection of missed parameter and protection from negative values
    bgColor[0] = bgColorData3.r;
    bgColor[1] = bgColorData3.g;
    bgColor[2] = bgColorData3.b;
    bgColor[3] = 1.f;
  }

  else if(bgColorData4.r>=0 && bgColorData4.g>=0 && bgColorData4.b>=0 && bgColorData4.a>=0){
    bgColor[0] = bgColorData4.r;
    bgColor[1] = bgColorData4.g;
    bgColor[2] = bgColorData4.b;
    bgColor[3] = bgColorData4.a;
  }

  m_pixel_samples = getParam<int>("pixelSamples", 1);

  if(m_background_image != nullptr){
    CHECK(rprObjectDelete(m_background_image))
    m_background_image = nullptr;
  }
  rpr_image_format format = {4, RPR_COMPONENT_TYPE_FLOAT32};
  rpr_image_desc desc = {1, 1, 0, 3 * sizeof(rpr_float), 0};
  CHECK(rprContextCreateImage(m_context, format, &desc, bgColor, &m_background_image))
  is_changed = true;
}

}
}
