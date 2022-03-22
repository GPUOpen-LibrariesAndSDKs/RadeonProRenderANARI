#include "Group.h"
#include "geometry/Surface.h"
#include "lights/Light.h"
#include "volume/Volume.h"

namespace anari {
namespace rpr {

Group::Group() { setCommitPriority(COMMIT_PRIORITY_GROUP); }

void Group::commit()
{
  auto surfaces = getParamObject<ObjectArray>("surface");
  auto lights = getParamObject<ObjectArray>("light");
  auto volumes = getParamObject<ObjectArray>("volume");
  m_surfaces.clear();
  m_volumes.clear();
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

  if(volumes)
  {
    for (int volume_number = 0; volume_number < volumes->size(); volume_number++)
    {
      Volume *volume = ((Volume **)volumes->handles())[volume_number];
      extendBounds(volume->bounds());
      m_volumes.push_back(volume);
    }
  }
}

void Group::attachToScene(rpr_scene scene, mat4 transform)
{
  for(Light *light : m_lights){
    light->attachToScene(scene, transform);
  }
  for(Surface *surface : m_surfaces){
    surface->attachToScene(scene, transform);
  }
  for(Volume *volume : m_volumes){
    volume->attachToScene(scene, transform);
  }
}

} // namespace rpr
ANARI_TYPEFOR_DEFINITION(rpr::Group *);
} // namespace anari
