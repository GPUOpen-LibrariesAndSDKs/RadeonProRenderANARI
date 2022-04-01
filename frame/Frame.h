/**********************************************************************
Copyright 2021 The Khronos Group
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

#include "../camera/Camera.h"
#include "../renderer/Renderer.h"
#include "../rpr_common.h"
#include "../scene/World.h"
#include "Future.h"
// std
#include <memory>
#include <vector>

namespace anari {
namespace rpr {

struct Frame : public Object
{
  explicit Frame(rpr_context context);
  ~Frame() override;

  bool getProperty(
      const std::string &name, ANARIDataType type, void *ptr, ANARIWaitMask mask) override;

  void   commit() override;
  void  *mapColorBuffer();
  float *mapDepthBuffer();

  void renderFrame();
  void invokeContinuation(ANARIDevice device) const;

  void      renewFuture();
  bool      futureIsValid() const;
  FuturePtr future();

  void  setDuration(float);
  float duration() const;

 private:
  void deleteFramebuffers();
  void clearFramebuffers();

  // Data //
  ANARIDataType m_format{ANARI_UNKNOWN};

  uvec2 m_size{};

  float m_lastFrameDuration{0.f};

  std::vector<vec4>          m_accum;
  std::vector<unsigned char> m_mappedPixelBuffer;

  std::vector<float> m_depthBuffer;

  IntrusivePtr<Renderer> m_renderer;
  IntrusivePtr<Camera>   m_camera;
  IntrusivePtr<World>    m_world;

  FuturePtr                    m_future;
  ANARIFrameCompletionCallback m_continuation{nullptr};
  void                        *m_continuationPtr{nullptr};

  bool      m_frameChanged{true};
  TimeStamp m_cameraLastChanged{0};
  TimeStamp m_rendererLastChanged{0};
  TimeStamp m_worldLastChanged{0};

  rpr_context     m_context;
  rpr_scene       m_scene{};
  rpr_framebuffer m_framebuffer{};
  rpr_framebuffer m_framebuffer_resolved{};
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Frame *, ANARI_FRAME)

} // namespace anari
