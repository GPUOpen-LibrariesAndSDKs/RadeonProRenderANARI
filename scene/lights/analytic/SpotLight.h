#pragma once

#include "AnalyticLight.h"

namespace anari::rpr{

struct SpotLight : public AnalyticLight
{
public:
  explicit SpotLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void*> &outInstances, mat4 transform) override;

private:
  vec3 m_position{};
  vec3 m_direction{};
  float32 m_innerAngle{};
  float32 m_outerAngle{};
  float32 m_power{};
};

}