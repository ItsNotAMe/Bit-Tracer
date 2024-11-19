#pragma once
#include "Common.h"

class ONB
{
public:
    ONB(const Vec3& n)
    {
        m_axis[2] = unitVector(n);
        Vec3 a = (std::fabs(m_axis[2].x()) > 0.9f) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
        m_axis[1] = unitVector(cross(m_axis[2], a));
        m_axis[0] = cross(m_axis[2], m_axis[1]);
    }

    const Vec3& u() const { return m_axis[0]; }
    const Vec3& v() const { return m_axis[1]; }
    const Vec3& w() const { return m_axis[2]; }

    Vec3 transform(const Vec3& v) const
    {
        // Transform from basis coordinates to local space.
        return (v[0] * m_axis[0]) + (v[1] * m_axis[1]) + (v[2] * m_axis[2]);
    }
private:
    Vec3 m_axis[3];
};