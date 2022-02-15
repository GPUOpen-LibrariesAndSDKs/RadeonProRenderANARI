#include "PrimitiveSampler.h"
#include "../scene/geometry/primitives/Primitives.h"

namespace anari::rpr{

PrimitiveSampler::PrimitiveSampler(rpr_context context, rpr_material_system matsys) : Sampler(context, matsys){}

void PrimitiveSampler::commit()
{
  delete m_attribute;

  auto *data = getParamObject<Array1D>("array");
  if(!data)
  {
    throw std::runtime_error("'array' is a required parameter");
  }
  std::vector<int> index;
  std::vector<vec4> processedData;

  index.reserve(data->size());
  for(int indexNumber=0; indexNumber<data->size(); indexNumber++)
  {
    index.push_back(indexNumber);
  }
  Primitives::processAttributeArray(data, index, processedData);
  m_attribute = new PerShapeAttribute(m_context, m_matsys, processedData.size(), (float*) processedData.data());
}

rpr_material_node PrimitiveSampler::generateMaterial(Geometry *geometry)
{
    return m_attribute->getMaterial();
}

PrimitiveSampler::~PrimitiveSampler(){
    delete m_attribute;
}

}