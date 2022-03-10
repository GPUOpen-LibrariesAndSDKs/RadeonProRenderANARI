// Copyright 2020 The Khronos Surface
// SPDX-License-Identifier: Apache-2.0

#include "Surface.h"
#include "Geometry.h"
#include "../../material/Material.h"

namespace anari {
namespace rpr {

Surface::Surface(rpr_material_system materialSystem) : m_matsys(materialSystem){
    setCommitPriority(COMMIT_PRIORITY_SURFACE);
}

void Surface::commit()
{
  IntrusivePtr<Material> material = getParamObject<Material>("material");
  m_geometry = getParamObject<Geometry>("geometry");

  if(!m_geometry) throw std::runtime_error("'geometry' is a required attribute");
  if(!material) throw std::runtime_error("'material' is a required attribute");

  // cleanup before new commit
  clearInstances();
  if(m_material_instance)
  {
    CHECK(rprObjectDelete(m_material_instance));
    m_material_instance = nullptr;
  }

  m_bounds = m_geometry->bounds();

  // generate new material instance
  m_material_instance = material->generateMaterial(m_geometry.ptr);
}

void Surface::addToScene(rpr_scene scene)
{
  addToScene(scene, mat4(1));
}

void Surface::addToScene(rpr_scene scene, mat4 transform)
{
  std::vector<rpr_shape> instances;
  getInstances(instances, transform);

  // attach instances to scene
  for(rpr_shape instance : instances){
    CHECK(rprSceneAttachShape(scene, instance))
    m_instances.push_back(instance);
  }
  // attach base invisible shape
  CHECK(rprSceneAttachShape(scene, m_geometry->getBaseShape()))
}

void Surface::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  std::vector<rpr_shape> instances;
  m_geometry->getInstances(instances, transform);

  for(rpr_shape instance: instances)
  {
    CHECK(rprShapeSetMaterial(instance, m_material_instance))
    out_shapes.push_back(instance);
  }
}

rpr_shape Surface::getBaseShape()
{
  return m_geometry->getBaseShape();
}

void Surface::clearInstances()
{
  for(rpr_shape instance : m_instances)
  {
    CHECK(rprObjectDelete(instance))
  }
  m_instances.clear();
}

Surface::~Surface(){
  clearInstances();
  if(m_material_instance){
    CHECK(rprObjectDelete(m_material_instance));
  }
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Surface *);
} // namespace anari

