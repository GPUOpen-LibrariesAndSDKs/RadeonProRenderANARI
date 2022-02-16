// Copyright 2020 The Khronos Geometry
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "../SceneObject.h"
#include "../../rpr_common.h"
#include "attributes/Attribute.h"
#include <map>

namespace anari {
namespace rpr {

struct Geometry : public SceneObject
{
  Geometry(rpr_context context, rpr_material_system  m_matsys);

  static Geometry *createInstance(rpr_context context, rpr_material_system  materialSystem, const char *type);

  virtual void getInstances(std::vector<rpr_shape> &out_shapes, mat4 transform) {};

  virtual Attribute *getAttribute(const char *name);

  virtual bool hasAttribute(const char *name);

  rpr_shape getBaseShape();

  ~Geometry() override;

protected:
    rpr_context m_context;
    rpr_material_system m_matsys; // material system is using to create lookup nodes for attributes
    rpr_shape m_base_shape{};

    std::map<std::string, Attribute *> m_attribute_map;

    void clearAttributes();
    static void checkArraySizes(Array1D *array, size_t size, std::runtime_error exception);
};

} // namespace reference

ANARI_TYPEFOR_SPECIALIZATION(rpr::Geometry *, ANARI_GEOMETRY)

} // namespace anari
