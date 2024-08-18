#include "Quad.h"

Quad::Quad(const Point3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> mat)
    : m_Q(Q), m_u(u), m_v(v), m_mat(mat)
{
    Vec3 n = cross(u, v);
    m_normal = unitVector(n);
    m_D = dot(m_normal, Q);
    m_w = n / dot(n, n);

    // Compute the bounding box of all four vertices.
    auto bboxDiagonal1 = AABB(Q, Q + u + v);
    auto bboxDiagonal2 = AABB(Q + u, Q + v);
    m_bbox = AABB(bboxDiagonal1, bboxDiagonal2);
}

bool Quad::hit(const Ray& r, HitRecord& rec, Interval tRange) const
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

    // Check if point is in the quad
    Interval unit(0, 1);
    if (!unit.contains(alpha) || !unit.contains(beta))
        return false;

    rec.u = alpha;
    rec.v = beta;
    
    rec.t = t;
    rec.Point = intersection;
    rec.Mat = m_mat;
    rec.setFaceNormal(r, m_normal);

    return true;
}
