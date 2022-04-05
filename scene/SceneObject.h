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

#include "../Array.h"

namespace anari::rpr {

struct SceneObject : public Object
{
  bool getProperty(const std::string &name, ANARIDataType type, void *ptr, ANARIWaitMask m) override;

  box3 bounds();

 protected:
  void extendBounds(box3 bounds);
  void resetBounds();

  box3 m_bounds{};
};

// Inline SceneObject definitions

inline box3 SceneObject::bounds()
{
  return m_bounds;
}

inline void SceneObject::extendBounds(box3 bounds)
{
  m_bounds.extend(bounds);
}

inline void SceneObject::resetBounds()
{
  m_bounds = {vec3(std::numeric_limits<float>::max()), -vec3(std::numeric_limits<float>::max())};
}

inline bool SceneObject::getProperty(
    const std::string &name, ANARIDataType type, void *ptr, ANARIWaitMask mask)
{
  if (name == "bounds" && type == ANARI_FLOAT32_BOX3)
  {
    std::memcpy(ptr, &m_bounds, sizeof(m_bounds));
    return true;
  }

  return Object::getProperty(name, type, ptr, mask);
}

} // namespace anari::rpr
