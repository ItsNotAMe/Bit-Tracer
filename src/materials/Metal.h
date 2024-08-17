#pragma once
#include "Common.h"

#include "Material.h"
#include "hittable/Hittable.h"

class Metal : public Material
{
public:
    Metal(const Color& albedo, float fuzz) : m_albedo(albedo), m_fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;
private:
    Color m_albedo;
    float m_fuzz;
};