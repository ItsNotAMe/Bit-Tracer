#include "Lambertian.h"

bool Lambertian::scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
    Vec3 scattedDirection = rec.Normal + randomUnitVector();

    // Catch degenerate scatter direction
    if (scattedDirection.nearZero())
        scattedDirection = rec.Normal;
    
    scattered = Ray(rec.Point, scattedDirection);
    attenuation = m_albedo;
    return true;
}