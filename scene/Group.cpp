#include "Group.h"
#include "geometry/Surface.h"
#include "lights/Light.h"

namespace anari {
namespace rpr {

Group::Group() { setCommitPriority(COMMIT_PRIORITY_GROUP); }

void Group::commit()
{
  auto surfaces = getParamObject<ObjectArray>("surface");
  auto lights = getParamObject<ObjectArray>("light");
  m_surfaces.clear();
  m_lights.clear();

  resetBounds();

  if(surfaces)
  {
    for (int surface_number = 0; surface_number < surfaces->size(); surface_number++)
    {
      Surface *surface = ((Surface **)surfaces->handles())[surface_number];
      extendBounds(surface->bounds());
      m_surfaces.push_back(surface);
    }
  }

  if(lights)
  {
    for (int light_number = 0; light_number < lights->size(); light_number ++)
    {
      Light *light = ((Light **)lights->handles())[light_number];
      m_lights.push_back(light);
    }
  }
}

void Group::getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform)
{
  for(Surface* surface : m_surfaces){
    surface->getInstances(out_shapes, transform);
  }
}

void Group::getBaseShapes(std::vector<rpr_shape> &outShapes)
{
  for(Surface* surface : m_surfaces){
    outShapes.push_back(surface->getBaseShape());
  }
}

void Group::addLightsToScene(rpr_scene scene)
{
  for(Light *light : m_lights){
    light->addToScene(scene);
  }
}

} // namespace rpr
ANARI_TYPEFOR_DEFINITION(rpr::Group *);
} // namespace anari
