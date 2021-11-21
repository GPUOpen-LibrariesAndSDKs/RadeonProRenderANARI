#include "RPRDevice.h"

#include "camera/Camera.h"

#include "scene/lights/Light.h"
#include "scene/geometry/Geometry.h"
#include "scene/geometry/Surface.h"

#include "material/Material.h"

// std
#include <chrono>
#include <exception>
#include <functional>
#include <limits>
#include <map>

// Define list of available render plugins
#if defined(WIN32)
std::map<std::string, std::string> RPRPlugins = {
    {"Northstar", "Northstar64.dll"},
    {"Tahoe", "Tahoe64.dll"},
    {"Hybrid", "Hybrid.dll"}};
#elif defined(__APPLE__)
std::map<std::string, std::string> RPRPlugins = {
    {"Northstar", "libNorthstar64.dylib"}, {"Tahoe", "libTahoe64.dylib"}};
#else
std::map<std::string, std::string> RPRPlugins = {
    {"Northstar", "./libNorthstar64.so"},
    {"Tahoe", "./libTahoe64.so"},
    {"Hybrid", "./Hybrid.so"}};
#endif

std::map<unsigned int, unsigned int> RPRDeviceMap = {
    {0, RPR_CREATION_FLAGS_ENABLE_CPU},
    {1, RPR_CREATION_FLAGS_ENABLE_GPU0},
    {2, RPR_CREATION_FLAGS_ENABLE_GPU1},
    {3, RPR_CREATION_FLAGS_ENABLE_GPU2},
    {4, RPR_CREATION_FLAGS_ENABLE_GPU3},
    {5, RPR_CREATION_FLAGS_ENABLE_GPU4},
    {6, RPR_CREATION_FLAGS_ENABLE_GPU5},
    {7, RPR_CREATION_FLAGS_ENABLE_GPU6},
    {8, RPR_CREATION_FLAGS_ENABLE_GPU7},
    {9, RPR_CREATION_FLAGS_ENABLE_GPU8},
    {10, RPR_CREATION_FLAGS_ENABLE_GPU9},
    {11, RPR_CREATION_FLAGS_ENABLE_GPU10},
    {12, RPR_CREATION_FLAGS_ENABLE_GPU11},
    {13, RPR_CREATION_FLAGS_ENABLE_GPU12},
    {14, RPR_CREATION_FLAGS_ENABLE_GPU13},
    {15, RPR_CREATION_FLAGS_ENABLE_GPU14},
    {16, RPR_CREATION_FLAGS_ENABLE_GPU15},
};


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
    declare_param_setter_object(Light *),
    declare_param_setter_object(Material *),
    declare_param_setter_object(Renderer *),
    // declare_param_setter_object(Sampler *),
    declare_param_setter_object(Surface *),
    // declare_param_setter_object(SpatialField *),
    // declare_param_setter_object(Volume *),
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

///////////////////////////////////////////////////////////////////////////////
// ExampleDevice definitions //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int RPRDevice::deviceImplements(const char *_extension)
{
    std::string extension = _extension;
    if (extension == ANARI_KHR_AREA_LIGHTS)
        return 1;
    return 0;
}

void RPRDevice::deviceSetParameter(
        const char *_id, ANARIDataType type, const void *mem)
{
    std::string id = _id;
    if (type == ANARI_STRING)
        setParam(id, std::string((static_cast<const char *>(mem))));
    else if (type == ANARI_UINT32)
        setParam(id, *(static_cast<const unsigned int *>(mem)));
}

void RPRDevice::deviceUnsetParameter(const char *id)
{
    removeParam(id);
}

