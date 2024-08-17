#include "Sphere.h"

Sphere::Sphere(const Point3& center, float radius, std::shared_ptr<Material> mat)
    : m_center(center), m_radius(std::fmax(0, radius)), m_mat(mat), m_isMoving(false)
{
    Vec3 radiusVec(m_radius);
    m_bbox = AABB(m_center - radiusVec, m_center + radiusVec);
}

Sphere::Sphere(const Point3& center1, const Point3& center2, float radius, std::shared_ptr<Material> mat)
    : m_center(center1), m_radius(std::fmax(0, radius)), m_mat(mat), m_isMoving(true)
{
    m_centerMoveDir = center2 - center1;
    Vec3 radiusVec(m_radius);
    AABB box1(center1 - radiusVec, center1 + radiusVec);
    AABB box2(center2 - radiusVec, center2 + radiusVec);
    m_bbox = AABB(box1, box2);
}

bool Sphere::hit(const Ray& r, HitRecord& rec, Interval tRange) const
{
    Point3 center = m_isMoving ? m_center + r.time() * m_centerMoveDir : m_center;
    Vec3 oc = center - r.origin();
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
    Vec3 outwardNormal = (rec.Point - center) / m_radius;
    rec.setFaceNormal(r, outwardNormal);

    return true;
}