#pragma once
#include "AnalyticLight.h"

namespace anari::rpr{

struct DirectionalLight : public AnalyticLight
{

public:
  explicit DirectionalLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void*> &outInstances, mat4 transform) override;

private:
  vec3 m_direction{};
  float32 m_angularDiameter{};
  float32 m_power{};

};

}
