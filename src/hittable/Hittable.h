#pragma once
#include "Common.h"

#include "AABB.h"

class Material;

struct HitRecord
{
    Point3 Point;
    Vec3 Normal;
    std::shared_ptr<Material> Mat;
    float t;
    float u, v;
    bool FrontFace;

    void setFaceNormal(const Ray& r, const Vec3& outwardNormal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        FrontFace = dot(r.direction(), outwardNormal) < 0;
        Normal = FrontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, HitRecord& rec, Interval tRange) const = 0;

    virtual AABB boundingBox() const = 0;
};

class Translate : public Hittable
{
public:
    Translate(std::shared_ptr<Hittable> object, const Vec3& offset)
        : m_object(object), m_offset(offset)
    {
        bbox = object->boundingBox() + offset;
    }

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override
    {
        // Move the ray backwards by the offset
        Ray offsetRay(r.origin() - m_offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!m_object->hit(offsetRay, rec, tRange))
            return false;

        // Move the intersection point forwards by the offset
        rec.Point += m_offset;

        return true;
    }

    AABB boundingBox() const override { return bbox; }
private:
    std::shared_ptr<Hittable> m_object;
    Vec3 m_offset;
    AABB bbox;
};

class RotateY : public Hittable
{
public:
    RotateY(std::shared_ptr<Hittable> object, double angle) : m_object(object)
    {
        auto radians = degreesToRadians(angle);
        m_sinTheta = std::sin(radians);
        m_cosTheta = std::cos(radians);
        m_bbox = object->boundingBox();

        Point3 min(INF, INF, INF);
        Point3 max(-INF, -INF, -INF);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * m_bbox.x().max() + (1 - i) * m_bbox.x().min();
                    auto y = j * m_bbox.y().max() + (1 - j) * m_bbox.y().min();
                    auto z = k * m_bbox.z().max() + (1 - k) * m_bbox.z().min();

                    auto newX = m_cosTheta * x + m_sinTheta * z;
                    auto newZ = -m_sinTheta * x + m_cosTheta * z;

                    Vec3 tester(newX, y, newZ);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        m_bbox = AABB(min, max);
    }

    bool hit(const Ray& r, HitRecord& rec, Interval tRange) const override
    {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = m_cosTheta * r.origin()[0] - m_sinTheta * r.origin()[2];
        origin[2] = m_sinTheta * r.origin()[0] + m_cosTheta * r.origin()[2];

        direction[0] = m_cosTheta * r.direction()[0] - m_sinTheta * r.direction()[2];
        direction[2] = m_sinTheta * r.direction()[0] + m_cosTheta * r.direction()[2];

        Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where)
        if (!m_object->hit(rotated_r, rec, tRange))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.Point;
        p[0] = m_cosTheta * rec.Point[0] + m_sinTheta * rec.Point[2];
        p[2] = -m_sinTheta * rec.Point[0] + m_cosTheta * rec.Point[2];

        // Change the normal from object space to world space
        auto normal = rec.Normal;
        normal[0] = m_cosTheta * rec.Normal[0] + m_sinTheta * rec.Normal[2];
        normal[2] = -m_sinTheta * rec.Normal[0] + m_cosTheta * rec.Normal[2];

        rec.Point = p;
        rec.Normal = normal;

        return true;
    }

    AABB boundingBox() const override { return m_bbox; }

private:
    std::shared_ptr<Hittable> m_object;
    double m_sinTheta;
    double m_cosTheta;
    AABB m_bbox;
};