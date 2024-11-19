#pragma once
#include "Common.h"

#include "Texture.h"
#include "SolidColor.h"

class CheckerTexture : public Texture
{
public:
    CheckerTexture(float scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
        : m_invScale(1.0 / scale), m_even(even), m_odd(odd) {}
    CheckerTexture(float scale, const Color& even, const Color& odd)
        : CheckerTexture(scale, std::make_shared<SolidColor>(even), std::make_shared<SolidColor>(odd)) {}

    Color value(float u, float v, const Point3& p) const override
    {
        int num = std::floor(p.x() * m_invScale) + std::floor(p.y() * m_invScale) + std::floor(p.z() * m_invScale);
        if (num % 2 == 0)
            return m_even->value(u, v, p);
        return m_odd->value(u, v, p);
    }
private:
    float m_invScale;
    std::shared_ptr<Texture> m_even;
    std::shared_ptr<Texture> m_odd;
};