#pragma once
#include "../Sampler.h"

namespace anari::rpr{

struct Image : public Sampler
{
public:
  Image(rpr_context context, rpr_material_system matsys);
  void commit() override;
  ~Image() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

protected:
  static rpr_int processFilter(const std::string& name);
  static rpr_int processWrap(const std::string& name);

  std::string m_input_attribute;
  rpr_image m_image;
  rpr_int m_filter;
};

}



