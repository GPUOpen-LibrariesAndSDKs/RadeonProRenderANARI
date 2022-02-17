#pragma once
#include "Sampler.h"
#include "../scene/geometry/attributes/PerShapeAttribute.h"

namespace anari::rpr{

struct PrimitiveSampler : public Sampler
{
public:
  PrimitiveSampler(rpr_context context, rpr_material_system matsys);
  void commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;
  ~PrimitiveSampler() override;
private:
  PerShapeAttribute *m_attribute{};
};


}


