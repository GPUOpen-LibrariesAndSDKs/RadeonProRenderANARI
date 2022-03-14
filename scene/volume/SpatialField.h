#pragma once
#include "../SceneObject.h"

namespace anari{
namespace rpr{

struct SpatialField : public SceneObject{

  friend struct ScivisVolume;

  SpatialField() = default;
  static SpatialField *createInstance(const char* type);

protected:
  static rpr_image_filter_type processFilter(const std::string &name);

  std::vector<float32> m_grid;
  std::vector<uint32> m_indices;
  uvec3 m_size{};
  vec3 m_position{};
  vec3 m_voxelSize{};
  rpr_image_filter_type m_filterType{};

};

}
ANARI_TYPEFOR_SPECIALIZATION(rpr::SpatialField *, ANARI_SPATIAL_FIELD)
}
