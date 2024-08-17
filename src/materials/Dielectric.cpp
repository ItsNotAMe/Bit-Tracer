#include "Dielectric.h"

bool Dielectric::scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
    float ri = rec.FrontFace ? 1.0f / m_refractionIndex : m_refractionIndex;

    Vec3 unitRayin = unitVector(rayIn.direction());
    float cosTheta = std::fmin(dot(-unitRayin, rec.Normal), 1.0);
    float sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = ri * sinTheta > 1.0;
    Vec3 direction;

    if (cannotRefract || reflectance(cosTheta, ri) > randomFloat())
        direction = reflect(unitRayin, rec.Normal);
    else
        direction = refract(unitRayin, rec.Normal, ri);

    attenuation = Color(1);
    scattered = Ray(rec.Point, direction, rayIn.time());
    return true;
}

float Dielectric::reflectance(double cosine, double refractionIndex)
{
    // Use Schlick's approximation for reflectance.
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}