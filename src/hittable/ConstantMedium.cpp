#include "ConstantMedium.h"

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, float density, std::shared_ptr<Texture> tex)
    : m_boundary(boundary), m_negInvDensity(-1.0f / density), m_phaseFunction(std::make_shared<Isotropic>(tex))
{

}

ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, float density, const Color& albedo)
    : m_boundary(boundary), m_negInvDensity(-1.0f / density), m_phaseFunction(std::make_shared<Isotropic>(albedo))
{

}

bool ConstantMedium::hit(const Ray& r, HitRecord& rec, Interval tRange) const
{
    HitRecord rec1, rec2;

    if (!m_boundary->hit(r, rec1, Interval::universe))
        return false;

    if (!m_boundary->hit(r, rec2, Interval(rec1.t + 0.0001, INF)))
        return false;

    if (rec1.t < tRange.min()) rec1.t = tRange.min();
    if (rec2.t > tRange.max()) rec2.t = tRange.max();

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    auto rayLength = r.direction().length();
    auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
    auto hitDistance = m_negInvDensity * std::log(randomFloat());

    if (hitDistance > distanceInsideBoundary)
        return false;

    rec.t = rec1.t + hitDistance / rayLength;
    rec.Point = r.at(rec.t);

    rec.Normal = Vec3(1, 0, 0); // arbitrary
    rec.FrontFace = true; // also arbitrary
    rec.Mat = m_phaseFunction;

    return true;
}