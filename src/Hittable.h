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