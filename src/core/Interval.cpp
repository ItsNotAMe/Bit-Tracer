#include "Interval.h"

const Interval Interval::empty = Interval(INF, -INF);
const Interval Interval::universe = Interval(-INF, INF);

float Interval::clamp(float x) const
{
    if (x < m_min)
        return m_min;
    if (x > m_max)
        return m_max;
    return x;
}

Interval Interval::expand(float delta) const
{
    float padding = delta / 2;
    return Interval(m_min - padding, m_max + padding);
}