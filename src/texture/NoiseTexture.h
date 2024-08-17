#pragma once
#include "Common.h"

#include "Texture.h"
#include "Perlin.h"

class NoiseTexture : public Texture
{
public:
    NoiseTexture(float scale) : m_scale(scale) {}

    Color value(float u, float v, const Point3& p) const override
    {
        return Color(.5, .5, .5) * (1 + std::sin(m_scale * p.z() + 10 * m_noise.turb(p, 7)));
    }
private:
    Perlin m_noise;
    float m_scale;
};