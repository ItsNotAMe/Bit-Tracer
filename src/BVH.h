#pragma once
#include "Common.h"

#include <algorithm>

#include "AABB.h"
#include "Hittable.h"
#include "HittableList.h"

class BVHNode : public Hittable
{
public:
    BVHNode(HittableList list)
        : BVHNode(list.objects(), 0, list.objects().size())
    {
    }
    BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        // Build the bounding box of the span of source objects.
        m_bbox = AABB::empty;
        for (size_t i = start; i < end; i++)
            m_bbox = AABB(m_bbox, objects[i]->boundingBox());

        // Split along the longest axis
        int axis = m_bbox.longestAxis();

        auto comparator = (axis == 0) ? boxCompareX
            : (axis == 1) ? boxCompareY
            : boxCompareZ;

        size_t objectSpan = end - start;

        if (objectSpan == 3)
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            m_left = std::make_shared<BVHNode>(objects, start, start + 2);
            m_right = objects[start + 2];
        }
        else if (objectSpan == 2)
        {
            m_left = objects[start];
            m_right = objects[start + 1];
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            size_t mid = start + objectSpan / 2;
            m_left = std::make_shared<BVHNode>(objects, start, mid);
            m_right = std::make_shared<BVHNode>(objects, mid, end);
        }
    }

    bool hit(const Ray& r, HitRecord& rec, Interval tRay) const override
    {
        if (!m_bbox.hit(r, tRay))
            return false;

        bool hit_left = m_left->hit(r, rec, tRay);
        bool hit_right = m_right->hit(r, rec, Interval(tRay.min(), hit_left ? rec.t : tRay.max()));

        return hit_left || hit_right;
    }

    AABB boundingBox() const override { return m_bbox; }
private:
    static bool boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index)
    {
        auto a_axis_interval = a->boundingBox().axisInterval(axis_index);
        auto b_axis_interval = b->boundingBox().axisInterval(axis_index);
        return a_axis_interval.min() < b_axis_interval.min();
    }

    static bool boxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 0);
    }

    static bool boxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 1);
    }

    static bool boxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return boxCompare(a, b, 2);
    }
private:
    std::shared_ptr<Hittable> m_left;
    std::shared_ptr<Hittable> m_right;
    AABB m_bbox;
};