#pragma once
#include "Mesh.h"

namespace anari::rpr{

struct Triangles : public Mesh
{
  inline Triangles(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem){};
  void commit() override;
};

}

