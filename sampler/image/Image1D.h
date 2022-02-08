#pragma once
#include "Image.h"

namespace anari::rpr{

struct Image1D : public Image
{
public:
  Image1D(rpr_context context, rpr_material_system matsys);
  void commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

private:
  rpr_image_wrap_type m_wrap_mode{};
};

}
