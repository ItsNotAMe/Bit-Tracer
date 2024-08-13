#pragma once

#include <cmath>
#include <iostream>

class Vec3
{
public:
    Vec3() : m_vec{0, 0, 0} {}
    Vec3(float e) : m_vec{e, e, e} {}
    Vec3(float e0, float e1, float e2) : m_vec{e0, e1, e2} {}

    float x() const { return m_vec[0]; }
    float y() const { return m_vec[1]; }
    float z() const { return m_vec[2]; }

    float r() const { return m_vec[0]; }
    float g() const { return m_vec[1]; }
    float b() const { return m_vec[2]; }

    Vec3 operator-() const { return Vec3(-m_vec[0], -m_vec[1], -m_vec[2]); }
    float operator[](int i) const { return m_vec[i]; }
    float &operator[](int i) { return m_vec[i]; }

    Vec3 &operator+=(const Vec3 &other)
    {
        m_vec[0] += other.m_vec[0];
        m_vec[1] += other.m_vec[1];
        m_vec[2] += other.m_vec[2];
        return *this;
    }

    Vec3 &operator*=(float t)
    {
        m_vec[0] *= t;
        m_vec[1] *= t;
        m_vec[2] *= t;
        return *this;
    }

    Vec3 &operator/=(float t)
    {
        return *this *= 1 / t;
    }

    float length() const
    {
        return std::sqrt(lengthSquared());
    }

    float lengthSquared() const
    {
        return m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2];
    }
private:
    float m_vec[3];
};

using Point3 = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
}

inline Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
}

inline Vec3 operator*(float t, const Vec3& v)
{
    return Vec3(t * v[0], t * v[1], t * v[2]);
}

inline Vec3 operator*(const Vec3& v, float t)
{
    return t * v;
}

inline Vec3 operator/(const Vec3& v, float t)
{
    return (1/t) * v;
}

inline float dot(const Vec3& v1, const Vec3& v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1[1] * v2[2] - v1[2] * v2[1],
                v1[2] * v2[0] - v1[0] * v2[2],
                v1[0] * v2[1] - v1[1] * v2[0]);
}

inline Vec3 unitVector(const Vec3& v)
{
    return v / v.length();
}