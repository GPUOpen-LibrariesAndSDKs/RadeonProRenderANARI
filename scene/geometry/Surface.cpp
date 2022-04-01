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

#include "Surface.h"
#include "Geometry.h"

namespace anari {
namespace rpr {

Surface::Surface(rpr_material_system materialSystem) : m_matsys(materialSystem)
{
  setCommitPriority(COMMIT_PRIORITY_SURFACE);
}

void Surface::commit()
{
  IntrusivePtr<Material> material = getParamObject<Material>("material");
  m_geometry                      = getParamObject<Geometry>("geometry");

  if (!m_geometry)
    throw std::runtime_error("'geometry' is a required attribute");
  if (!material)
    throw std::runtime_error("'material' is a required attribute");

  // cleanup before new commit
  clearInstances();
  if (m_material_instance)
  {
    CHECK(rprObjectDelete(m_material_instance));
    m_material_instance = nullptr;
  }

  m_bounds = m_geometry->bounds();

  // generate new material instance
  m_material_instance = material->generateMaterial(m_geometry.ptr);
}

void Surface::attachToScene(rpr_scene scene, mat4 transform)
{
  std::vector<rpr_shape> instances;
  getInstances(instances, transform);

  // attach instances to scene
  for (rpr_shape instance : instances)
  {
    CHECK(rprSceneAttachShape(scene, instance))
    m_instances.push_back(instance);
  }
  // attach base invisible shape
  if (m_geometry->getBaseShape())
  {
    CHECK(rprSceneAttachShape(scene, m_geometry->getBaseShape()))
  }
}

void Surface::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  std::vector<rpr_shape> instances;
  m_geometry->getInstances(instances, transform);

  for (rpr_shape instance : instances)
  {
    CHECK(rprShapeSetMaterial(instance, m_material_instance))
    out_shapes.push_back(instance);
  }
}

void Surface::clearInstances()
{
  for (rpr_shape instance : m_instances)
  {
    CHECK(rprObjectDelete(instance))
  }
  m_instances.clear();
}

Surface::~Surface()
{
  clearInstances();
  if (m_material_instance)
  {
    CHECK(rprObjectDelete(m_material_instance));
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Surface *);
} // namespace anari
