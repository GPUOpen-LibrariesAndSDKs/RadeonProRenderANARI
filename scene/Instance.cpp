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

  resetBounds();
  extendBounds(xfmBox(m_transform, m_group->bounds()));
}

void Instance::attachToScene(rpr_scene scene)
{
  m_group->addToScene(scene, m_transform);
}


} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Instance *);

} // namespace anari
