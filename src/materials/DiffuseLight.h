#pragma once
#include "Common.h"

#include "Material.h"
#include "texture/Texture.h"
#include "texture/SolidColor.h"
#include "hittable/Hittable.h"

class DiffuseLight : public Material
{
public:
    DiffuseLight(const Color& emit) : m_tex(std::make_shared<SolidColor>(emit)) {}
    DiffuseLight(std::shared_ptr<Texture> texture) : m_tex(texture) {}

    Color emitted(double u, double v, const Point3& p) const override { return m_tex->value(u, v, p); }
private:
    std::shared_ptr<Texture> m_tex;
};