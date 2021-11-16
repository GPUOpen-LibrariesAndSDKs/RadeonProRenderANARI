// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Frame.h"
// std
#include <algorithm>
#include <random>
#include <thread>

namespace anari {
namespace rpr {

// Helper functions ///////////////////////////////////////////////////////////

static uint32_t cvt_uint32(const float &f)
{
  return (uint32_t)(255.f * clamp(f, 0.f, 1.f));
}

static uint32_t cvt_uint32(const vec4 &v)
{
  return (cvt_uint32(v.x) << 0) | (cvt_uint32(v.y) << 8)
      | (cvt_uint32(v.z) << 16) | (cvt_uint32(v.w) << 24);
}

static uint32_t cvt_uint32_srgb(const vec4 &v)
{
  return cvt_uint32(vec4(
      pow(v.x, 1.f / 2.2f), pow(v.y, 1.f / 2.2f), pow(v.z, 1.f / 2.2f), v.w));
}

// Frame definitions //////////////////////////////////////////////////////////

Frame::Frame(rpr_context &context) : m_context(context)
{
  m_future = std::unique_ptr<Future>(new Future);
}

void Frame::commit()
{
  m_renderer = getParamObject<Renderer>("renderer");
  m_camera = getParamObject<Camera>("camera");
  m_world = getParamObject<World>("world");

  m_format = getParam<ANARIFrameFormat>("format", ANARI_FB_NONE);
  m_size.x = getParam<int>("width", 0);
  m_size.y = getParam<int>("height", 0);

  m_continuation = getParam<ANARIRenderContinuation>("continuation", nullptr);
  m_continuationPtr = getParam<void *>("continuationPtr", nullptr);

  auto numPixels = m_size.x * m_size.y;

  m_accum.resize(numPixels);
  m_mappedPixelBuffer.resize(numPixels * sizeOfFbFormat(m_format));

  // create framebuffer desc
  rpr_framebuffer_desc desc;
  desc.fb_width = m_size.x;
  desc.fb_height = m_size.y;
  rpr_framebuffer_format fmt = {4, RPR_COMPONENT_TYPE_FLOAT32};
  CHECK(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_fb));
  CHECK(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_fb_resolved));
  is_changed = true;
}

ivec2 Frame::size() const
{
  return m_size;
}

ANARIFrameFormat Frame::format() const
{
  return m_format;
}

void Frame::clear()
{
  CHECK(rprFrameBufferClear(m_fb));
}

void Frame::invokeContinuation(ANARIDevice d) const
{
  if (m_continuation) {
    this->refInc();
    std::thread([=]() {
      m_continuation(d, (ANARIFrame)this, m_continuationPtr);
      this->refDec();
    }).detach();
  }
}

void *Frame::mapColorBuffer()
{
  CHECK(rprContextResolveFrameBuffer(m_context, m_fb, m_fb_resolved, true))

  CHECK(rprFrameBufferGetInfo(m_fb_resolved,
      RPR_FRAMEBUFFER_DATA,
      m_size.x * m_size.y * 4 * 4,
      m_accum.data(),
      NULL))

  switch (format()) {
  case ANARI_FB_RGBA8: {
    std::transform(m_accum.begin(),
        m_accum.end(),
        (uint32_t *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return cvt_uint32(in * m_invFrameID); });
    break;
  }
  case ANARI_FB_SRGBA: {
    std::transform(m_accum.begin(),
        m_accum.end(),
        (uint32_t *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return cvt_uint32_srgb(in * m_invFrameID); });
    break;
  }
  case ANARI_FB_RGBA32F:
  default: {
    std::transform(m_accum.begin(),
        m_accum.end(),
        (vec4 *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return in * m_invFrameID; });
    break;
  }
  }

  return m_mappedPixelBuffer.data();
}

int Frame::frameID() const
{
  return m_frameID;
}

void Frame::newFrame()
{
  m_frameID++;
  m_invFrameID = 1.f / (m_frameID + 1);
}

void Frame::addToScene(rpr_scene scene)
{
  if (is_changed) {
    CHECK(rprContextSetAOV(m_context, RPR_AOV_COLOR, m_fb))
    is_changed = false;
  }
}

void Frame::prepScene(rpr_context &context)
{
  rpr_scene scene;
  CHECK(rprContextGetScene(context, &scene))

  m_camera->addToScene(scene);
  addToScene(scene);
  m_renderer->addToScene(scene);
  m_world->addToScene(scene);
}

Future &Frame::future()
{
  return *m_future;
}

Frame::~Frame()
{
  CHECK(rprObjectDelete(m_fb))
  CHECK(rprObjectDelete(m_fb_resolved))
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Frame *);

} // namespace anari
