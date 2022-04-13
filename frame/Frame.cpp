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
  return (uint32_t)(255.f * glm::clamp(f, 0.f, 1.f));
}

static uint32_t cvt_uint32(const vec4 &v)
{
  return (cvt_uint32(v.x) << 0) | (cvt_uint32(v.y) << 8) | (cvt_uint32(v.z) << 16)
      | (cvt_uint32(v.w) << 24);
}

static uint32_t cvt_uint32_srgb(const vec4 &v)
{
  return cvt_uint32(vec4(pow(v.x, 1.f / 2.2f), pow(v.y, 1.f / 2.2f), pow(v.z, 1.f / 2.2f), v.w));
}

// Frame definitions //////////////////////////////////////////////////////////

Frame::Frame(rpr_context context) : m_context(context)
{
  CHECK(rprContextCreateScene(m_context, &m_scene))
  setCommitPriority(COMMIT_PRIORITY_FRAME);
}

Frame::~Frame()
{
  if (futureIsValid())
    m_future->wait();

  deleteFramebuffers();
  CHECK(rprObjectDelete(m_scene))
  m_scene = nullptr;
}

void Frame::commit()
{
  deleteFramebuffers();

  m_renderer = getParamObject<Renderer>("renderer");
  m_camera   = getParamObject<Camera>("camera");
  m_world    = getParamObject<World>("world");

  m_format = getParam<ANARIDataType>("color", ANARI_UFIXED8_RGBA_SRGB);
  m_size   = getParam<uvec2>("size", ivec2(10));

  m_continuation    = getParam<ANARIFrameCompletionCallback>("frameCompletionCallback", nullptr);
  m_continuationPtr = getParam<void *>("frameCompletionCallbackUserData", nullptr);

  auto numPixels = m_size.x * m_size.y;

  m_accum.resize(numPixels);
  m_mappedPixelBuffer.resize(numPixels * sizeOf(m_format));
  m_depthBuffer.resize(numPixels);

  // create framebuffer desc
  rpr_framebuffer_desc desc;
  desc.fb_width              = m_size.x;
  desc.fb_height             = m_size.y;
  rpr_framebuffer_format fmt = {4, RPR_COMPONENT_TYPE_FLOAT32};
  CHECK(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_framebuffer))
  CHECK(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_framebuffer_resolved))

  m_frameChanged        = true;
  m_cameraLastChanged   = 0;
  m_rendererLastChanged = 0;
  m_worldLastChanged    = 0;
}

bool Frame::getProperty(const std::string &name, ANARIDataType type, void *ptr, ANARIWaitMask mask)
{
  if (name == "duration" && type == ANARI_FLOAT32 && futureIsValid())
  {
    if (mask & ANARI_WAIT)
      m_future->wait();
    float d = duration();
    std::memcpy(ptr, &d, sizeof(d));
    return true;
  }

  return Object::getProperty(name, type, ptr, mask);
}

void *Frame::mapColorBuffer()
{
  CHECK(rprContextResolveFrameBuffer(m_context, m_framebuffer, m_framebuffer_resolved, true))

  CHECK(rprFrameBufferGetInfo(m_framebuffer_resolved, RPR_FRAMEBUFFER_DATA,
      m_size.x * m_size.y * 4 * 4, m_accum.data(), nullptr))

  switch (m_format)
  {
  case ANARI_UFIXED8_VEC4: {
    std::transform(m_accum.begin(), m_accum.end(), (uint32_t *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return cvt_uint32(in); });
    break;
  }
  case ANARI_UFIXED8_RGBA_SRGB: {
    std::transform(m_accum.begin(), m_accum.end(), (uint32_t *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return cvt_uint32_srgb(in); });
    break;
  }
  case ANARI_FLOAT32_VEC4: {
    std::transform(m_accum.begin(), m_accum.end(), (vec4 *)m_mappedPixelBuffer.data(),
        [&](const vec4 &in) { return in; });
    break;
  }
  default: {
    throw std::runtime_error("cannot map ANARI_UNKNOWN format");
  }
  }

  return m_mappedPixelBuffer.data();
}

float *Frame::mapDepthBuffer()
{
  // TODO depth framebuffer
  return m_depthBuffer.data();
}

void Frame::clearFramebuffers()
{
  CHECK(rprFrameBufferClear(m_framebuffer))
}

void Frame::deleteFramebuffers()
{
  if (m_framebuffer)
  {
    CHECK(rprObjectDelete(m_framebuffer))
    m_framebuffer = nullptr;
  }
  if (m_framebuffer_resolved)
  {
    CHECK(rprObjectDelete(m_framebuffer_resolved))
    m_framebuffer_resolved = nullptr;
  }
}

void Frame::renderFrame()
{
  Renderer &r = *m_renderer;
  Camera   &c = *m_camera;
  World    &w = *m_world;

  bool rendererChanged = r.lastUpdated() > m_rendererLastChanged;
  bool cameraChanged   = c.lastUpdated() > m_cameraLastChanged;
  bool worldChanged    = w.lastUpdated() > m_worldLastChanged;

  if (rendererChanged)
  {
    r.attachToScene(m_scene);
    m_rendererLastChanged = r.lastUpdated();
  }
  if (cameraChanged)
  {
    c.attachToScene(m_scene);
    m_cameraLastChanged = c.lastUpdated();
  }
  if (worldChanged)
  {
    w.attachToScene(m_scene);
    m_worldLastChanged = w.lastUpdated();
  }
  if (m_frameChanged)
  {
    CHECK(rprContextSetAOV(m_context, RPR_AOV_COLOR, m_framebuffer))
    CHECK(rprContextSetScene(m_context, m_scene))
    m_frameChanged = false;
  }

  if (cameraChanged | rendererChanged | worldChanged | m_frameChanged)
  {
    clearFramebuffers();
  }

  CHECK(rprContextRender(m_context))
}

void Frame::invokeContinuation(ANARIDevice device) const
{
  if (m_continuation)
    m_continuation(m_continuationPtr, device, (ANARIFrame)this);
}

void Frame::renewFuture()
{
  m_future = std::make_shared<Future>();
}

bool Frame::futureIsValid() const
{
  return m_future.get() != nullptr;
}

FuturePtr Frame::future()
{
  return m_future;
}

void Frame::setDuration(float d)
{
  m_lastFrameDuration = d;
}

float Frame::duration() const
{
  return m_lastFrameDuration;
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Frame *);

} // namespace anari
