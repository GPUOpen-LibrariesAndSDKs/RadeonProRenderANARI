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

#include "RPRDevice.h"

#include "anari/detail/Library.h"

#include "camera/Camera.h"
#include "frame/Frame.h"
#include "material/Material.h"
#include "sampler/Sampler.h"
#include "scene/geometry/Geometry.h"
#include "scene/geometry/Surface.h"
#include "scene/Instance.h"
#include "scene/lights/Light.h"
#include "scene/volume/SpatialField.h"
#include "scene/volume/Volume.h"
#include "scene/World.h"

#include "version.h"

// std
#include <chrono>
#include <exception>
#include <limits>
#include <map>

// Define list of available render plugins
#if defined(WIN32)
std::map<std::string, std::string> RPRPlugins = {{"Northstar", "Northstar64.dll"}};
//{"HybridPro", "HybridPro.dll"}}; // hybrid support will be added later
#elif defined(__APPLE__)
std::map<std::string, std::string> RPRPlugins = {{"Northstar", "libNorthstar64.dylib"}};
#else
std::map<std::string, std::string> RPRPlugins = {{"Northstar", "libNorthstar64.so"}};
//{"HybridPro", "./HybridPro.so"}}; // hybrid support will be added later
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

namespace anari::rpr {

///////////////////////////////////////////////////////////////////////////////
// Helper functions ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void writeToVoidP(void *_p, T v)
{
  T *p = (T *)_p;
  *p   = v;
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

#define declare_param_setter(TYPE)                                                                           \
  {                                                                                                          \
    ANARITypeFor<TYPE>::value,                                                                               \
        [](ANARIObject o, const char *p, const void *v) { setParamOnObject(o, p, *(TYPE *)v); }              \
  }

#define declare_param_setter_object(TYPE)                                                                    \
  {                                                                                                          \
    ANARITypeFor<TYPE>::value, [](ANARIObject o, const char *p, const void *v) {                             \
      using OBJECT_T = typename std::remove_pointer<TYPE>::type;                                             \
      auto ptr       = *((TYPE *)v);                                                                         \
      if (ptr)                                                                                               \
        setParamOnObject(o, p, IntrusivePtr<OBJECT_T>(ptr));                                                 \
      else                                                                                                   \
        removeParamOnObject(o, p);                                                                           \
    }                                                                                                        \
  }

#define declare_param_setter_string(TYPE)                                                                    \
  {                                                                                                          \
    ANARITypeFor<TYPE>::value, [](ANARIObject o, const char *p, const void *v) {                             \
      const char *str = (const char *)v;                                                                     \
      setParamOnObject(o, p, std::string(str));                                                              \
    }                                                                                                        \
  }

#define declare_param_setter_void_ptr(TYPE)                                                                  \
  {                                                                                                          \
    ANARITypeFor<TYPE>::value,                                                                               \
        [](ANARIObject o, const char *p, const void *v) { setParamOnObject(o, p, const_cast<void *>(v)); }   \
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
    declare_param_setter_object(Sampler *),
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

///////////////////////////////////////////////////////////////////////////////
// ExampleDevice definitions //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int RPRDevice::deviceImplements(const char *_extension)
{
  std::string extension = _extension;
  if (extension == "ANARI_KHR_AREA_LIGHTS")
    return 1;
  if (extension == "ANARI_KHR_FRAME_COMPLETION_CALLBACK")
    return 1;
  return 0;
}

void RPRDevice::deviceSetParameter(const char *_id, ANARIDataType type, const void *mem)
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
  m_plugin_name   = getParam<std::string>("RPRPlugin", m_plugin_name);
  m_render_device = getParam<unsigned int>("RPRDevice", m_render_device);

  // Register rendering plugin
  rpr_int pluginID = rprRegisterPlugin(RPRPlugins[m_plugin_name].c_str());
  CHECK_NE(pluginID, -1)
  rpr_int plugins[]   = {pluginID};
  size_t  pluginCount = sizeof(plugins) / sizeof(plugins[0]);

  // Create context the selected GPU
  // TODO handle multiple devices
  rpr_creation_flags flags = RPRDeviceMap[m_render_device];
#ifdef __APPLE__
  flags |= RPR_CREATION_FLAGS_ENABLE_METAL;
#endif
  CHECK(rprCreateContext(RPR_API_VERSION, plugins, pluginCount, flags, nullptr, nullptr, &m_context));

  // Set active plugin.
  CHECK(rprContextSetActivePlugin(m_context, plugins[0]));

  CHECK(rprContextSetParameterByKey1u(m_context, RPR_CONTEXT_Y_FLIP, 0))

  CHECK(rprContextCreateMaterialSystem(m_context, 0, &m_matsys))
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

ANARIArray1D RPRDevice::newArray1D(void *appMemory, ANARIMemoryDeleter deleter, void *userData,
    ANARIDataType type, uint64_t numItems, uint64_t byteStride)
{
  if (isObject(type))
  {
    return createObjectForAPI<ObjectArray, ANARIArray1D>(
        appMemory, deleter, userData, type, numItems, byteStride);
  } else
  {
    return createObjectForAPI<Array1D, ANARIArray1D>(
        appMemory, deleter, userData, type, numItems, byteStride);
  }
}

ANARIArray2D RPRDevice::newArray2D(void *appMemory, ANARIMemoryDeleter deleter, void *userData,
    ANARIDataType type, uint64_t numItems1, uint64_t numItems2, uint64_t byteStride1, uint64_t byteStride2)
{
  return createObjectForAPI<Array2D, ANARIArray2D>(
      appMemory, deleter, userData, type, numItems1, numItems2, byteStride1, byteStride2);
}

ANARIArray3D RPRDevice::newArray3D(void *appMemory, ANARIMemoryDeleter deleter, void *userData,
    ANARIDataType type, uint64_t numItems1, uint64_t numItems2, uint64_t numItems3, uint64_t byteStride1,
    uint64_t byteStride2, uint64_t byteStride3)
{
  return createObjectForAPI<Array3D, ANARIArray3D>(appMemory, deleter, userData, type, numItems1, numItems2,
      numItems3, byteStride1, byteStride2, byteStride3);
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
  return (ANARIGeometry)Geometry::createInstance(m_context, m_matsys, type);
}

ANARISpatialField RPRDevice::newSpatialField(const char *type)
{
  return (ANARISpatialField)SpatialField::createInstance(type);
}

ANARISurface RPRDevice::newSurface()
{
  return createObjectForAPI<Surface, ANARISurface>(m_matsys);
}

ANARIVolume RPRDevice::newVolume(const char *type)
{
  return (ANARIVolume)Volume::createInstance(type, m_context, m_matsys);
}

// Model Meta-Data ////////////////////////////////////////////////////////////

ANARIMaterial RPRDevice::newMaterial(const char *type)
{
  return (ANARIMaterial)Material::createInstance(type, m_matsys);
}

ANARISampler RPRDevice::newSampler(const char *type)
{
  return (ANARISampler)Sampler::createInstance(type, m_context, m_matsys);
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIGroup RPRDevice::newGroup()
{
  return createObjectForAPI<Group, ANARIGroup>();
}

ANARIInstance RPRDevice::newInstance()
{
  return createObjectForAPI<Instance, ANARIInstance>();
}

// Top-level Worlds ///////////////////////////////////////////////////////////

ANARIWorld RPRDevice::newWorld()
{
  return createObjectForAPI<World, ANARIWorld>();
}

int RPRDevice::getProperty(
    ANARIObject object, const char *name, ANARIDataType type, void *mem, uint64_t size, ANARIWaitMask mask)
{
  if (mask == ANARI_WAIT)
    flushCommitBuffer();

  if ((void *)object == (void *)this)
  {
    std::string_view prop = name;
    if (prop == "version" && type == ANARI_INT32)
    {
      int version = RPR_ANARI_VERSION_MAJOR * 10000 + RPR_ANARI_VERSION_MINOR * 100 + RPR_ANARI_VERSION_PATCH;
      writeToVoidP(mem, version);
      return 1;
    }
    if (prop == "version.major" && type == ANARI_INT32)
    {
      int version = RPR_ANARI_VERSION_MAJOR;
      writeToVoidP(mem, version);
      return 1;
    }
    if (prop == "version.minor" && type == ANARI_INT32)
    {
      int version = RPR_ANARI_VERSION_MINOR;
      writeToVoidP(mem, version);
      return 1;
    }
    if (prop == "version.patch" && type == ANARI_INT32)
    {
      int version = RPR_ANARI_VERSION_PATCH;
      writeToVoidP(mem, version);
      return 1;
    }
    if (prop == "geometryMaxIndex" && type == ANARI_UINT64)
    {
      uint64 limit = std::numeric_limits<rpr_int>::max();
      writeToVoidP(mem, limit);
      return 1;
    }
  } else
    return referenceFromHandle(object).getProperty(name, type, mem, mask);

  return 0;
}

// Object + Parameter Lifetime Management /////////////////////////////////////

void RPRDevice::setParameter(ANARIObject object, const char *name, ANARIDataType type, const void *mem)
{
  if (type == ANARI_UNKNOWN)
    throw std::runtime_error("cannot set ANARI_UNKNOWN parameter type");

  auto *fcn = setParamFcns[type];

  if (fcn)
    fcn(object, name, mem);
  else
  {
    fprintf(stderr, "warning - no parameter setter for type '%i'\n", int(type));
  }
}

void RPRDevice::unsetParameter(ANARIObject o, const char *name)
{
  referenceFromHandle(o).removeParam(name);
}

void RPRDevice::commit(ANARIObject h)
{
  auto &o = referenceFromHandle(h);
  o.refInc(RefType::INTERNAL);
  if (o.commitPriority() != COMMIT_PRIORITY_DEFAULT)
    m_needToSortCommits = true;
  m_objectsToCommit.push_back(&o);
}

void RPRDevice::release(ANARIObject o)
{
  if (!o)
    return;

  auto &obj = referenceFromHandle(o);

  bool privatizeArray =
      isArray(obj.type()) && obj.useCount(RefType::INTERNAL) > 0 && obj.useCount(RefType::PUBLIC) == 1;

  referenceFromHandle(o).refDec(RefType::PUBLIC);

  if (privatizeArray)
    ((Array *)o)->privatize();
}

void RPRDevice::retain(ANARIObject o)
{
  referenceFromHandle(o).refInc(RefType::PUBLIC);
}

// Frame Manipulation /////////////////////////////////////////////////////////

ANARIFrame RPRDevice::newFrame()
{
  return createObjectForAPI<Frame, ANARIFrame>(m_context);
}

const void *RPRDevice::frameBufferMap(ANARIFrame _fb, const char *channel)
{
  auto &fb = referenceFromHandle<Frame>(_fb);

  if (channel == std::string("color"))
    return fb.mapColorBuffer();
  else if (channel == std::string("depth"))
    return fb.mapDepthBuffer();
  else
    return nullptr;
}

void RPRDevice::frameBufferUnmap(ANARIFrame _fb, const char *channel)
{
  // no-op
}

// Frame Rendering ////////////////////////////////////////////////////////////

ANARIRenderer RPRDevice::newRenderer(const char *type)
{
  return (ANARIRenderer)Renderer::createInstance(m_context, type);
}

void RPRDevice::renderFrame(ANARIFrame frame)
{
  flushCommitBuffer();

  auto *f = &referenceFromHandle<Frame>(frame);

  auto device_handle = this_device();

  f->renewFuture();
  auto future = f->future();

  std::thread([=]() {
    auto start = std::chrono::steady_clock::now();
    f->renderFrame();
    auto end = std::chrono::steady_clock::now();

    f->invokeContinuation(device_handle);
    f->setDuration(std::chrono::duration<float>(end - start).count());

    future->markComplete();
  }).detach();
}

int RPRDevice::frameReady(ANARIFrame f, ANARIWaitMask m)
{
  auto &frame = referenceFromHandle<Frame>(f);
  if (!frame.futureIsValid())
    return 0;
  else if (m == ANARI_NO_WAIT)
    return frame.future()->isReady();
  else
  {
    frame.future()->wait();
    return 1;
  }
}

void RPRDevice::discardFrame(ANARIFrame)
{
  // TODO
}

// Other RPRDevice definitions ////////////////////////////////////////////

RPRDevice::RPRDevice()
{
  deviceCommit();
}

RPRDevice::~RPRDevice()
{
  CHECK(rprObjectDelete(m_matsys))
  CheckNoLeak(m_context);
  CHECK(rprObjectDelete(m_context));
  m_context = nullptr; // Always delete the RPR Context last.
}

void RPRDevice::flushCommitBuffer()
{
  if (m_needToSortCommits)
  {
    std::sort(m_objectsToCommit.begin(), m_objectsToCommit.end(),
        [](Object *o1, Object *o2) { return o1->commitPriority() < o2->commitPriority(); });
  }

  m_needToSortCommits = false;

  for (auto o : m_objectsToCommit)
  {
    o->commit();
    o->markUpdated();
    o->refDec(RefType::INTERNAL);
  }

  m_objectsToCommit.clear();
}

} // namespace anari::rpr

static char deviceName[] = "rpr";

extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_NEW_DEVICE(rpr, subtype)
{
  return (ANARIDevice) new anari::rpr::RPRDevice();
}

extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_INIT(rpr)
{
  printf("...loaded rpr library!\n");
}

extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_DEVICE_SUBTYPES(rpr, libdata)
{
  static const char *devices[] = {deviceName, nullptr};
  return devices;
}

extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_OBJECT_SUBTYPES(
    rpr, libdata, deviceSubtype, objectType)
{
  // TODO
  return nullptr;
}

extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_OBJECT_PARAMETERS(
    rpr, libdata, deviceSubtype, objectSubtype, objectType)
{
  // TODO
  return nullptr;
}
extern "C" RPR_DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_PARAMETER_PROPERTY(rpr, libdata, deviceSubtype,
    objectSubtype, objectType, parameterName, parameterType, propertyName, propertyType)
{
  // TODO
  return nullptr;
}

extern "C" RPR_DEVICE_INTERFACE ANARIDevice anariNewRPRDevice()
{
  return (ANARIDevice) new anari::rpr::RPRDevice();
}