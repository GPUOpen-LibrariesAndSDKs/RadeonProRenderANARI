#pragma once
#include "Volume.h"
#include "SpatialField.h"
#include "../../sampler/image/Image1D.h"

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
  static float32 processDensity(float32 index, Array1D* densityLookup, rpr_image_filter_type filter);

  rpr_image m_color_map{};
  rpr_material_node m_color_material{};
  rpr_material_node m_color_grid_material{};
  rpr_material_node m_density_grid_material{};
  rpr_grid m_color_grid{};
  rpr_grid m_density_grid{};
  mat4 m_transform{};
};

}
