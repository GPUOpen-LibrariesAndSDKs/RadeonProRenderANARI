#pragma once
#include "../Light.h"

namespace anari::rpr{

struct AnalyticLight : public Light
{
public:
  explicit AnalyticLight(rpr_context &context);
  void attachToScene(rpr_scene scene, mat4 transform) override;

protected:
  static float32 calculateSteradianAngle(float32 angle);
};
}


