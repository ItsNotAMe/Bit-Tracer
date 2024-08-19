#pragma once
#include "Common.h"

#include "PDF.h"

class SpherePDF : public PDF
{
public:
    SpherePDF() {}

    float value(const Vec3& direction) const override
    {
        return 1 / (4 * PI);
    }

    Vec3 generate() const override
    {
        return randomUnitVector();
    }
};