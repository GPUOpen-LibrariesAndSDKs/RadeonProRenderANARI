// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "RPRDevice.h"

#include "Data.h"
#include "camera/Camera.h"
#include "frame/Frame.h"
#include "renderer/Renderer.h"
#include "scene/Instance.h"
#include "scene/geometry/GeometricModel.h"
#include "scene/geometry/Geometry.h"
#include "scene/lights/Light.h"

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
    {"Northstar", "libNorthstar64.so"},
    {"Tahoe", "libTahoe64.so"},
    {"Hybrid", "Hybrid.so"}};
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

namespace anari {
namespace rpr {

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
  }                                                                            \
                                                                               \
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

///////////////////////////////////////////////////////////////////////////////
// RPRDevice definitions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int RPRDevice::deviceImplements(const char *_extension)
{
    std::string extension = _extension;
    if (extension == ANARI_KHR_FRAME_COMPLETION_CALLBACK)
        return 1;
    return 0;
}

void RPRDevice::deviceSetParameter(
        const char *_id, ANARIDataType type, const void *mem)
{
    std::string id = _id;
    if (id == "numThreads" && type == ANARI_INT32) {
        setParam(id, *static_cast<const int *>(mem));
    }
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
      RPR_API_VERSION, plugins, pluginCount, flags, NULL, NULL, &m_context));

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


// Modules ////////////////////////////////////////////////////////////////////

int RPRDevice::loadModule(const char *name)
{
  // TODO
  return 1;
}

// Data Arrays ////////////////////////////////////////////////////////////////

ANARIData RPRDevice::newSharedData(const void *sharedData,
    ANARIDataType type,
    uint64_t numItems1,
    int64_t byteStride1,
    uint64_t numItems2,
    int64_t byteStride2,
    uint64_t numItems3,
    int64_t byteStride3)
{
  if (byteStride1 != 0 || byteStride2 != 0 || byteStride3 != 0)
    throw std::runtime_error("Cannot handle strided data arrays!");

  return createObjectForAPI<SharedData, ANARIData>(
      sharedData, type, uvec3(numItems1, numItems2, numItems3));
}

ANARIData RPRDevice::newData(ANARIDataType type,
    uint64_t numItems1,
    uint64_t numItems2,
    uint64_t numItems3)
{
  return createObjectForAPI<PrivateData, ANARIData>(
      type, uvec3(numItems1, numItems2, numItems3));
}