void RPRDevice::deviceCommit()
{
  m_plugin_name = getParam<std::string>("RPRPlugin", m_plugin_name);
  m_render_device = getParam<unsigned int>("RPRDevice", m_render_device);

  // Register rendering plugin
  rpr_int pluginID = rprRegisterPlugin(RPRPlugins[m_plugin_name].c_str());
  CHECK_NE(pluginID, -1)
  rpr_int plugins[] = {pluginID};
  size_t pluginCount = sizeof(plugins) / sizeof(plugins[0]);

  // Create context the selected GPU
  // TODO handle multiple devices
  rpr_creation_flags flags = RPRDeviceMap[m_render_device];
#ifdef __APPLE__
  flags |= RPR_CREATION_FLAGS_ENABLE_METAL;
#endif
  CHECK(rprCreateContext(
      RPR_API_VERSION, plugins, pluginCount, flags, nullptr, nullptr, &m_context));

  // Set active plugin.
  CHECK(rprContextSetActivePlugin(m_context, plugins[0]));

  CHECK(rprContextSetParameterByKey1u(m_context, RPR_CONTEXT_Y_FLIP, 0))

  CHECK(rprContextCreateMaterialSystem(m_context, 0, &m_matsys))

  rpr_scene scene;
  CHECK(rprContextCreateScene(m_context, &scene))
  CHECK(rprContextSetScene(m_context, scene))
}

void RPRDevice::deviceRetain()
{
  this->refInc();
}

void RPRDevice::deviceRelease()
{
  this->refDec();
}

// Data Arrays ////////////////////////////////////////////////////////////////

ANARIArray1D RPRDevice::newArray1D(void *appMemory,
    ANARIMemoryDeleter deleter,
    void *userData,
    ANARIDataType type,
    uint64_t numItems,
    uint64_t byteStride)
{
  if (isObject(type)) {
    return createObjectForAPI<ObjectArray, ANARIArray1D>(
        appMemory, deleter, userData, type, numItems, byteStride);
  } else {
    return createObjectForAPI<Array1D, ANARIArray1D>(
        appMemory, deleter, userData, type, numItems, byteStride);
  }
}

ANARIArray2D RPRDevice::newArray2D(void *appMemory,
    ANARIMemoryDeleter deleter,
    void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t byteStride1,
    uint64_t byteStride2)
{
  return createObjectForAPI<Array2D, ANARIArray2D>(appMemory,
      deleter,
      userData,
      type,
      numItems1,
      numItems2,
      byteStride1,
      byteStride2);
}

ANARIArray3D RPRDevice::newArray3D(void *appMemory,
    ANARIMemoryDeleter deleter,
    void *userData,
    ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3,
    uint64_t byteStride1,
    uint64_t byteStride2,
    uint64_t byteStride3)
{
  return createObjectForAPI<Array3D, ANARIArray3D>(appMemory,
      deleter,
      userData,
      type,
      numItems1,
      numItems2,
      numItems3,
      byteStride1,
      byteStride2,
      byteStride3);
}

void *RPRDevice::mapArray(ANARIArray a)
{
  return referenceFromHandle<Array>(a).map();
}

void RPRDevice::unmapArray(ANARIArray a)
{
  referenceFromHandle<Array>(a).unmap();
}

// Renderable Objects /////////////////////////////////////////////////////////

ANARILight RPRDevice::newLight(const char *type)
{
  return (ANARILight)Light::createInstance(m_context, m_matsys, type);
}

ANARICamera RPRDevice::newCamera(const char *type)
{
  return (ANARICamera)Camera::createInstance(m_context, type);
}

ANARIGeometry RPRDevice::newGeometry(const char *type)
{
    return (ANARIGeometry)Geometry::createInstance(m_context, type);
}

ANARISpatialField RPRDevice::newSpatialField(const char *type)
{
    return createPlaceholderObject<ANARISpatialField>();
}

ANARISurface RPRDevice::newSurface()
{
  return createObjectForAPI<Surface, ANARISurface>(m_matsys);
}

ANARIVolume RPRDevice::newVolume(const char *_type)
{
    return createPlaceholderObject<ANARIVolume>();
}

// Model Meta-Data ////////////////////////////////////////////////////////////

ANARIMaterial RPRDevice::newMaterial(const char *type)
{
    return (ANARIMaterial)Material::createInstance(type, m_matsys);
}

ANARISampler RPRDevice::newSampler(const char *type)
{
  return createPlaceholderObject<ANARISampler>();
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIGroup RPRDevice::newGroup()
{
  return createPlaceholderObject<ANARIGroup>();
}

ANARIInstance RPRDevice::newInstance()
{
  return createPlaceholderObject<ANARIInstance>();
}

} // rpr
} // anari

