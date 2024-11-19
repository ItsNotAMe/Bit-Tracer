#include "Triangle.h"

Triangle::Triangle(const Point3& v1, const Vec3& v2, const Vec3& v3, std::shared_ptr<Material> mat)
    : m_Q(v1), m_u(v2 - v1), m_v(v3 - v1), m_mat(mat)
{
    Vec3 n = cross(m_u, m_v);
    m_normal = unitVector(n);
    m_D = dot(m_normal, m_Q);
    m_w = n / dot(n, n);

    // Compute the bounding box of all four vertices.
    Point3 min = Point3(std::fmin(v1.x(), std::fmin(v2.x(), v3.x())), std::fmin(v1.y(), std::fmin(v2.y(), v3.y())), std::fmin(v1.z(), std::fmin(v2.z(), v3.z())));
    Point3 max = Point3(std::fmax(v1.x(), std::fmax(v2.x(), v3.x())), std::fmax(v1.y(), std::fmax(v2.y(), v3.y())), std::fmax(v1.z(), std::fmax(v2.z(), v3.z())));
    m_bbox = AABB(min - 0.001f, max + 0.001f);

    Vec3 e1 = v2 - v1;
    Vec3 e2 = v3 - v1;
    Vec3 e3 = cross(e1, e2);
    m_area = 0.5f * e3.length();
}

bool Triangle::hit(const Ray& r, HitRecord& rec, Interval tRange) const
{
    float denom = dot(m_normal, r.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < 1e-8)
        return false;

    float t = (m_D - dot(m_normal, r.origin())) / denom;
    if (!tRange.contains(t))
        return false;

    // Determine if the hit point lies within the planar shape using its plane coordinates.
    Point3 intersection = r.at(t);
    Vec3 QToInterVec = intersection - m_Q;
    float alpha = dot(m_w, cross(QToInterVec, m_v));
    float beta = dot(m_w, cross(m_u, QToInterVec));

    // Check if point is in the triangle
    if (alpha < 0 || beta < 0 || alpha + beta >= 1)
        return false;

    rec.u = alpha;
    rec.v = beta;
    
    rec.t = t;
    rec.Point = intersection;
    rec.Mat = m_mat;
    rec.setFaceNormal(r, m_normal);

    return true;
}

float Triangle::pdfValue(const Point3& origin, const Vec3& direction) const
{
    HitRecord rec;
    if (!this->hit(Ray(origin, direction), rec, Interval(0.001, INF)))
        return 0;

    float distanceSquared = rec.t * rec.t * direction.lengthSquared();
    float cosine = std::fabs(dot(direction, rec.Normal) / direction.length());

    return distanceSquared / (cosine * m_area);
}

Vec3 Triangle::random(const Point3& origin) const
{
    float alpha = randomFloat();
    float beta = randomFloat(0, 1 - alpha);
    Point3 p = m_Q + (alpha * m_u) + (beta * m_v);
    return p - origin;
}