#pragma once

// anari
#include "anari/detail/Helpers.h"
#include "anari/detail/IntrusivePtr.h"
#include "anari/detail/ParameterizedObject.h"
// anari_cpp
#include "anari/anari_cpp/ext/glm.h"
// glm
#include "glm_extras.h"
//rpr
#include "rpr_common.h"
// std
#include <functional>
#include <map>
#include <memory>

// clang-format off
#define COMMIT_PRIORITY_DEFAULT       0
#define COMMIT_PRIORITY_GEOMETRY      1
#define COMMIT_PRIORITY_SPATIAL_FIELD 1
#define COMMIT_PRIORITY_SURFACE       2
#define COMMIT_PRIORITY_VOLUME        2
#define COMMIT_PRIORITY_GROUP         3
#define COMMIT_PRIORITY_INSTANCE      4
#define COMMIT_PRIORITY_WORLD         5
#define COMMIT_PRIORITY_FRAME         6
// clang-format on

namespace anari {
namespace rpr {

using TimeStamp = uint64_t;
TimeStamp newTimeStamp();

struct Object : public RefCounted, public ParameterizedObject
{
    Object() = default;

    template <typename T>
    T *getParamObject(const std::string &name, T *valIfNotFound = nullptr);

    virtual bool getProperty(
            const std::string &name, ANARIDataType type, void *ptr, ANARIWaitMask m);

    virtual void commit();

    void setObjectType(ANARIDataType type);

    ANARIDataType type() const;

    TimeStamp lastUpdated() const;
    void markUpdated();

    int commitPriority() const;

  protected:
    void setCommitPriority(int p);
    virtual void addToScene(rpr_scene scene) {};

  private:
    ANARIDataType m_type{ANARI_OBJECT};
    TimeStamp m_lastUpdated{newTimeStamp()};
    int m_commitPriority{COMMIT_PRIORITY_DEFAULT};
};

// Helper types for creating objects //

template <typename T>
using FactoryFcn = T *(*)();

template <typename T>
using FactoryMap = std::map<std::string, FactoryFcn<T>>;

template <typename T>
using FactoryMapPtr = std::unique_ptr<FactoryMap<T>>;

// Inlined defintions /////////////////////////////////////////////////////////

template <typename T>
inline T *Object::getParamObject(const std::string &name, T *valIfNotFound)
{
    if (!hasParam(name))
        return valIfNotFound;

    using OBJECT_T = typename std::remove_pointer<T>::type;
    using PTR_T = IntrusivePtr<OBJECT_T>;

    PTR_T val = getParam<PTR_T>(name, PTR_T());
    return val.ptr;
}

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Object *, ANARI_OBJECT);

} // namespace anari
