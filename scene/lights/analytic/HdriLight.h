#pragma once

#include "AnalyticLight.h"

namespace anari::rpr{

struct HdriLight : public AnalyticLight{

public:
  explicit HdriLight(rpr_context context);
  void commit() override;
  void getInstances(std::vector<void*> &outInstances, mat4 transform) override;
  ~HdriLight() override;

private:
  vec3 m_up{};
  vec3 m_direction{};
  rpr_image m_image{};
  float32 m_scale{};

  void clear();

};

}
