#pragma once

#include "../rpr_common.h"
#include "SceneObject.h"

namespace anari {
namespace rpr {

struct Surface;

struct Group : public SceneObject
{
  Group();

  void commit() override;

private:
  std::vector<Surface *> m_surfaces;
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::Group *, ANARI_GROUP)
} // namespace anari
