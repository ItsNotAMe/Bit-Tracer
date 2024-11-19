#pragma once
#include "Common.h"

#include "Hittable.h"
#include "materials/Material.h"
#include "materials/Isotropic.h"
#include "texture/Texture.h"

class ConstantMedium : public Hittable
{
public:
    ConstantMedium(std::shared_ptr<Hittable> boundary, float density, std::shared_ptr<Texture> tex);
    ConstantMedium(std::shared_ptr<Hittable> boundary, float density, const Color& albedo);
    
    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override;

    AABB boundingBox() const override { return m_boundary->boundingBox(); }
private:
    std::shared_ptr<Hittable> m_boundary;
    double m_negInvDensity;
    std::shared_ptr<Material> m_phaseFunction;
};