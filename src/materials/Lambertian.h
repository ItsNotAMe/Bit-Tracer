#pragma once
#include "Common.h"

#include "Material.h"
#include "texture/Texture.h"
#include "texture/SolidColor.h"
#include "hittable/Hittable.h"

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo) : m_tex(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> texture) : m_tex(texture) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;
private:
    std::shared_ptr<Texture> m_tex;
};