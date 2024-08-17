#include "AABB.h"

const AABB AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);

AABB::AABB() {}

AABB::AABB(const Interval& x, const Interval& y, const Interval& z)
    : m_x(x), m_y(y), m_z(z)
{
    padToMinimums();
}

AABB::AABB(const Point3& a, const Point3& b)
{
    m_x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
    m_y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
    m_z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

    padToMinimums();
}

AABB::AABB(const AABB& box0, const AABB& box1)
{
    m_x = Interval(box0.m_x, box1.m_x);
    m_y = Interval(box0.m_y, box1.m_y);
    m_z = Interval(box0.m_z, box1.m_z);
}

const Interval& AABB::axisInterval(int n) const
{
    if (n == 0) return m_x;
    if (n == 1) return m_y;
    return m_z;
}

bool AABB::hit(const Ray& r, Interval tRay) const
{

    for (int axis = 0; axis < 3; axis++)
    {
        const Interval& ax = axisInterval(axis);
        const float adinv = 1.0 / r.direction()[axis];

        float t0 = (ax.min() - r.origin()[axis]) * adinv;
        float t1 = (ax.max() - r.origin()[axis]) * adinv;

        if (t0 < t1)
        {
            if (t0 > tRay.min()) tRay.min() = t0;
            if (t1 < tRay.max()) tRay.max() = t1;
        }
        else
        {
            if (t1 > tRay.min()) tRay.min() = t1;
            if (t0 < tRay.max()) tRay.max() = t0;
        }

        if (tRay.max() <= tRay.min())
            return false;
    }
    return true;
}

int AABB::longestAxis() const
{
    // Returns the index of the longest axis of the bounding box.
    if (m_x.size() > m_y.size())
        return m_x.size() > m_z.size() ? 0 : 2;
    else
        return m_y.size() > m_z.size() ? 1 : 2;
}

void AABB::padToMinimums()
{
    // Adjust the AABB so that no side is narrower than some delta, padding if necessary.

    float delta = 0.0001f;
    if (m_x.size() < delta) m_x = m_x.expand(delta);
    if (m_y.size() < delta) m_y = m_y.expand(delta);
    if (m_z.size() < delta) m_z = m_z.expand(delta);
}