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

#pragma once

#include "../Object.h"
#include "../rpr_common.h"
#include "../RPRDevice.h"

namespace anari {
namespace rpr {

struct Renderer : public Object
{
  explicit Renderer(rpr_context context);
  static Renderer *createInstance(rpr_context context, const char *type);
  ~Renderer() override;

  void commit() override{};
  void attachToScene(rpr_scene scene);

 protected:
  rpr_context m_context;
  rpr_image   m_background_image = nullptr;
  int         m_pixel_samples    = 1;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Renderer *, ANARI_RENDERER);

} // namespace anari
