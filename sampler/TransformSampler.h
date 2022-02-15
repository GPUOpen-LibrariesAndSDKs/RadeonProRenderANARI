#pragma once
#include "Sampler.h"
#include "util/TransformNode.h"

namespace anari::rpr{

struct TransformSampler : public Sampler
{
public:
  TransformSampler(rpr_context context, rpr_material_system matsys);
  void commit() override;
  rpr_material_node generateMaterial(Geometry * geometry) override;
  ~TransformSampler() override;

private:
  std::vector<TransformNode *> m_transform_nodes;
  mat4 m_transform{};
  std::string m_input_attribute;

  void clearNodes();
};

}



