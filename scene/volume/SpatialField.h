#pragma once
#include "../SceneObject.h"

namespace anari{
namespace rpr{

struct SpatialField : public SceneObject{

};

}

ANARI_TYPEFOR_SPECIALIZATION(rpr::SpatialField, ANARI_SPATIAL_FIELD)

}
