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

#include "Object.h"

namespace anari {
namespace rpr {

std::atomic<TimeStamp> g_timeStamp = []() { return 0; }();

TimeStamp newTimeStamp()
{
  return g_timeStamp++;
}

void Object::commit()
{
  // no-op
}

bool Object::getProperty(const std::string &, ANARIDataType, void *, ANARIWaitMask)
{
  return false;
}

void Object::setObjectType(ANARIDataType type)
{
  m_type = type;
}

ANARIDataType Object::type() const
{
  return m_type;
}

TimeStamp Object::lastUpdated() const
{
  return m_lastUpdated;
}

void Object::markUpdated()
{
  m_lastUpdated = newTimeStamp();
}

int Object::commitPriority() const
{
  return m_commitPriority;
}

void Object::setCommitPriority(int p)
{
  m_commitPriority = p;
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Object *);

} // namespace anari
