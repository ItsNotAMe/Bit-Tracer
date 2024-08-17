#pragma once
#include "Common.h"

struct HitRecord;

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const { return false; }

    virtual Color emitted(double u, double v, const Point3& p) const { return Color(0, 0, 0); }
};