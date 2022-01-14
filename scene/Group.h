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

  void getInstances(std::vector<rpr_shape> &out_shapes);

  void getBaseShapes(std::vector<rpr_shape> &outShapes);

private:
  std::vector<Surface *> m_surfaces;
};

} // namespace rpr
ANARI_TYPEFOR_SPECIALIZATION(rpr::Group *, ANARI_GROUP)
} // namespace anari
