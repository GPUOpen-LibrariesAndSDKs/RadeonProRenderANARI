#include "Triangles.h"

namespace anari::rpr {

Triangles::Triangles(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem) {}

void Triangles::commit()
{
  auto index_data = getParamObject<Array1D>("primitive.index");
  auto vertex_data = getParamObject<Array1D>("vertex.position");

  if (!vertex_data)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

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

  processAttributes(vertex_data);

  CHECK(rprShapeSetVisibility(m_base_shape, false))  // base shape is always invisible
}


} // namespace anari
