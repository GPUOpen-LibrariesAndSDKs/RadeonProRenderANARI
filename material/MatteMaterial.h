#pragma once
#include "Material.h"
#include "../sampler/Sampler.h"

namespace anari::rpr{

struct MatteMaterial : public Material{

    explicit MatteMaterial(rpr_material_system);

    void commit() override;

    ~MatteMaterial() override;

    rpr_material_node generateMaterial(Geometry *geometry) override;

private:
    vec3 m_diffuse_color;
    std::string m_diffuse_color_attribute;
    IntrusivePtr<Sampler> m_diffuse_color_sampler;

    float m_opacity;
    std::string m_opacity_attribute;
    IntrusivePtr<Sampler> m_opacity_sampler;
    rpr_material_node m_transparency_node{};
};

}//namespace anari





