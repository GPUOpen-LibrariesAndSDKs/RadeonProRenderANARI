#pragma once
#include "SpatialField.h"

namespace anari::rpr{

struct StructuredRegularSpatialField : public SpatialField
{
public:
  void commit() override;

private:
  void processValue(uvec3 position, Array3D *data);
};

}

