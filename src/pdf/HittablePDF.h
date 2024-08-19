#pragma once
#include "Common.h"

#include "PDF.h"
#include "hittable/Hittable.h"

class HittablePDF : public PDF
{
public:
    HittablePDF(const Hittable& objects, const Point3& origin) : m_objects(objects), m_origin(origin) {}

    float value(const Vec3& direction) const override
    {
        return m_objects.pdfValue(m_origin, direction);
    }

    Vec3 generate() const override
    {
        return m_objects.random(m_origin);
    }
private:
    const Hittable& m_objects;
    Point3 m_origin;
};