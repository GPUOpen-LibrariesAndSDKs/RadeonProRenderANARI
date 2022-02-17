#pragma once
#include "../Object.h"
#include "../../scene/geometry/Geometry.h"
#include "util/TransformNode.h"

namespace anari{
namespace rpr{

struct Sampler : public Object
{
public:
  Sampler(rpr_context context, rpr_material_system matsys);
  static Sampler *createInstance(const char *type, rpr_context context, rpr_material_system matsys);
  inline virtual rpr_material_node generateMaterial(Geometry *geometry) {return nullptr;};

protected:
  rpr_context m_context;
  rpr_material_system m_matsys;
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Sampler *, ANARI_SAMPLER);
ANARI_TYPEFOR_SPECIALIZATION(glm::mat4, ANARI_FLOAT32_MAT4)
}
