#pragma once
#include "AnalyticLight.h"

namespace anari::rpr{

struct PointLight : public AnalyticLight
{
public:
  explicit PointLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void*> &outInstances, mat4 transform) override;

private:
  vec3 m_position{};
  float32 m_power{};
  float32 m_radius{};

};

}