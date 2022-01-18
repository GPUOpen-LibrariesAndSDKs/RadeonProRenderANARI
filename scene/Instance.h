#pragma once

#include "Group.h"
#include "SceneObject.h"
#include "../rpr_common.h"

namespace anari {
namespace rpr {

struct Group;

struct Instance : public SceneObject
{
  Instance();

  void commit() override;

  void addToScene(rpr_scene scene) override;

  ~Instance() override;

private:

  IntrusivePtr<Group> m_group;
  mat4 m_transform{};

  std::vector<rpr_shape> m_instances;

  void clearInstances();

};
} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Instance *, ANARI_INSTANCE);

} // namespace anari
