#include "Sphere.h"

#include "core/ONB.h"

Sphere::Sphere(const Point3& center, float radius, std::shared_ptr<Material> mat)
    : m_center(center), m_radius(std::fmax(0, radius)), m_mat(mat), m_isMoving(false)
{
    Vec3 radiusVec(m_radius);
    m_bbox = AABB(m_center - radiusVec, m_center + radiusVec);
}

Sphere::Sphere(const Point3& center1, const Point3& center2, float radius, std::shared_ptr<Material> mat)
    : m_center(center1), m_radius(std::fmax(0, radius)), m_mat(mat), m_isMoving(true)
{
    m_centerMoveDir = center2 - center1;
    Vec3 radiusVec(m_radius);
    AABB box1(center1 - radiusVec, center1 + radiusVec);
    AABB box2(center2 - radiusVec, center2 + radiusVec);
    m_bbox = AABB(box1, box2);
}

bool Sphere::hit(const Ray& r, HitRecord& rec, Interval tRange) const
{
    Point3 center = m_isMoving ? m_center + r.time() * m_centerMoveDir : m_center;
    Vec3 oc = center - r.origin();
    float a = r.direction().lengthSquared();
    float h = dot(r.direction(), oc);
    float c = oc.lengthSquared() - m_radius * m_radius;

    float discriminant = h * h - a * c;

    if (discriminant < 0)
        return false;

    float sqrtd = std::sqrt(discriminant);

    float t = (h - sqrtd) / a;
    if (!tRange.surrounds(t))
    {
        t = (h + sqrtd) / a;
        if (!tRange.surrounds(t))
            return false;
    }

    rec.t = t;
    rec.Point = r.at(t);
    rec.Mat = m_mat;
    Vec3 outwardNormal = (rec.Point - center) / m_radius;
    rec.setFaceNormal(r, outwardNormal);
    getSphereUV((rec.Point - m_center) / m_radius, rec.u, rec.v);

    return true;
}

float Sphere::pdfValue(const Point3& origin, const Vec3& direction) const
{
    // This method only works for stationary spheres.

    HitRecord rec;
    if (!this->hit(Ray(origin, direction), rec, Interval(0.001, INF)))
        return 0;

    auto cosThetaMax = std::sqrt(1 - m_radius * m_radius / (m_center - origin).lengthSquared());
    auto solidAngle = 2 * PI * (1 - cosThetaMax);

    return  1 / solidAngle;
}

Vec3 Sphere::random(const Point3& origin) const
{
    Vec3 direction = m_center - origin;
    auto distanceSquared = direction.lengthSquared();
    ONB uvw(direction);
    return uvw.transform(randomToSphere(m_radius, distanceSquared));
}

void Sphere::getSphereUV(const Point3& p, float& u, float& v)
{
    float phi = std::atan2(-p.z(), p.x()) + PI;
    float theta = std::acos(-p.y());
    u = phi / (2 * PI);
    v = theta / PI;
}

Vec3 Sphere::randomToSphere(float radius, float distanceSquared)
{
    float r1 = randomFloat();
    float r2 = randomFloat();
    float z = 1 + r2 * (std::sqrt(1 - radius * radius / distanceSquared) - 1);

    float phi = 2 * PI * r1;
    float x = std::cos(phi) * std::sqrt(1 - z * z);
    float y = std::sin(phi) * std::sqrt(1 - z * z);

    return Vec3(x, y, z);
}