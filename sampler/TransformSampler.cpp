#include "TransformSampler.h"

namespace anari::rpr{

TransformSampler::TransformSampler(rpr_context context, rpr_material_system matsys) : Sampler(context, matsys){}

void TransformSampler::commit()
{
  clearNodes();
  m_transform = getParam<mat4x4>("transform", mat4(1));
  m_input_attribute = getParam<std::string>("inAttribute", "attribute0");
}

rpr_material_node TransformSampler::generateMaterial(Geometry *geometry)
{
  if(!geometry->hasAttribute(m_input_attribute.c_str())){
    throw std::runtime_error("attribute is not supported by geometry");
  }
  auto *node = new TransformNode(m_matsys, m_transform, geometry->getAttribute(m_input_attribute.c_str())->getMaterial());
  m_transform_nodes.push_back(node);
  return node->getMaterial();
}

void TransformSampler::clearNodes()
{
  for(TransformNode *node : m_transform_nodes)
  {
    delete node;
  }
  m_transform_nodes.clear();
}

TransformSampler::~TransformSampler(){
  clearNodes();
}

}
