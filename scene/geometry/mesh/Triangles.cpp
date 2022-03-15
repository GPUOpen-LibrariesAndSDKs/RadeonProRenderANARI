#include "Triangles.h"

namespace anari::rpr {

Triangles::Triangles(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem) {}

void Triangles::commit()
{
  auto index_data = getParamObject<Array1D>("primitive.index");
  auto normal_data = getParamObject<Array1D>("vertex.normal");
  auto vertex_data = getParamObject<Array1D>("vertex.position");

  if (!vertex_data)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

  calculateBounds(vertex_data);

  // process indices
  rpr_int *index;
  std::size_t num_faces;
  std::vector<uvec3> default_index;

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

  //process normals
  rpr_float *normal = normal_data ? (rpr_float *) normal_data->dataAs<vec3>() : nullptr;
  size_t num_normals = normal_data ? normal_data->size() : 0;
  size_t normal_stride = normal_data ? sizeof(rpr_float) * 3 : 0;
  rpr_int *normal_indices = normal_data ? index : nullptr;
  size_t nidx_stride = normal_data ? sizeof(rpr_int) : 0;


  std::vector<rpr_int> faces(num_faces, 3);
  CHECK(rprContextCreateMesh(m_context, (rpr_float*)vertex_data->dataAs<vec3>(), vertex_data->size(), sizeof(rpr_float) * 3,
                             normal, num_normals, normal_stride, nullptr, 0, 0,
                             index, sizeof(rpr_int), normal_indices, nidx_stride, nullptr, 0,
                             faces.data(), num_faces, &m_base_shape))

  processAttributes(vertex_data);

  CHECK(rprShapeSetVisibility(m_base_shape, false))  // base shape is always invisible
}


} // namespace anari
