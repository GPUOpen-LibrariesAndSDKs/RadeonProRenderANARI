#pragma once
#include "Material.h"

namespace anari::rpr{

struct MatteMaterial : public Material{

    explicit MatteMaterial(rpr_material_system);

    void commit() override;

    ~MatteMaterial() override;

    rpr_material_node generateMaterial(Geometry *geometry) override;

private:
    vec3 m_diffuse_color;
    std::string m_diffuse_color_attribute;

    float m_opacity;
    std::string m_opacity_attribute;
    rpr_material_node m_transparency_node{};
};

}//namespace anari





