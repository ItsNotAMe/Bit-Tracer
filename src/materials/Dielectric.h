#pragma once
#include "Common.h"

#include "Material.h"
#include "Hittable.h"

class Dielectric : public Material
{
public:
    Dielectric(float refractionIndex) : m_refractionIndex(refractionIndex) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;
private:
    static float reflectance(double cosine, double refractionIndex);
private:
    float m_refractionIndex;
};