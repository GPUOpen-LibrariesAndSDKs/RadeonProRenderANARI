#pragma once
#include "Material.h"

namespace anari::rpr{

struct SimpleMaterial : public Material{

    explicit SimpleMaterial(rpr_material_system);

    void commit() override;

private:
    rpr_material_node generateMaterial(rpr_material_node vertex_color) override;

    vec3 m_diffuse_color{};
    float m_transparency{};
};

}//namespace anari





