#include "Triangles.h"

namespace anari::rpr {

Triangles::Triangles(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem) {}

void Triangles::commit()
{
  auto index_data = getParamObject<Array1D>("primitive.index");
  auto vertex_data = getParamObject<Array1D>("vertex.position");
  auto color_data = getParamObject<Array1D>("vertex.color");
  auto attribute0_data = getParamObject<Array1D>("vertex.attribute0");
  auto attribute1_data = getParamObject<Array1D>("vertex.attribute1");
  auto attribute2_data = getParamObject<Array1D>("vertex.attribute2");
  auto attribute3_data = getParamObject<Array1D>("vertex.attribute3");

  if (!vertex_data)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

  checkArraySizes(color_data, vertex_data->size(), std::runtime_error("'vertex.position' and 'vertex.color' sizes are incompatible"));
  checkArraySizes(attribute0_data, vertex_data->size(), std::runtime_error("'vertex.position' and 'vertex.attribute0' sizes are incompatible"));
  checkArraySizes(attribute1_data, vertex_data->size(), std::runtime_error("'vertex.position' and 'vertex.attribute1' sizes are incompatible"));
  checkArraySizes(attribute2_data, vertex_data->size(), std::runtime_error("'vertex.position' and 'vertex.attribute2' sizes are incompatible"));
  checkArraySizes(attribute3_data, vertex_data->size(), std::runtime_error("'vertex.position' and 'vertex.attribute3' sizes are incompatible"));

  calculateBounds(vertex_data);

  rpr_int *index;
  std::size_t num_faces;
  std::vector<uvec3> default_index;

  // process indices
  if(index_data)
  {
    num_faces = index_data->size();
    index = (rpr_int *) index_data->dataAs<uvec3>();
  }
  else  // 'index_data' array is missed. We should use default indices ((0,1,2), (3,4,5), ...)
  {
    for (uint32_t i = 0; i < vertex_data->size(); i += 3)
    {
      default_index.push_back(uvec3(0, 1, 2) + i);
    }
    num_faces = default_index.size();
    index = (rpr_int *)default_index.data();
  }

  std::vector<rpr_int> faces(num_faces, 3);
  CHECK(rprContextCreateMesh(m_context, (rpr_float*)vertex_data->dataAs<vec3>(), vertex_data->size(), sizeof(rpr_float) * 3,
                             nullptr, 0, 0, nullptr, 0, 0,
                             index, sizeof(rpr_int), nullptr, 0, nullptr, 0,
                             faces.data(), num_faces, &m_base_shape))

  applyAttribute(attribute0_data, 0);
  applyAttribute(attribute1_data, 1);
  applyAttribute(attribute2_data, 2);
  applyAttribute(attribute3_data, 3);
  applyAttribute(color_data, 4);

  CHECK(rprShapeSetVisibility(m_base_shape, false))  // base shape is always invisible
}


} // namespace anari
