// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../rpr_common.h"
#include "../SceneObject.h"

namespace anari {
namespace rpr {

struct Light : public SceneObject
{
public:
  explicit Light(rpr_context &context);

  static Light *createInstance(rpr_context &context, rpr_material_system matsys, const char *type);

  void commit() override;

  inline void addToScene(rpr_scene scene) override{};

  virtual inline void getInstances(std::vector<void *> &outInstances, mat4 transform){};

  ~Light() override;

  protected:
    rpr_context m_context;
    std::vector<void *> m_instances; // this could be rpr_light or rpr_shape
    void * m_light = nullptr;

    vec3 m_color{};
    bool m_visible{};

  private:
    void clear();
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Light *, ANARI_LIGHT)

} // namespace anari
