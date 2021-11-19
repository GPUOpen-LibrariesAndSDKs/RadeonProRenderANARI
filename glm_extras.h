// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

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
  static thread_local std::minstd_rand generator;
  static std::uniform_real_distribution<float> distribution{0.f, 1.f};
  return distribution(generator);
}

static vec3 randomDir()
{
  static thread_local std::minstd_rand gen;
  static std::uniform_real_distribution<float> dist{-1.f, 1.f};
  vec3 dir(dist(gen), dist(gen), dist(gen));
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
inline typename range_t<T>::element_t clamp(
        const typename range_t<T>::element_t &t, const range_t<T> &r)
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

} // namespace anari
