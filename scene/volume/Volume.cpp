/**********************************************************************
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

#include "Volume.h"
#include "ScivisVolume.h"

namespace anari {
namespace rpr {

Volume::Volume(rpr_context context, rpr_material_system matsys) : m_context(context), m_matsys(matsys) {}

void Volume::commit()
{
  clear();
}

Volume *Volume::createInstance(const char *type, rpr_context context, rpr_material_system matsys)
{
  if (strcmp(type, "scivis") == 0)
  {
    return new ScivisVolume(context, matsys);
  }
  throw std::runtime_error("cannot create volume");
}

void Volume::attachToScene(rpr_scene scene, mat4 transform)
{
  rpr_shape     shape;
  rpr_mesh_info mesh_properties[16];
  mesh_properties[0] = (rpr_mesh_info)RPR_MESH_VOLUME_FLAG;
  mesh_properties[1] = (rpr_mesh_info)1; // enable the Volume flag for the Mesh
  mesh_properties[2] = (rpr_mesh_info)0;
  rprContextCreateMeshEx2(m_context, nullptr, 0, 0, nullptr, 0, 0, nullptr, 0, 0, 0, nullptr, nullptr,
      nullptr, nullptr, 0, nullptr, 0, nullptr, nullptr, nullptr, 0, mesh_properties, &shape);
  CHECK(rprShapeSetVolumeMaterial(shape, m_volume_material))

  mat4 transformResult = getVolumeTransform() * transform;
  CHECK(rprShapeSetTransform(shape, false, value_ptr(transformResult)))
  m_instances.push_back(shape);
  CHECK(rprSceneAttachShape(scene, shape))
}

void Volume::clear()
{
  for (rpr_shape shape : m_instances)
  {
    CHECK(rprObjectDelete(shape))
  }
  m_instances.clear();

  if (m_volume_material)
  {
    CHECK(rprObjectDelete(m_volume_material))
  }
}

Volume::~Volume()
{
  clear();
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Volume *);

} // namespace anari