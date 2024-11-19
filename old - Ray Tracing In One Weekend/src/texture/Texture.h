#pragma once
#include "Common.h"

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color value(float u, float v, const Point3& p) const = 0;
};