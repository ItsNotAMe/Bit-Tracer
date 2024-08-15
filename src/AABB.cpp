#include "AABB.h"

const AABB AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);

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