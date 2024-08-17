#pragma once
#include "Utility.h"

class Interval
{
public:
    Interval() : m_min(+INF), m_max(-INF) {}
    Interval(float min, float max) : m_min(min), m_max(max) {}
    Interval(const Interval& a, const Interval& b)
    {
        // Create the interval tightly enclosing the two input intervals.
        m_min = a.m_min <= b.m_min ? a.m_min : b.m_min;
        m_max = a.m_max >= b.m_max ? a.m_max : b.m_max;
    }

    float& min() { return m_min; }
    float& max() { return m_max; }
    float min() const { return m_min; }
    float max() const { return m_max; }

    float size() const { return m_max - m_min; }

    bool contains(float x) const { return m_min <= x && x <= m_max; }
    bool surrounds(float x) const { return m_min < x && x < m_max; }

    float clamp(float x) const;
    Interval expand(float delta) const;
public:
    static const Interval empty, universe;
private:
    float m_min, m_max;
};