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

#include "Renderer.h"
#include "PathTracerRenderer.h"
// specific types

namespace anari {
namespace rpr {

Renderer::Renderer(rpr_context context) : m_context(context) {}

Renderer *Renderer::createInstance(rpr_context context, const char *type)
{
  return new PathTracerRenderer(context);
}

void Renderer::attachToScene(rpr_scene scene)
{
  CHECK(rprSceneSetBackgroundImage(scene, m_background_image))
  CHECK(rprContextSetParameterByKey1u(m_context, RPR_CONTEXT_ITERATIONS, m_pixel_samples))
}

Renderer::~Renderer()
{
  if (m_background_image)
  {
    CHECK(rprObjectDelete(m_background_image))
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Renderer *);

} // namespace anari
