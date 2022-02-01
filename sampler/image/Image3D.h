#pragma once
#include "Image.h"

namespace anari::rpr{

struct Image3D : public Image
{
public:
  Image3D(rpr_context context, rpr_material_system matsys);
  void commit() override;
  rpr_material_node generateMaterial(Geometry *geometry) override;

private:
  rpr_int m_wrap_mode_u{};
  rpr_int m_wrap_mode_v{};
  rpr_int m_wrap_mode_w{};
};

}
