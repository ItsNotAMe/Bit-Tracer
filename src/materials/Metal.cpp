#include "Metal.h"

bool Metal::scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
    Vec3 reflected = reflect(rayIn.direction(), rec.Normal);
    reflected = unitVector(reflected) + (m_fuzz * randomUnitVector());
    scattered = Ray(rec.Point, reflected, rayIn.time());
    attenuation = m_albedo;
    return (dot(scattered.direction(), rec.Normal) > 0);
}