#pragma once
#include "Common.h"

class AABB
{
public:
    AABB();
    AABB(const Interval& x, const Interval& y, const Interval& z);
    AABB(const Point3& a, const Point3& b);
    AABB(const AABB& box0, const AABB& box1);

    const Interval& axisInterval(int n) const;
    Interval x() const { return m_x; }
    Interval y() const { return m_y; }
    Interval z() const { return m_z; }
    
    Vec3 max() const { return Vec3(m_x.max(), m_y.max(), m_z.max()); }
    Vec3 centroid() const { return (min() + max()) * 0.5f; }
    Vec3 min() const { return Vec3(m_x.min(), m_y.min(), m_z.min()); }
    float surfaceArea();

    bool hit(const Ray& r, Interval tRange) const;

    int longestAxis() const;
private:
    void padToMinimums();
public:
    static const AABB empty, universe;
private:
    Interval m_x, m_y, m_z;
};

AABB operator+(const AABB& bbox, const Vec3& offset);
AABB operator+(const Vec3& offset, const AABB& bbox);