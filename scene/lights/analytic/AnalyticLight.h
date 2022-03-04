#pragma once
#include "../Light.h"

namespace anari::rpr{

struct AnalyticLight : public Light
{
public:
  explicit AnalyticLight(rpr_context &context);
  void addToScene(rpr_scene scene) override;

protected:
  static mat4 calculateRotation(vec3 direction, vec3 startDirection=vec3(0,0,-1));
  static float32 calculateSteradianAngle(float32 angle);
};
}


