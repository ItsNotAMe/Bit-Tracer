#pragma once
#include "Utility.h"

class Vec3
{
public:
    Vec3() : m_vec{ 0, 0, 0 } {}
    Vec3(float e) : m_vec{ e, e, e } {}
    Vec3(float e0, float e1, float e2) : m_vec{ e0, e1, e2 } {}

    float& x() { return m_vec[0]; }
    float& y() { return m_vec[1]; }
    float& z() { return m_vec[2]; }
    float x() const { return m_vec[0]; }
    float y() const { return m_vec[1]; }
    float z() const { return m_vec[2]; }

    float& r() { return m_vec[0]; }
    float& g() { return m_vec[1]; }
    float& b() { return m_vec[2]; }
    float r() const { return m_vec[0]; }
    float g() const { return m_vec[1]; }
    float b() const { return m_vec[2]; }

    Vec3 operator-() const { return Vec3(-m_vec[0], -m_vec[1], -m_vec[2]); }
    float operator[](int i) const { return m_vec[i]; }
    float& operator[](int i) { return m_vec[i]; }

    Vec3& operator+=(const Vec3& other)
    {
        m_vec[0] += other.m_vec[0];
        m_vec[1] += other.m_vec[1];
        m_vec[2] += other.m_vec[2];
        return *this;
    }

    Vec3& operator*=(float t)
    {
        m_vec[0] *= t;
        m_vec[1] *= t;
        m_vec[2] *= t;
        return *this;
    }

    Vec3& operator/=(float t)
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

    static Vec3 random()
    {
        return Vec3(randomFloat(), randomFloat(), randomFloat());
    }

    static Vec3 random(float min, float max)
    {
        return Vec3(randomFloat(min, max), randomFloat(min, max), randomFloat(min, max));
    }

    bool nearZero() const
    {
        // Return true if the vector is close to zero in all dimensions.
        float s = 1e-8;
        return std::fabs(m_vec[0]) < s && std::fabs(m_vec[1]) < s && std::fabs(m_vec[2]) < s;
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
    return (1 / t) * v;
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

inline Vec3 randomInUnitDisk()
{
    while (true)
    {
        Vec3 p = Vec3(randomFloat(-1, 1), randomFloat(-1, 1), 0);
        if (p.lengthSquared() < 1)
            return p;
    }
}

/*inline Vec3 randomInUnitSphere()
{
    while (true)
    {
        Vec3 p = Vec3::random(-1, 1);
        if (p.lengthSquared() < 1)
            return p;
    }
}*/

inline Vec3 randomUnitVector()
{
    //return unitVector(randomInUnitSphere());
    return unitVector(Vec3::random(-1, 1));
}

inline Vec3 randomOnHemisphere(const Vec3& normal)
{
    Vec3 onUnitSphere = randomUnitVector();
    if (dot(onUnitSphere, normal) > 0.0) // In the same hemisphere as the normal
        return onUnitSphere;
    else
        return -onUnitSphere;
}

inline Vec3 randomCosineDirection()
{
    float r1 = randomFloat();
    float r2 = randomFloat();

    float phi = 2 * PI * r1;
    float x = std::cos(phi) * std::sqrt(r2);
    float y = std::sin(phi) * std::sqrt(r2);
    float z = std::sqrt(1 - r2);

    return Vec3(x, y, z);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
    return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
{
    auto cosTheta = std::fmin(dot(-uv, n), 1.0);
    Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
    Vec3 rOutParallel = -std::sqrt(std::fabs(1.0 - rOutPerp.lengthSquared())) * n;
    return rOutPerp + rOutParallel;
}