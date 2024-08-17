#pragma once
#include "Utility.h"

#include "Vec3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& orig, const Vec3& dir, float time) : m_orig(orig), m_dir(dir), m_time(time) {}
    Ray(const Point3& orig, const Vec3& dir) : Ray(orig, dir, 0) {}

    const Point3& origin() const { return m_orig; }
    const Vec3& direction() const { return m_dir; }
    float time() const { return m_time; }

    Point3 at(float t) const
    {
        return m_orig + t * m_dir;
    }
private:
    Point3 m_orig;
    Vec3 m_dir;
    float m_time;
};