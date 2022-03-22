#pragma once
#include "Camera.h"

namespace anari::rpr{

struct Orthographic : public Camera{
public:
  explicit Orthographic(rpr_camera camera);

  void commit() override;
};

}
