#include "SpatialField.h"
#include "StucturedRegularSpatialField.h"

namespace anari{
namespace rpr{

const static std::map<std::string, rpr_image_filter_type> image_filter_types = {
    {"nearest", RPR_IMAGE_FILTER_TYPE_NEAREST},
    {"linear", RPR_IMAGE_FILTER_TYPE_LINEAR}};


SpatialField *SpatialField::createInstance(const char *type)
{
  if(strcmp(type, "structuredRegular") == 0)
  {
    return new StructuredRegularSpatialField();
  }

  throw std::runtime_error("could not create spatial field");
}


rpr_image_filter_type SpatialField::processFilter(const std::string &name)
{
  if(image_filter_types.find(name) != image_filter_types.end()){
    return image_filter_types.find(name)->second;
  }
  throw std::runtime_error("unknown filter type");
}

}

ANARI_TYPEFOR_DEFINITION(rpr::SpatialField);

}
