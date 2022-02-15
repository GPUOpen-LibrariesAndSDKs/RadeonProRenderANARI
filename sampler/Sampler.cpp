#include "Sampler.h"
#include "image/Image1D.h"
#include "image/Image2D.h"
#include "image/Image3D.h"
#include "TransformSampler.h"

namespace anari{
namespace rpr{

Sampler::Sampler(rpr_context context, rpr_material_system matsys) : m_context(context), m_matsys(matsys) {}

Sampler *Sampler::createInstance(const char *type, rpr_context context, rpr_material_system matsys)
{
  if(strcmp(type, "image1D") == 0){
    return new Image1D(context, matsys);
  }
  if(strcmp(type, "image2D") == 0){
    return new Image2D(context, matsys);
  }
  if(strcmp(type, "image3D") == 0){
    return new Image3D(context, matsys);
  }
  if(strcmp(type, "transform") == 0){
    return new TransformSampler(context, matsys);
  }
  //TODO Primitive sampler
  throw std::runtime_error("cannot create sampler");
}

}

ANARI_TYPEFOR_DEFINITION(rpr::Sampler *);

}
