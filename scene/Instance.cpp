// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Instance.h"

namespace anari {
namespace rpr {

Instance::Instance()
{
  setCommitPriority(COMMIT_PRIORITY_INSTANCE);
}

void Instance::commit()
{
  m_group = getParamObject<Group>("group");
  m_transform = getParam<mat4x3>("transform", mat4x3(1));

  if(!m_group)
  {
    throw std::runtime_error("'group' is a required parameter!");
  }

  extendBounds(xfmBox(m_transform, m_group->bounds()));
}

void Instance::addToScene(rpr_scene scene)
{
  std::vector<rpr_shape> base_shapes;
  clearInstances();
  m_group->getInstances(m_instances);
  m_group->getBaseShapes(base_shapes);

  for(rpr_shape instance : m_instances)
  {
    CHECK(rprShapeSetTransform(instance, false, value_ptr(m_transform)))
    CHECK(rprSceneAttachShape(scene, instance))
  }

  for(rpr_shape base_shape : base_shapes)
  {
    CHECK(rprSceneAttachShape(scene, base_shape))
  }

  m_group->addLightsToScene(scene);
}

void Instance::clearInstances()
{
  for(rpr_shape instance : m_instances)
  {
    CHECK(rprObjectDelete(instance))
  }
  m_instances.clear();
}

Instance::~Instance()
{
  clearInstances();
}


} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Instance *);

} // namespace anari
