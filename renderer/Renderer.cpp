// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.h"
#include "PathTracerRenderer.h"
// specific types

namespace anari {
namespace rpr {

Renderer::Renderer(rpr_context context) : m_context(context){}

Renderer *Renderer::createInstance(rpr_context context, const char *type)
{
  return new PathTracerRenderer(context);
}


void Renderer::addToScene(rpr_scene scene)
{
    CHECK(rprSceneSetBackgroundImage(scene, m_background_image))
    CHECK(rprContextSetParameterByKey1u(m_context, RPR_CONTEXT_ITERATIONS, m_pixel_samples))
}

Renderer::~Renderer(){
  if(m_background_image){
    CHECK(rprObjectDelete(m_background_image))
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Renderer *);

} // namespace anari
