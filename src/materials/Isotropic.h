#pragma once
#include "Common.h"

#include "Material.h"
#include "texture/SolidColor.h"
#include "hittable/Hittable.h"
#include "pdf/SpherePDF.h"

class Isotropic : public Material
{
public:
    Isotropic(const Color& albedo) : m_tex(std::make_shared<SolidColor>(albedo)) {}
    Isotropic(std::shared_ptr<Texture> tex) : m_tex(tex) {}

    bool scatter(const Ray& rayIn, const HitRecord& rec, ScatterRecord& srec) const override
    {
        srec.Attenuation = m_tex->value(rec.u, rec.v, rec.Point);
        srec.PDF = std::make_shared<SpherePDF>();
        srec.SkipPDF = false;
        return true;
    }

    float scatteringPDF(const Ray& rayIn, const HitRecord& rec, const Ray& scattered) const override
    {
        return 1 / (4 * PI);
    }
private:
    std::shared_ptr<Texture> m_tex;
};