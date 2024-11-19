#include "Lambertian.h"

#include "core/ONB.h"
#include "pdf/CosinePDF.h"

bool Lambertian::scatter(const Ray& rayIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = m_tex->value(rec.u, rec.v, rec.Point);
    srec.PDF = std::make_shared<CosinePDF>(rec.Normal);
    srec.SkipPDF = false;
    return true;
}

float Lambertian::scatteringPDF(const Ray& rayIn, const HitRecord& rec, const Ray& scattered) const
{
    float cosTheta = dot(rec.Normal, unitVector(scattered.direction()));
    return cosTheta < 0 ? 0 : cosTheta / PI;
}