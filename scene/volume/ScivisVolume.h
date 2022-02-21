#pragma once
#include "Volume.h"
#include "SpatialField.h"

namespace anari::rpr{

struct ScivisVolume : public Volume
{
public:
  ScivisVolume(rpr_context context, rpr_material_system matsys);
  void commit() override;
  ~ScivisVolume() override;

private:
  void clear();
  mat4 getVolumeTransform() override;

  SpatialField *m_field{};
  rpr_material_node m_density_material{};
  rpr_grid m_grid{};
};

}
