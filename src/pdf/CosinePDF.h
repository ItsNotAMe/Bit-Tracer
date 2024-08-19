#pragma once
#include "Common.h"

#include "PDF.h"
#include "core/ONB.h"

class CosinePDF : public PDF
{
public:
    CosinePDF(const Vec3& w) : m_uvw(w) {}

    float value(const Vec3& direction) const override
    {
        auto cosineTheta = dot(unitVector(direction), m_uvw.w());
        return std::fmax(0, cosineTheta / PI);
    }

    Vec3 generate() const override
    {
        return m_uvw.transform(randomCosineDirection());
    }
private:
    ONB m_uvw;
};