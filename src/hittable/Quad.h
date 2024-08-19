#pragma once
#include "Common.h"

#include "Hittable.h"
#include "HittableList.h"

class Quad : public Hittable
{
public:
    Quad(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> mat);

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

inline std::shared_ptr<HittableList> box(const Point3& a, const Point3& b, std::shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = std::make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Point3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
    auto max = Point3(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

    auto dx = Vec3(max.x() - min.x(), 0, 0);
    auto dy = Vec3(0, max.y() - min.y(), 0);
    auto dz = Vec3(0, 0, max.z() - min.z());

    sides->add(std::make_shared<Quad>(Point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(std::make_shared<Quad>(Point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(std::make_shared<Quad>(Point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(std::make_shared<Quad>(Point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(std::make_shared<Quad>(Point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(std::make_shared<Quad>(Point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}