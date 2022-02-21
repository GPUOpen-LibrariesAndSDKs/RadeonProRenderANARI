#pragma once
#include "../SceneObject.h"

namespace anari{
namespace rpr{

struct Volume : public SceneObject{

public:
  Volume(rpr_context context, rpr_material_system matsys);
  static Volume *createInstance(const char *type, rpr_context context, rpr_material_system matsys);
  void getInstances(std::vector<rpr_shape> &out_shapes, mat4 externalTransform);
  void commit() override;
  void addToScene(rpr_scene scene) override;
  ~Volume() override;

protected:
  virtual mat4 getVolumeTransform(){return mat4(1);};

  rpr_context m_context;
  rpr_material_system m_matsys;
  rpr_material_node m_volume_material{};

private:
  void clearInstances();
  void clear();

  std::vector<rpr_shape> m_instances;
};

}

ANARI_TYPEFOR_SPECIALIZATION(rpr::Volume *, ANARI_VOLUME)

}

