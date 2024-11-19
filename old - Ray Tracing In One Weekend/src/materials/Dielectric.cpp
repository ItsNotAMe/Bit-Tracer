#include "Dielectric.h"

bool Dielectric::scatter(const Ray& rayIn, const HitRecord& rec, ScatterRecord& srec) const
{
    srec.Attenuation = Color(1.0, 1.0, 1.0);
    srec.PDF = nullptr;
    srec.SkipPDF = true;
    double ri = rec.FrontFace ? (1.0 / m_refractionIndex) : m_refractionIndex;

    Vec3 unitDirection = unitVector(rayIn.direction());
    double cosTheta = std::fmin(dot(-unitDirection, rec.Normal), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = ri * sinTheta > 1.0;
    Vec3 direction;

    if (cannotRefract || reflectance(cosTheta, ri) > randomFloat())
        direction = reflect(unitDirection, rec.Normal);
    else
        direction = refract(unitDirection, rec.Normal, ri);

    srec.SkipPDFRay = Ray(rec.Point, direction, rayIn.time());
    return true;
}

float Dielectric::reflectance(double cosine, double refractionIndex)
{
    // Use Schlick's approximation for reflectance.
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}