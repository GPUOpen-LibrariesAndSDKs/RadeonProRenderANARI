#pragma once
#include "AnalyticLight.h"

namespace anari::rpr{

struct RingLight : public AnalyticLight
{
public:
  explicit RingLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void*> &outInstances, mat4 transform) override;

private:
  vec3 m_position{};
  vec3 m_direction{};
  float32 m_outerAngle{};
  float32 m_innerAngle{};
  float32 m_power{};
  float32 m_radius{};
};

}
