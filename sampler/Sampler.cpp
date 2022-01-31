#include "Sampler.h"
#include "image/Image1D.h"

namespace anari{
namespace rpr{

Sampler::Sampler(rpr_context context, rpr_material_system matsys) : m_context(context), m_matsys(matsys) {}

Sampler *Sampler::createInstance(char *type, rpr_context context, rpr_material_system matsys)
{
  if(strcmp(type, "image1D") == 0){
    return new Image1D(context, matsys);
  }
  throw std::runtime_error("cannot create sampler");
}

void Sampler::clearInstances()
{
  for(rpr_material_node node : m_instances){
    CHECK(rprObjectDelete(node))
  }
  m_instances.clear();
}

Sampler::~Sampler()
{
  clearInstances();
}

}

ANARI_TYPEFOR_DEFINITION(rpr::Sampler *);

}