#pragma once
#include "Common.h"

class AABB
{
public:
    AABB() {}
    AABB(const Interval& x, const Interval& y, const Interval& z)
        : m_x(x), m_y(y), m_z(z)
    {
    }
    AABB(const Point3& a, const Point3& b)
    {
        m_x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        m_y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        m_z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
    }
    AABB(const AABB& box0, const AABB& box1)
    {
        m_x = Interval(box0.m_x, box1.m_x);
        m_y = Interval(box0.m_y, box1.m_y);
        m_z = Interval(box0.m_z, box1.m_z);
    }

    const Interval& axisInterval(int n) const;

    bool hit(const Ray& r, Interval tRay) const;

    int longestAxis() const;
public:
    static const AABB empty, universe;
private:
    Interval m_x, m_y, m_z;
};