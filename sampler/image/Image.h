#pragma once
#include "../Sampler.h"
#include <map>

namespace anari::rpr{

struct Image : public Sampler
{
public:
  Image(rpr_context context, rpr_material_system matsys);
  void commit() override;
  ~Image() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;
  void clearInstances() override;

protected:
  static rpr_image_filter_type processFilter(const std::string& name);
  static rpr_image_wrap_type processWrap(const std::string& name);

  rpr_material_node applyTransformNode(mat4 transform, rpr_material_node input);

  std::string m_input_attribute;
  mat4 m_input_transform{};
  mat4 m_output_transform{};
  rpr_image m_image{};
  rpr_image_filter_type m_filter{};

  std::vector<TransformNode *> m_transform_nodes;
};

}



