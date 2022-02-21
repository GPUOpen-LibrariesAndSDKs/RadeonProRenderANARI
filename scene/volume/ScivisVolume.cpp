#include "ScivisVolume.h"

namespace anari::rpr{

ScivisVolume::ScivisVolume(rpr_context context, rpr_material_system matsys) : Volume(context, matsys){}

void ScivisVolume::commit()
{
  clear();
  Volume::commit();

  m_field = getParamObject<SpatialField>("field");
  if(!m_field) throw std::runtime_error("'field' is a required parameter");

  extendBounds(m_field->bounds());

  CHECK(rprContextCreateGrid(m_context, &m_grid, m_field->m_size.x, m_field->m_size.y, m_field->m_size.z,
                                 (rpr_int *)m_field->m_indices.data(), m_field->m_indices.size() / 3, RPR_GRID_INDICES_TOPOLOGY_XYZ_U32,
                                 (rpr_float *)m_field->m_grid.data(), sizeof(m_field->m_grid[0]) * m_field->m_grid.size(), 0))

  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_VOLUME, &m_volume_material))
  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_GRID_SAMPLER, &m_density_material))
  CHECK(rprMaterialNodeSetInputGridDataByKey(m_density_material, RPR_MATERIAL_INPUT_DATA, m_grid))
  CHECK(rprMaterialNodeSetInputNByKey(m_volume_material, RPR_MATERIAL_INPUT_DENSITYGRID, m_density_material))
  CHECK(rprMaterialNodeSetInputFByKey(m_volume_material, RPR_MATERIAL_INPUT_DENSITY, 0.03, 0.03, 0.03, 1))
  CHECK(rprMaterialNodeSetInputFByKey(m_volume_material, RPR_MATERIAL_INPUT_COLOR, 1.f, 1.f, 1.f, 1))
}

mat4 ScivisVolume::getVolumeTransform()
{
  mat4 volumeTranslation = translate(mat4(1), (m_field->bounds().lower + m_field->bounds().upper) * 0.5f);

  // rotations and minus in scale are for compensation of different coordinate systems
  mat4 volumeScale = scale(mat4(1), -(m_field->bounds().upper - m_field->bounds().lower));
  mat4 volumeRotation = rotate(mat4(1), radians(90.f), vec3(0,1,0));
  volumeRotation = rotate(volumeRotation, radians(180.f), vec3(1,0,0));
  mat4 transform = volumeScale * volumeRotation * volumeTranslation;

  return transform;
}

void ScivisVolume::clear()
{
  if(m_density_material){
    CHECK(rprObjectDelete(m_density_material))
  }
  if(m_grid){
    CHECK(rprObjectDelete(m_grid))
  }
}

ScivisVolume::~ScivisVolume(){
  clear();
}

}
