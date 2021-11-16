// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
// std
#include <algorithm>
#include <vector>

namespace anari {
namespace rpr {

struct Data : public Object
{
  Data(ANARIDataType type, uvec3 size);
  virtual ~Data() = default;

  uvec3 size() const;
  ANARIDataType type() const;

  int shape() const;

  virtual const void *data() const = 0;
  virtual void *data() = 0;

  void copyFrom(const Data &other);

 protected:
  void incrementHandles();
  void decrementHandles();

  // Data //
  ANARIDataType m_type{ANARI_UNKNOWN};
  uvec3 m_size{0};
};

struct SharedData : public Data
{
  SharedData(const void *mem, ANARIDataType type, uvec3 size);
  ~SharedData();

  const void *data() const override;
  void *data() override;

 private:
  const void *m_data{nullptr};
};

struct PrivateData : public Data
{
  PrivateData(ANARIDataType type, uvec3 size);
  ~PrivateData();

  const void *data() const override;
  void *data() override;

 private:
  std::vector<unsigned char> m_data;
};

template <typename T, typename SHAPE_T = size_t>
struct DataView
{
  static_assert(std::is_same<SHAPE_T, size_t>::value
      || std::is_same<SHAPE_T, uvec2>::value
      || std::is_same<SHAPE_T, uvec3>::value);

  DataView(Data *data = nullptr);

  SHAPE_T size() const;
  T *data() const;

  operator bool() const;

 private:
  Data *m_data{nullptr};
};

// Inlined definitions ////////////////////////////////////////////////////////

// Data //

inline Data::Data(ANARIDataType type, uvec3 size) : m_type(type), m_size(size)
{}

inline ANARIDataType Data::type() const
{
  return m_type;
}

inline int Data::shape() const
{
  if (m_size.y == 1 && m_size.z == 1)
    return 1;
  else if (m_size.z == 1)
    return 2;
  else
    return 3;
}

inline uvec3 Data::size() const
{
  return m_size;
}

inline void Data::copyFrom(const Data &other)
{
  decrementHandles();
  const size_t numBytes =
      longProduct(other.size()) * sizeOfDataType(other.type());
  std::memcpy(data(), other.data(), numBytes);
  incrementHandles();
}

inline void Data::incrementHandles()
{
  if (!isObject(type()))
    return;

  auto *begin = (Object **)data();
  auto *end = begin + longProduct(size());
  std::for_each(begin, end, [](Object *obj) {
    if (obj)
      obj->refInc();
  });
}

inline void Data::decrementHandles()
{
  if (!isObject(type()))
    return;

  auto *begin = (Object **)data();
  auto *end = begin + longProduct(size());
  std::for_each(begin, end, [](Object *obj) {
    if (obj)
      obj->refDec();
  });
}

// SharedData //

inline SharedData::SharedData(const void *mem, ANARIDataType type, uvec3 size)
    : Data(type, size), m_data(mem)
{
  incrementHandles();
}

inline SharedData::~SharedData()
{
  decrementHandles();
}

inline const void *SharedData::data() const
{
  return m_data;
}

inline void *SharedData::data()
{
  return const_cast<void *>(m_data);
}

// PrivateData //

inline PrivateData::PrivateData(ANARIDataType type, uvec3 size)
    : Data(type, size)
{
  m_data.resize(longProduct(size) * sizeOfDataType(type));
}

inline PrivateData::~PrivateData()
{
  decrementHandles();
}

inline const void *PrivateData::data() const
{
  return m_data.data();
}

inline void *PrivateData::data()
{
  return m_data.data();
}

// DataView //

template <typename T, typename SHAPE_T>
inline DataView<T, SHAPE_T>::DataView(Data *data) : m_data(data)
{
  int incomingShape = 1;
  if (std::is_same<SHAPE_T, uvec2>::value)
    incomingShape = 2;
  else if (std::is_same<SHAPE_T, uvec3>::value)
    incomingShape = 3;

  if (data && (data->shape() != incomingShape))
    throw std::runtime_error("DataView : Data shape mismatch");
}

template <typename T, typename SHAPE_T>
inline SHAPE_T DataView<T, SHAPE_T>::size() const
{
  if (!m_data)
    return SHAPE_T(0);

  if constexpr (std::is_same<SHAPE_T, size_t>::value)
    return m_data->size().x;
  else if constexpr (std::is_same<SHAPE_T, uvec2>::value)
    return uvec2(m_data->size().x, m_data->size().y);
  else
    return m_data->size();
}

template <typename T, typename SHAPE_T>
T *DataView<T, SHAPE_T>::data() const
{
  return m_data == nullptr ? nullptr : (T *)m_data->data();
}

template <typename T, typename SHAPE_T>
DataView<T, SHAPE_T>::operator bool() const
{
  return m_data != nullptr;
}

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Data *, ANARI_DATA);

} // namespace anari
