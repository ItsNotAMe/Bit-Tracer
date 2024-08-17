#pragma once
#include "Common.h"

#include "Texture.h"

class SolidColor : public Texture
{
public:
    SolidColor(const Color& albedo) : m_albedo(albedo) {}
    SolidColor(float red, float green, float blue) : SolidColor(Color(red, green, blue)) {}

    Color value(float u, float v, const Point3& p) const override
    {
        return m_albedo;
    }
private:
    Color m_albedo;
};