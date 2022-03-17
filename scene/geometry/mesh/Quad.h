#pragma once
#include "Mesh.h"

namespace anari::rpr{

struct Quad : public Mesh
{
  inline Quad(rpr_context context, rpr_material_system materialSystem) : Mesh(context, materialSystem){};
  void commit() override;
};

}