#include "Lambertian.h"

bool Lambertian::scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
    Vec3 scattedDirection = rec.Normal + randomUnitVector();

    // Catch degenerate scatter direction
    if (scattedDirection.nearZero())
        scattedDirection = rec.Normal;
    
    scattered = Ray(rec.Point, scattedDirection, rayIn.time());
    attenuation = m_tex->value(rec.u, rec.v, rec.Point);
    return true;
}