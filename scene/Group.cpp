#include "Group.h"
#include "geometry/Surface.h"

namespace anari {
namespace rpr {

Group::Group() { setCommitPriority(COMMIT_PRIORITY_GROUP); }

void Group::commit()
{
  auto surfaces = getParamObject<ObjectArray>("surface");
  m_surfaces.clear();

  resetBounds();

  for (int surface_number = 0; surface_number < surfaces->size();
       surface_number++) {
    Surface *surface = ((Surface **)surfaces->handles())[surface_number];
    extendBounds(surface->bounds());
    m_surfaces.push_back(surface);
  }
}

} // namespace rpr
ANARI_TYPEFOR_DEFINITION(rpr::Group *);
} // namespace anari
