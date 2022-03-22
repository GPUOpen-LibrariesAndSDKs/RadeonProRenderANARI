#pragma once
#include "../SceneObject.h"

namespace anari{
namespace rpr{

struct Volume : public SceneObject{

public:
  Volume(rpr_context context, rpr_material_system matsys);
  static Volume *createInstance(const char *type, rpr_context context, rpr_material_system matsys);
  void commit() override;
  void attachToScene(rpr_scene scene, mat4 transform = mat4(1));
  ~Volume() override;

protected:
  virtual mat4 getVolumeTransform(){return mat4(1);};

  rpr_context m_context;
  rpr_material_system m_matsys;
  rpr_material_node m_volume_material{};

private:
  void clear();

  std::vector<rpr_shape> m_instances;
};

}

ANARI_TYPEFOR_SPECIALIZATION(rpr::Volume *, ANARI_VOLUME)

}

