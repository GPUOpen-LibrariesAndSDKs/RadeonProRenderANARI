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

protected:
  static rpr_image_filter_type processFilter(const std::string& name);
  static rpr_image_wrap_type processWrap(const std::string& name);

  std::string m_input_attribute;
  rpr_image m_image{};
  rpr_image_filter_type m_filter{};
};

}



