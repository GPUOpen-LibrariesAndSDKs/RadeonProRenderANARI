// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../Object.h"
#include "../rpr_common.h"
#include "../RPRDevice.h"
#include "../renderer/Renderer.h"
#include "../camera/Camera.h"
#include "../scene/World.h"
#include "Future.h"
// std
#include <memory>
#include <vector>

namespace anari {
namespace rpr {

struct Frame : public Object
{
  friend struct RPRDevice;

  Frame(rpr_context &context);

  ivec2 size() const;
  ANARIFrameFormat format() const;

  void *mapColorBuffer();

  void clear();

  void invokeContinuation(ANARIDevice d) const;

  void commit() override;

  void addToScene(rpr_scene scene) override;

  void prepScene(rpr_context &context);

  Future &future();

  ~Frame();

 private:
  int frameID() const;
  void newFrame();

  // Data //
  ANARIFrameFormat m_format{ANARI_FB_NONE};

  ivec2 m_size;

  int m_frameID{-1};
  float m_invFrameID{1.f};

  std::vector<vec4> m_accum;
  std::vector<unsigned char> m_mappedPixelBuffer;
  rpr_framebuffer m_fb;
  rpr_framebuffer m_fb_resolved;
  rpr_context m_context;

  IntrusivePtr<Renderer> m_renderer;
  IntrusivePtr<Camera> m_camera;
  IntrusivePtr<World> m_world;

  std::unique_ptr<Future> m_future;
  ANARIRenderContinuation m_continuation{nullptr};
  void *m_continuationPtr{nullptr};
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Frame *, ANARI_FRAME);

} // namespace anari
