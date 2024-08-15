#pragma once
#include "Common.h"

#include "Hittable.h"
#include "materials/Material.h"

class Sphere : public Hittable
{
public:
    Sphere(const Point3& center, float radius, std::shared_ptr<Material> mat);

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override;

    AABB boundingBox() const override { return m_bbox; }
private:
    Point3 m_center;
    float m_radius;
    std::shared_ptr<Material> m_mat;
    AABB m_bbox;
};