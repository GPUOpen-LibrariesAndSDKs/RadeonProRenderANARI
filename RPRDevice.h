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

// rpr
#include "RadeonProRender.h"
// anari
#include "anari/backend/utilities/IntrusivePtr.h"
#include "anari/backend/DeviceImpl.h"
#include "anari/backend/utilities/ParameterizedObject.h"
#include "interface.h"

namespace anari::rpr {

struct Object;

struct RPR_DEVICE_INTERFACE RPRDevice : public DeviceImpl, public ParameterizedObject, public RefCounted
{
  /////////////////////////////////////////////////////////////////////////////
  // Main interface to accepting API calls
  /////////////////////////////////////////////////////////////////////////////

  // Data Arrays //////////////////////////////////////////////////////////////

  ANARIArray1D newArray1D(const void *appMemory,
      ANARIMemoryDeleter deleter,
      const void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t byteStride1) override;

  ANARIArray2D newArray2D(const void *appMemory,
      ANARIMemoryDeleter deleter,
      const void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t numItems2,
      uint64_t byteStride1,
      uint64_t byteStride2) override;

  ANARIArray3D newArray3D(const void *appMemory,
      ANARIMemoryDeleter deleter,
      const void *userdata,
      ANARIDataType,
      uint64_t numItems1,
      uint64_t numItems2,
      uint64_t numItems3,
      uint64_t byteStride1,
      uint64_t byteStride2,
      uint64_t byteStride3) override;

  void *mapArray(ANARIArray) override;
  void  unmapArray(ANARIArray) override;

  // Renderable Objects ///////////////////////////////////////////////////////

  ANARILight newLight(const char *type) override;

  ANARICamera newCamera(const char *type) override;

  ANARIGeometry     newGeometry(const char *type) override;
  ANARISpatialField newSpatialField(const char *type) override;

  ANARISurface newSurface() override;
  ANARIVolume  newVolume(const char *type) override;

  // Surface Meta-Data ////////////////////////////////////////////////////////

  ANARIMaterial newMaterial(const char *material_type) override;

  ANARISampler newSampler(const char *type) override;

  // Instancing ///////////////////////////////////////////////////////////////

  ANARIGroup newGroup() override;

  ANARIInstance newInstance() override;

  // Top-level Worlds /////////////////////////////////////////////////////////

  ANARIWorld newWorld() override;

  // Object + Parameter Lifetime Management ///////////////////////////////////

  int getProperty(ANARIObject object, const char *name, ANARIDataType type, void *mem, uint64_t size,
      uint32_t mask) override;

  void setParameter(ANARIObject object, const char *name, ANARIDataType type, const void *mem) override;

  void unsetParameter(ANARIObject object, const char *name) override;

  void commitParameters(ANARIObject object) override;

  void release(ANARIObject _obj) override;

  void retain(ANARIObject _obj) override;

  // FrameBuffer Manipulation /////////////////////////////////////////////////

  ANARIFrame newFrame() override;

  const void *frameBufferMap(ANARIFrame fb,
      const char *channel,
      uint32_t *width,
      uint32_t *height,
      ANARIDataType *pixelType) override;

  void frameBufferUnmap(ANARIFrame fb, const char *channel) override;

  // Frame Rendering //////////////////////////////////////////////////////////

  ANARIRenderer newRenderer(const char *type) override;

  void renderFrame(ANARIFrame) override;
  int  frameReady(ANARIFrame, ANARIWaitMask) override;
  void discardFrame(ANARIFrame) override;

  /////////////////////////////////////////////////////////////////////////////
  // Helper/other functions and data members
  /////////////////////////////////////////////////////////////////////////////

  RPRDevice();

  ~RPRDevice() override;

  void flushCommitBuffer();

 private:
  rpr_context         m_context{};
  rpr_material_system m_matsys{};
  std::string         m_plugin_name   = "Northstar";
  unsigned int        m_render_device = 1; // render with GPU0 by default

  std::vector<Object *> m_objectsToCommit;
  bool                  m_needToSortCommits{false};

  void deviceSetParameter(const char *id, ANARIDataType type, const void *mem);
  void deviceUnsetParameter(const char *id);
  void deviceCommit();
};

} // namespace anari::rpr
