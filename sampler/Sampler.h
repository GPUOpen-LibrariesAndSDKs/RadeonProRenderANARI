#pragma once
#include "../Object.h"
#include "../../scene/geometry/Geometry.h"

namespace anari{
namespace rpr{

struct Sampler : public Object
{
public:
  Sampler(rpr_context context, rpr_material_system matsys);
  Sampler *createInstance(char *type, rpr_context context, rpr_material_system matsys);
  inline virtual rpr_material_node generateMaterial(Geometry *geometry) {return nullptr;};
  ~Sampler() override;

protected:
  rpr_context m_context;
  rpr_material_system m_matsys;
  std::vector<rpr_material_node> m_instances;

  void clearInstances();
};

} // namespace rpr

ANARI_TYPEFOR_SPECIALIZATION(rpr::Sampler *, ANARI_SAMPLER);
}