void RPRDevice::copyData(const ANARIData source,
    ANARIData destination,
    uint64_t dstIndex1,
    uint64_t dstIndex2,
    uint64_t dstIndex3)
{
  if (dstIndex1 != 0 || dstIndex2 != 0 || dstIndex3 != 0)
    throw std::runtime_error("can only copy into base (0,0,0) indices");

  auto &dst = referenceFromHandle<Data>(destination);
  auto &src = referenceFromHandle<Data>(source);

  if (dst.size() != src.size())
    throw std::runtime_error("can only copy arrays of the same size");

  if (dst.type() != src.type())
    throw std::runtime_error("can only copy between arrays of same type");

  dst.copyFrom(src);
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

ANARIVolume RPRDevice::newVolume(const char *type)
{
  return createPlaceholderObject<ANARIVolume>();
}

ANARIGeometricModel RPRDevice::newGeometricModel()
{
  return createObjectForAPI<GeometricModel, ANARIGeometricModel>(m_context);
}

ANARIVolumetricModel RPRDevice::newVolumetricModel(const char *)
{
  return createPlaceholderObject<ANARIVolumetricModel>();
}

// Model Meta-Data ////////////////////////////////////////////////////////////

ANARIMaterial RPRDevice::newMaterial(const char *type)
{
  return (ANARIMaterial)Material::createInstance(type, m_matsys);
}

ANARITransferFunction RPRDevice::newTransferFunction(const char *type)
{
  return createPlaceholderObject<ANARITransferFunction>();
}

ANARITexture RPRDevice::newTexture(const char *type)
{
  return createPlaceholderObject<ANARITexture>();
}

// Instancing /////////////////////////////////////////////////////////////////

ANARIInstance RPRDevice::newInstance()
{
  return createObjectForAPI<Instance, ANARIInstance>();
}

// Top-level Worlds ///////////////////////////////////////////////////////////

ANARIWorld RPRDevice::newWorld()
{
  return createObjectForAPI<World, ANARIWorld>();
}

ANARIBounds RPRDevice::getBounds(ANARIObject o)
{
  ANARIBounds retval;
  auto bounds = referenceFromHandle<>(o).bounds();
  std::memcpy(&retval, &bounds, sizeof(retval));
  return retval;
}

int RPRDevice::getProperty(ANARIObject object,
    const char *name,
    ANARIDataType type,
    void *mem,
    uint64_t size,
    uint32_t mask)
{
  if (std::string("bounds") == name && type == ANARI_BOX3F && mem != nullptr
      && size >= sizeof(ANARIBounds)) {
    ANARIBounds *b = (ANARIBounds *)mem;
    *b = getBounds(object);
    return 1;
  }
  return 0;
}

// Object + Parameter Lifetime Management /////////////////////////////////////

void RPRDevice::setObjectParam(
    ANARIObject object, const char *name, ANARIDataType type, const void *mem)
{
  if (!object)
    return;

  if (type == ANARI_UNKNOWN)
    throw std::runtime_error("cannot set ANARI_UNKNOWN parameter type");

  // if (type == ANARI_BYTE || type == ANARI_DATA) {
  // setParamOnObject(object, name, *(const unsigned char *)mem);
  // return;
  //}

  auto *fcn = setParamFcns[type];

  if (fcn)
    fcn(object, name, mem);
}

void RPRDevice::removeObjectParam(ANARIObject object, const char *name)
{
  auto &obj = referenceFromHandle<>(object);
  auto *existing = obj.getParam<Object *>(name, nullptr);
  if (existing)
    existing->refDec();
  obj.removeParam(name);
}

void RPRDevice::commit(ANARIObject o)
{
  if (o)
    referenceFromHandle<>(o).commit();
}

void RPRDevice::release(ANARIObject o)
{
  if (o)
    referenceFromHandle<>(o).refDec();
}

void RPRDevice::retain(ANARIObject o)
{
  if (o)
    referenceFromHandle<>(o).refInc();
}

// Frame Manipulation /////////////////////////////////////////////////////////

ANARIFrame RPRDevice::frameCreate()
{
  return createObjectForAPI<Frame, ANARIFrame>(m_context);
}

ANARIImageOperation RPRDevice::newImageOperation(const char *type)
{
  return createPlaceholderObject<ANARIImageOperation>();
}

const void *RPRDevice::frameBufferMap(ANARIFrame _fb, const char *channel)
{
  auto &fb = referenceFromHandle<Frame>(_fb);

  if (channel == std::string("color"))
    return fb.mapColorBuffer();
  else
    return nullptr;
}

void RPRDevice::frameBufferUnmap(ANARIFrame fb, const char *channel)
{
  // no-op
}

float RPRDevice::getVariance(ANARIFrame fb)
{
  return std::numeric_limits<float>::infinity();
}

void RPRDevice::resetAccumulation(ANARIFrame fb)
{
  referenceFromHandle<Frame>(fb).clear();
}

// Frame Rendering ////////////////////////////////////////////////////////////

ANARIRenderer RPRDevice::newRenderer(const char *type)
{
  return (ANARIRenderer)Renderer::createInstance(m_context, type);
}

void RPRDevice::renderFrame(ANARIFrame frame)
{
  auto *f = &referenceFromHandle<Frame>(frame);

  f->prepScene(m_context);

  auto frameStartTime = std::chrono::steady_clock::now();
  CHECK(rprContextRender(m_context));
  auto frameEndTime = std::chrono::steady_clock::now();

  auto diff = frameEndTime - frameStartTime;
  f->future().m_duration = std::chrono::duration<double>(diff).count();

  f->invokeContinuation(this_device());
}

int RPRDevice::isReady(ANARIFrame, ANARISyncEvent)
{
  // TODO
  return 1;
}

void RPRDevice::wait(ANARIFrame, ANARISyncEvent)
{
  // TODO
}

void RPRDevice::cancel(ANARIFrame)
{
  // TODO
}

float RPRDevice::getProgress(ANARIFrame)
{
  // TODO
  return 1.f;
}

float RPRDevice::getTaskDuration(ANARIFrame f)
{
  return referenceFromHandle<Frame>(f).future().m_duration;
}

ANARIPickResult RPRDevice::pick(
    ANARIFrame f, float screenPos_x, float screenPos_y)
{
  ANARIPickResult retval;

  // TODO
  std::memset(&retval, 0, sizeof(retval));

  return retval;
}

RPRDevice::~RPRDevice()
{
  rpr_scene scene;
  CHECK(rprContextGetScene(m_context, &scene))
  CHECK(rprObjectDelete(scene))
  CHECK(rprObjectDelete(m_matsys))
  CheckNoLeak(m_context);
  CHECK(rprObjectDelete(m_context));
  m_context = nullptr; // Always delete the RPR Context last.
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Data *);

} // namespace anari

extern "C" ANARI_INTERFACE void anari_module_rpr_init()
{
  anari::Device::registerType<anari::rpr::RPRDevice>("rpr");
}
