#pragma once
#include "Common.h"

#include "Material.h"
#include "Hittable.h"

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo) : m_albedo(albedo) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;
private:
    Color m_albedo;
};