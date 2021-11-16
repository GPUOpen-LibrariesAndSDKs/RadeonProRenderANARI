#include "RPRDevice.h"

// std
#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <map>

namespace anari{
namespace rpr{

///////////////////////////////////////////////////////////////////////////////
// Helper functions ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void writeToVoidP(void *_p, T v)
{
    T *p = (T *)_p;
    *p = v;
}

template <typename T, typename... Args>
inline T *createRegisteredObject(Args &&...args)
{
    return new T(std::forward<Args>(args)...);
}

template <typename HANDLE_T, typename OBJECT_T>
inline HANDLE_T getHandleForAPI(OBJECT_T *object)
{
    return (HANDLE_T)object;
}

template <typename OBJECT_T, typename HANDLE_T, typename... Args>
inline HANDLE_T createObjectForAPI(Args &&...args)
{
    auto o = createRegisteredObject<OBJECT_T>(std::forward<Args>(args)...);
    o->setObjectType(ANARITypeFor<HANDLE_T>::value);
    return getHandleForAPI<HANDLE_T>(o);
}

template <typename HANDLE_T>
inline HANDLE_T createPlaceholderObject()
{
    return createObjectForAPI<Object, HANDLE_T>();
}

template <typename OBJECT_T = Object, typename HANDLE_T = ANARIObject>
inline OBJECT_T &referenceFromHandle(HANDLE_T handle)
{
    return *((OBJECT_T *)handle);
}

using SetParamFcn = void(ANARIObject, const char *, const void *);

template <typename T>
static void setParamOnObject(ANARIObject obj, const char *p, const T &v)
{
    referenceFromHandle(obj).setParam(p, v);
}

static void removeParamOnObject(ANARIObject obj, const char *p)
{
    referenceFromHandle(obj).removeParam(p);
}

#define declare_param_setter(TYPE)                                             \
  {                                                                            \
    ANARITypeFor<TYPE>::value,                                                 \
        [](ANARIObject o, const char *p, const void *v) {                      \
          setParamOnObject(o, p, *(TYPE *)v);                                  \
        }                                                                      \
  }

#define declare_param_setter_object(TYPE)                                      \
  {                                                                            \
    ANARITypeFor<TYPE>::value,                                                 \
        [](ANARIObject o, const char *p, const void *v) {                      \
          using OBJECT_T = typename std::remove_pointer<TYPE>::type;           \
          auto ptr = *((TYPE *)v);                                             \
          if (ptr)                                                             \
            setParamOnObject(o, p, IntrusivePtr<OBJECT_T>(ptr));               \
          else                                                                 \
            removeParamOnObject(o, p);                                         \
        }                                                                      \
  }

#define declare_param_setter_string(TYPE)                                      \
  {                                                                            \
    ANARITypeFor<TYPE>::value,                                                 \
        [](ANARIObject o, const char *p, const void *v) {                      \
          const char *str = (const char *)v;                                   \
          setParamOnObject(o, p, std::string(str));                            \
        }                                                                      \
  }

#define declare_param_setter_void_ptr(TYPE)                                    \
  {                                                                            \
    ANARITypeFor<TYPE>::value,                                                 \
        [](ANARIObject o, const char *p, const void *v) {                      \
          setParamOnObject(o, p, const_cast<void *>(v));                       \
        }                                                                      \
  }

static std::map<int, SetParamFcn *> setParamFcns = {
    declare_param_setter(ANARIDataType),
    declare_param_setter(Device *),
    declare_param_setter_void_ptr(void *),
    declare_param_setter(ANARIFrameCompletionCallback),
    declare_param_setter(bool),
    declare_param_setter_object(Object *),
    declare_param_setter_object(Camera *),
    declare_param_setter_object(Array *),
    declare_param_setter_object(Array1D *),
    declare_param_setter_object(Array2D *),
    declare_param_setter_object(Array3D *),
    declare_param_setter_object(Frame *),
    declare_param_setter_object(Geometry *),
    declare_param_setter_object(Group *),
    declare_param_setter_object(Instance *),
    // declare_param_setter_object(Light *),
    declare_param_setter_object(Material *),
    declare_param_setter_object(Renderer *),
    // declare_param_setter_object(Sampler *),
    declare_param_setter_object(Surface *),
    declare_param_setter_object(SpatialField *),
    declare_param_setter_object(Volume *),
    declare_param_setter_object(World *),
    declare_param_setter_string(const char *),
    declare_param_setter(char),
    declare_param_setter(unsigned char),
    declare_param_setter(short),
    declare_param_setter(int),
    declare_param_setter(unsigned int),
    declare_param_setter(long),
    declare_param_setter(unsigned long),
    declare_param_setter(long long),
    declare_param_setter(unsigned long long),
    declare_param_setter(float),
    declare_param_setter(double),
    declare_param_setter(ivec2),
    declare_param_setter(ivec3),
    declare_param_setter(ivec4),
    declare_param_setter(uvec2),
    declare_param_setter(uvec3),
    declare_param_setter(uvec4),
    declare_param_setter(vec2),
    declare_param_setter(vec3),
    declare_param_setter(vec4),
    declare_param_setter(mat2),
    declare_param_setter(mat3),
    declare_param_setter(mat3x2),
    declare_param_setter(mat4x3),
    declare_param_setter(mat4)};

#undef declare_param_setter
#undef declare_param_setter_object
#undef declare_param_setter_string
#undef declare_param_setter_void_ptr

} // rpr
} // anari

