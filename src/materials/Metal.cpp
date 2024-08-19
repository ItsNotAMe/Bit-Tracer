#include "Metal.h"

bool Metal::scatter(const Ray& rayIn, const HitRecord& rec, ScatterRecord& srec) const
{
    Vec3 reflected = reflect(rayIn.direction(), rec.Normal);
    reflected = unitVector(reflected) + (m_fuzz * randomUnitVector());

    srec.Attenuation = m_albedo;
    srec.PDF = nullptr;
    srec.SkipPDF = true;
    srec.SkipPDFRay = Ray(rec.Point, reflected, rayIn.time());
    return true;
}