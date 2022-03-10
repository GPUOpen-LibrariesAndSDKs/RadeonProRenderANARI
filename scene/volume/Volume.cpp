#include "Volume.h"
#include "ScivisVolume.h"

namespace anari{
namespace rpr{

Volume::Volume(rpr_context context, rpr_material_system matsys) : m_context(context), m_matsys(matsys){}

void Volume::commit()
{
  clear();
}

Volume *Volume::createInstance(const char *type, rpr_context context, rpr_material_system matsys)
{
  if(strcmp(type, "scivis") == 0)
  {
    return new ScivisVolume(context, matsys);
  }
  throw std::runtime_error("cannot create volume");
}

void Volume::getInstances(std::vector<rpr_shape> &out_shapes, mat4 externalTransform)
{
  rpr_shape shape;
  rpr_mesh_info mesh_properties[16];
  mesh_properties[0] = (rpr_mesh_info)RPR_MESH_VOLUME_FLAG;
  mesh_properties[1] = (rpr_mesh_info)1; // enable the Volume flag for the Mesh
  mesh_properties[2] = (rpr_mesh_info)0;
  rprContextCreateMeshEx2(m_context, nullptr, 0, 0, nullptr, 0, 0, nullptr, 0, 0, 0, nullptr, nullptr,
                          nullptr, nullptr, 0, nullptr, 0, nullptr, nullptr, nullptr, 0, mesh_properties, &shape);
  CHECK(rprShapeSetVolumeMaterial(shape, m_volume_material))

  mat4 transform = getVolumeTransform() * externalTransform;
  CHECK(rprShapeSetTransform(shape, false, value_ptr(transform)))

  out_shapes.push_back(shape);
}

void Volume::addToScene(rpr_scene scene)
{
  getInstances(m_instances, mat4(1));
  CHECK(rprSceneAttachShape(scene, m_instances[m_instances.size() - 1]))
}

void Volume::clear() {
  clearInstances();
  if(m_volume_material)
  {
    CHECK(rprObjectDelete(m_volume_material))
  }
}

void Volume::clearInstances()
{
  for(rpr_shape shape : m_instances)
  {
    CHECK(rprObjectDelete(shape))
  }
  m_instances.clear();
}

Volume::~Volume()
{
  clear();
}

}

ANARI_TYPEFOR_DEFINITION(rpr::Volume *);

}