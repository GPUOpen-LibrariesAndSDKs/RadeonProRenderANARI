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

#include <glm/ext.hpp>
#include <glm/glm.hpp>
// std
#include <limits>
#include <optional>
#include <random>

namespace anari::rpr {

using namespace glm;

static float randUniformDist()
{
  static thread_local std::minstd_rand         generator;
  static std::uniform_real_distribution<float> distribution{0.f, 1.f};
  return distribution(generator);
}

static vec3 randomDir()
{
  static thread_local std::minstd_rand         gen;
  static std::uniform_real_distribution<float> dist{-1.f, 1.f};
  vec3                                         dir(dist(gen), dist(gen), dist(gen));
  return normalize(dir);
}

// AABB type //////////////////////////////////////////////////////////////////

template <typename T>
struct range_t
{
  range_t() = default;
  range_t(const T &t) : lower(t), upper(t) {}
  range_t(const T &_lower, const T &_upper) : lower(_lower), upper(_upper) {}

  T size() const
  {
    return upper - lower;
  }

  T center() const
  {
    return .5f * (lower + upper);
  }

  void extend(const T &t)
  {
    lower = min(lower, t);
    upper = max(upper, t);
  }

  void extend(const range_t<T> &t)
  {
    lower = min(lower, t.lower);
    upper = max(upper, t.upper);
  }

  T clamp(const T &t) const
  {
    return max(lower, min(t, upper));
  }

  T lower{T(std::numeric_limits<float>::max())};
  T upper{T(-std::numeric_limits<float>::max())};
};

using box1 = range_t<float>;
using box3 = range_t<vec3>;

// Operations on range_t //

template <typename T>
inline typename range_t<T>::element_t size(const range_t<T> &r)
{
  return r.upper - r.lower;
}

template <typename T>
inline typename range_t<T>::element_t center(const range_t<T> &r)
{
  return .5f * (r.lower + r.upper);
}

template <typename T>
inline typename range_t<T>::element_t clamp(const typename range_t<T>::element_t &t, const range_t<T> &r)
{
  return max(r.lower, min(t, r.upper));
}

// Helper functions ///////////////////////////////////////////////////////////

inline size_t longProduct(uvec3 v)
{
  return size_t(v.x) * size_t(v.y) * size_t(v.z);
}

inline float reduce_max(const vec3 &v)
{
  return std::max(std::max(v.x, v.y), v.z);
}

inline float reduce_max(const vec4 &v)
{
  return std::max(std::max(std::max(v.x, v.y), v.z), v.w);
}

inline vec3 xfmPoint(const mat4 &m, const vec3 &p)
{
  return vec3(m * vec4(p, 1.f));
}

inline box3 xfmBox(const mat4 &m, const box3 &b)
{
  box3 retval;
  retval.extend(xfmPoint(m, vec3(b.lower.x, b.lower.y, b.lower.z)));
  retval.extend(xfmPoint(m, vec3(b.lower.x, b.lower.y, b.upper.z)));
  retval.extend(xfmPoint(m, vec3(b.lower.x, b.upper.y, b.lower.z)));
  retval.extend(xfmPoint(m, vec3(b.lower.x, b.upper.y, b.upper.z)));
  retval.extend(xfmPoint(m, vec3(b.upper.x, b.lower.y, b.lower.z)));
  retval.extend(xfmPoint(m, vec3(b.upper.x, b.lower.y, b.upper.z)));
  retval.extend(xfmPoint(m, vec3(b.upper.x, b.upper.y, b.lower.z)));
  retval.extend(xfmPoint(m, vec3(b.upper.x, b.upper.y, b.upper.z)));
  return retval;
}

inline mat4 calculateRotation(vec3 direction, vec3 startDirection = vec3(0, 0, -1))
{
  if (direction == startDirection)
  {
    return mat4(1); // no rotation
  }
  if (direction == -startDirection)
  {
    return rotate(mat4(1), pi<float32>(), vec3(1, 0, 0)); // reverse on 180 degrees
  }
  float angle = acos(dot(normalize(direction), normalize(startDirection)));
  vec3  axis  = cross(startDirection, direction);
  return rotate(mat4(1), angle, axis);
}

} // namespace anari::rpr
