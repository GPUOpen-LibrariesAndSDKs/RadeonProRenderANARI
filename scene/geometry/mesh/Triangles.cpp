#include "Triangles.h"

namespace anari::rpr {

Triangles::Triangles(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem) {}

void Triangles::commit()
{
  auto index = getParamObject<Array1D>("primitive.index");
  auto vertex = getParamObject<Array1D>("vertex.position");
  auto color = getParamObject<Array1D>("vertex.color");

  if (!vertex)
    throw std::runtime_error("required 'vertex.position' data array is missed or have incorrect type!");

  calculateBounds(vertex);

  if(index)
  {
    std::size_t num_faces = index->size();
    std::vector<rpr_int> faces(num_faces, 3);
    CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertex->dataAs<vec3>(), vertex->size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int*) index->dataAs<uvec3>(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_shape))
  }
  else  // 'index' array is missed. We should use default indices ((0,1,2), (3,4,5), ...)
  {
    std::vector<uvec3> default_index;
    for (uint32_t i = 0; i < vertex->size(); i += 3)
    {
      default_index.push_back(uvec3(0, 1, 2) + i);
    }
    std::size_t num_faces = default_index.size();
    std::vector<rpr_int> faces(num_faces, 3);
    CHECK(rprContextCreateMesh(m_context, (rpr_float*) vertex->dataAs<vec3>(), vertex->size(), sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int*) default_index.data(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_shape))
  }

  applyColor(color);
  CHECK(rprShapeSetVisibility(m_base_shape, false))  // base shape is always invisible
}


} // namespace anari