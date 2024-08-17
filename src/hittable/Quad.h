#pragma once
#include "Common.h"

#include "Hittable.h"

class Quad : public Hittable
{
public:
    Quad(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> mat);

    AABB boundingBox() const override { return m_bbox; }

    bool hit(const Ray& r, HitRecord& rec, Interval tRay) const override;
private:
    Point3 m_Q;
    Vec3 m_u, m_v;
    Vec3 m_w;
    std::shared_ptr<Material> m_mat;
    AABB m_bbox;

    // Plane properties
    Vec3 m_normal;
    float m_D;
};