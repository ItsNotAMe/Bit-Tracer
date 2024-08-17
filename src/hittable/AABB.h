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

    bool hit(const Ray& r, Interval tRay) const;

    int longestAxis() const;
private:
    void padToMinimums();
public:
    static const AABB empty, universe;
private:
    Interval m_x, m_y, m_z;
};