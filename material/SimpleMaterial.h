#pragma once
#include "Material.h"

namespace anari::rpr{

struct SimpleMaterial : public Material{

    explicit SimpleMaterial(rpr_material_system);

    void commit() override;

private:
    rpr_material_node generateMaterial(rpr_material_node vertex_color) override;

    vec3 m_diffuse_color{vec3(0.99f)};
    float m_transparency{0.f};
};

}//namespace anari





