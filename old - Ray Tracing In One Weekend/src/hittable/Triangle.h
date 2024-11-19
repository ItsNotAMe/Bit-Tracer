#pragma once
#include "Common.h"

#include "Hittable.h"
#include "HittableList.h"

class Triangle : public Hittable
{
public:
    Triangle(const Point3& v1, const Point3& v2, const Point3& v3, std::shared_ptr<Material> mat);

    AABB boundingBox() const override { return m_bbox; }

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override;

    float pdfValue(const Point3& origin, const Vec3& direction) const override;

    Vec3 random(const Point3& origin) const override;
private:
    Point3 m_Q;
    Vec3 m_u, m_v;
    Vec3 m_w;
    std::shared_ptr<Material> m_mat;
    AABB m_bbox;
    float m_area;

    // Plane properties
    Vec3 m_normal;
    float m_D;
};