/**********************************************************************
Copyright 2022 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

#include "QuadLight.h"

namespace anari::rpr {

QuadLight::QuadLight(rpr_context &context, rpr_material_system &matsys) : Light(context)
{
  m_matsys = matsys;
}

void QuadLight::commit()
{
  Light::commit();
  clear();

  auto position  = getParam<vec3>("position", vec3(0));
  auto edge1     = getParam<vec3>("edge1", vec3(1, 0, 0));
  auto edge2     = getParam<vec3>("edge2", vec3(0, 1, 0));
  auto intensity = getParam<float32>("intensity", 1.f);
  auto power     = getParam<float32>("power", 1.f);
  auto radiance  = getParam<float32>("radiance", 1.f);
  auto side      = getParam<std::string>("side", "front");

  float32 resultPower;
  float32 area = length(cross(edge1, edge2));
  if (hasParam("intensity") || !(hasParam("power") || !hasParam("radiance")))
  {
    resultPower = intensity / area;
  } else if (hasParam("power") || !hasParam("radiance"))
  {
    resultPower = power / area;
  } else
  {
    resultPower = radiance;
  }

  m_vertex.clear();
  m_vertex.push_back(position);
  m_vertex.push_back(position + edge1);
  m_vertex.push_back(position + edge2);
  m_vertex.push_back(position + edge1 + edge2);

  vec3 normal = normalize(cross(edge1, edge2));
  if (strcmp(side.c_str(), "back") == 0)
  {
    normal *= -1;
  }
  rpr_uint mode = strcmp(side.c_str(), "both") == 0 ? RPR_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED
                                                    : RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED;

  m_normal.clear();
  m_normal.push_back(m_vertex[0] + normal);
  m_normal.push_back(m_vertex[1] + normal);
  m_normal.push_back(m_vertex[2] + normal);
  m_normal.push_back(m_vertex[3] + normal);

  CHECK(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &m_material))
  CHECK(rprMaterialNodeSetInputFByKey(m_material, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR,
      m_color.r * resultPower, m_color.g * resultPower, m_color.b * resultPower, 1.f))
  CHECK(
      rprMaterialNodeSetInputFByKey(m_material, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 1.f, 1.f, 1.f, 1.f))
  CHECK(rprMaterialNodeSetInputUByKey(m_material, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, mode))
}

void QuadLight::attachToScene(rpr_scene scene, mat4 transform)
{
  rpr_shape instance;
  // create new mesh per instance because emissive material assigned to rpr instance produces core crash
  CHECK(rprContextCreateMesh(m_context, (rpr_float *)m_vertex.data(), 4, sizeof(rpr_float) * 3,
      (rpr_float *)m_normal.data(), 4, sizeof(rpr_float) * 3, nullptr, 0, 0, m_index, sizeof(rpr_int),
      m_index, sizeof(rpr_int), nullptr, 0, m_faces, 1, &instance))
  CHECK(rprShapeSetTransform(instance, false, value_ptr(transform)))
  CHECK(rprShapeSetMaterial(instance, m_material))
  CHECK(rprShapeSetVisibilityFlag(instance, RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG, m_visible))
  CHECK(rprShapeSetVisibilityFlag(instance, RPR_SHAPE_VISIBILITY_SHADOW, m_visible))
  CHECK(rprShapeSetVisibilityFlag(instance, RPR_SHAPE_VISIBILITY_LIGHT, m_visible))
  m_instances.push_back(instance);
  CHECK(rprSceneAttachShape(scene, instance))
}

void QuadLight::clear()
{
  if (m_material)
  {
    CHECK(rprObjectDelete(m_material))
    m_material = nullptr;
  }
}

QuadLight::~QuadLight()
{
  clear();
}

} // namespace anari::rpr
