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

#include "../Object.h"
// std
#include <chrono>
#include <future>
#include <memory>

namespace anari::rpr {

struct Future
{
  Future();
  ~Future() = default;

  void wait() const;
  bool isReady() const;
  void markComplete();

 private:
  std::promise<void> m_promise;
  std::future<void>  m_future;
};

using FuturePtr = std::shared_ptr<Future>;

// Inlined definitions ////////////////////////////////////////////////////////

inline Future::Future()
{
  m_future = m_promise.get_future();
}

inline void Future::wait() const
{
  if (m_future.valid())
    m_future.wait();
}

inline bool Future::isReady() const
{
  return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

inline void Future::markComplete()
{
  m_promise.set_value();
}

} // namespace anari::rpr