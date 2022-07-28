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

#include "Object.h"
// std
#include <vector>
// anari
#include "anari/anari_cpp/Traits.h"

namespace anari {
namespace rpr {

enum class ArrayShape
{
  ARRAY1D,
  ARRAY2D,
  ARRAY3D
};

struct Array : public Object
{
  Array(const void *appMemory, ANARIMemoryDeleter deleter, const void *deleterPtr, ANARIDataType elementType);
  ~Array() override;

  ANARIDataType elementType() const;

  void *map();
  void  unmap();

  template <typename T>
  T *dataAs();

  virtual ArrayShape shape() const = 0;

  virtual void privatize() = 0;
  bool         wasPrivatized() const;

 protected:
  void makePrivatizedCopy(size_t numElements);
  void freeAppMemory();

  const void        *m_mem{nullptr};
  ANARIMemoryDeleter m_deleter{nullptr};
  const void        *m_deleterPtr{nullptr};

 private:
  ANARIDataType m_elementType{ANARI_UNKNOWN};
  bool          m_privatized{false};
};

struct Array1D : public Array
{
  Array1D(const void *appMemory, ANARIMemoryDeleter deleter, const void *deleterPtr, ANARIDataType type,
      uint64_t numItems, uint64_t byteStride);

  ArrayShape shape() const override;

  size_t size() const;

  void privatize() override;

 private:
  size_t m_size{0};
};

struct Array2D : public Array
{
  Array2D(const void *appMemory, ANARIMemoryDeleter deleter, const void *deleterPtr, ANARIDataType type,
      uint64_t numItems1, uint64_t numItems2, uint64_t byteStride1, uint64_t byteStride2);

  ArrayShape shape() const override;

  size_t size(int dim) const;
  uvec2  size() const;

  void privatize() override;

 private:
  size_t m_size[2] = {0, 0};
};

struct Array3D : public Array
{
  Array3D(const void *appMemory, ANARIMemoryDeleter deleter, const void *deleterPtr, ANARIDataType type,
      uint64_t numItems1, uint64_t numItems2, uint64_t numItems3, uint64_t byteStride1, uint64_t byteStride2,
      uint64_t byteStride3);

  ArrayShape shape() const override;

  size_t size(int dim) const;
  uvec3  size() const;

  void privatize() override;

 private:
  size_t m_size[3] = {0, 0, 0};
};

struct ObjectArray : public Array
{
  ObjectArray(const void *appMemory, ANARIMemoryDeleter deleter, const void *deleterPtr, ANARIDataType type,
      uint64_t numItems, uint64_t byteStride);
  ~ObjectArray() override;

  ArrayShape shape() const override;

  size_t size() const;

  void privatize() override;

  Object **handles();

  void appendHandle(Object *);
  void removeAppendedHandles();

 private:
  std::vector<Object *> m_handleArray;
  size_t                m_numAppended{0};
};

// Inlined definitions ////////////////////////////////////////////////////////

template <typename T>
inline T *Array::dataAs()
{
  if (ANARITypeFor<T>::value != m_elementType)
    throw std::runtime_error("incorrect element type queried for array");
  return (T *)m_mem;
}

// Object specializations /////////////////////////////////////////////////////

template <>
inline ObjectArray *Object::getParamObject<ObjectArray>(const std::string &name, ObjectArray *valIfNotFound)
{
  if (!hasParam(name))
    return valIfNotFound;

  using PTR_T = IntrusivePtr<Array1D>;
  auto val    = getParam<PTR_T>(name, PTR_T());
  return (ObjectArray *)val.ptr;
}

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Array *, ANARI_ARRAY);
ANARI_TYPEFOR_SPECIALIZATION(rpr::Array1D *, ANARI_ARRAY1D);
ANARI_TYPEFOR_SPECIALIZATION(rpr::Array2D *, ANARI_ARRAY2D);
ANARI_TYPEFOR_SPECIALIZATION(rpr::Array3D *, ANARI_ARRAY3D);
ANARI_TYPEFOR_SPECIALIZATION(rpr::ObjectArray *, ANARI_ARRAY1D);

} // namespace anari
