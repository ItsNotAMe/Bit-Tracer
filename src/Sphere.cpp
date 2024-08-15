#include "Sphere.h"

Sphere::Sphere(const Point3& center, float radius, std::shared_ptr<Material> mat)
    : m_center(center), m_radius(std::fmax(0, radius)), m_mat(mat)
{
    Vec3 radiusVec(m_radius);
    m_bbox = AABB(m_center - radiusVec, m_center + radiusVec);
}

bool Sphere::hit(const Ray& r, HitRecord& rec, Interval tRange) const
{
    Vec3 oc = m_center - r.origin();
    float a = r.direction().lengthSquared();
    float h = dot(r.direction(), oc);
    float c = oc.lengthSquared() - m_radius * m_radius;

    float discriminant = h * h - a * c;

    if (discriminant < 0)
        return false;
    
    float sqrtd = std::sqrt(discriminant);

    float t = (h - sqrtd) / a;
    if (!tRange.surrounds(t))
    {
        t = (h + sqrtd) / a;
        if (!tRange.surrounds(t))
            return false;
    }

    rec.t = t;
    rec.Point = r.at(t);
    rec.Mat = m_mat;
    Vec3 outwardNormal = (rec.Point - m_center) / m_radius;
    rec.setFaceNormal(r, outwardNormal);

    return true;
}