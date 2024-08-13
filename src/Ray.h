#pragma once

#include "Vec3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& orig, const Vec3& dir) : m_orig(orig), m_dir(dir) {}

    const Point3& origin() const { return m_orig; }
    const Vec3& direction() const { return m_dir; }

    Point3 at(double t)
    {
        return m_orig + t * m_dir;
    }
private:
    Point3 m_orig;
    Vec3 m_dir;
};