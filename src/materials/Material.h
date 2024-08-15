#pragma once
#include "Common.h"

struct HitRecord;

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const { return false; }
};