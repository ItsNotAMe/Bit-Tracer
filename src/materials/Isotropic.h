#pragma once
#include "Common.h"

#include "Material.h"
#include "texture/SolidColor.h"
#include "hittable/Hittable.h"

class Isotropic : public Material
{
public:
    Isotropic(const Color& albedo) : m_tex(std::make_shared<SolidColor>(albedo)) {}
    Isotropic(std::shared_ptr<Texture> tex) : m_tex(tex) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        scattered = Ray(rec.Point, randomUnitVector(), rayIn.time());
        attenuation = m_tex->value(rec.u, rec.v, rec.Point);
        return true;
    }

private:
    std::shared_ptr<Texture> m_tex;
};